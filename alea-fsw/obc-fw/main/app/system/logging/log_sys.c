/**
 * @file log_sys.c
 * @brief OBC logging module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "log_sys.h"
#include "log_sys_serial.h"
#include "logger.h"

// OBC
#include "obc_featuredefs.h"
#include "obc_serial_tx.h"
#include "obc_rtc.h"
#include "obc_utils.h"
#include "obc_mram.h"
#include "obc_filesystem.h"

// Utils
#include "data_fmt.h"
#include "io_stream.h"

// FreeRTOS
#include "rtos.h"

// Third-Party
#include "printf.h"

// Standard Library
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define HEADER_SIZE       7U     ///< Size of header in bytes
#define SERIAL_TIMEOUT_MS 500U

#define LOG_PRINTF_LOG_ID 0
#define LOG_PRINTF_SIG_ID 0

#define LOG_CMD_SYS_SCHED_RESP_LOG_ID 67
#define LOG_CMD_SYS_SCHED_RESP_SIG_ID 0

#define LOG_SYS_MUTEX_TIMEOUT_MS 2000

#define LOG_DELIMITER_SIZE  4

typedef struct {
    uint8_t log_id;
    uint8_t sig_id;
} log_sig_pair_t;

static const log_sig_pair_t log_sig_pairs[] = {
    {  0, 0 }, // PRINTF
    { 74, 0 }, // SYS_WRITE_ENQUEUE_FAIL
    { 74, 1 }, // SYS_FS_FAIL
    { 39, 1 }, // WATCHDOG_PET
    { 39, 2 }, // WATCHDOG_HAPPY
};
static const size_t n_pairs = sizeof(log_sig_pairs) / sizeof(log_sig_pairs[0]);

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void log_signal_internal(uint8_t log_id, uint8_t sig_id, size_t raw_payload_len, const void *payload_ptr, bool use_rtc);
static void encode_header(uint8_t log_id, uint8_t sig_id, uint8_t size, uint8_t header_ptr[HEADER_SIZE], bool use_rtc);
static bool get_actual_file_sizes(uint32_t *arr);
static void fs_log_save(uint8_t *buffer, size_t buffer_len);
static inline bool saveable_log(uint8_t log_id, uint8_t sig_id);
static void log_2_fs_init(void);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static lfs_file_t file = { 0 };

static const char *LOG_FILE_NAMES[] = {"log_0", "log_1", "log_2", "log_3"};
static uint8_t log_file_current;

// This doesn't reflect the actual size of the current logfile in flash, since it is updated on calls to
// fs_write_enqueue. Thus, in general, the size reflected by this variable will be slightly larger than
// the actual file size between FS flush cycles
static uint32_t log_file_current_size;

static const uint8_t LOG_DELIMITER[] = {0xBE, 0xEE, 0xEE, 0xEF};

static SemaphoreHandle_t logSysMutex;

static bool log_2_flash_initialized = false;

/* log_sys internal variables to keep track of read state for cmd_impl_GET_LOGS() */
typedef struct {
    bool reading;
    uint8_t *buf;
    uint32_t total_bytes_remaining;
    uint16_t chunk_size;
    uint8_t readfile_index;
    uint32_t readfile_bytes_remaining;
    uint32_t file_sizes[FS_LOGGING_FILES_NUM];
    lfs_file_t readfile;
} log_read_state_t;

static log_read_state_t log_read = { 0 };

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/* This is a special LOG_ function used as a debug printf */
void LOG_PRINTF(char *format, ...) {
    log_LOG_PRINTF__reserved_data_t log_data = { 0 };

    va_list args_ptr;
    va_start(args_ptr, format);
    vsnprintf((char *)log_data.message, sizeof(log_data.message), format, args_ptr);
    va_end(args_ptr);

    const data_fmt_field_desc_t data_field_descs[1] = {
        { .type = DATA_FMT_FIELD_TYPE_U8, .struct_offset = offsetof(log_LOG_PRINTF__reserved_data_t, message), .array_len = sizeof(log_data.message) },
    };

    const data_fmt_desc_t data_desc = { .fields = data_field_descs, .count = 1 };

    log_sys_log_signal_with_data(LOG_PRINTF_LOG_ID, LOG_PRINTF_SIG_ID, sizeof(log_data.message), (const void *)&log_data, &data_desc);
}

/* This is a special LOG_ function used for cmd system schedule responses */
void LOG_CMD_SYS_SCHED_RESP(uint32_t num_bytes, const uint8_t *data) {
    log_signal_internal(LOG_CMD_SYS_SCHED_RESP_LOG_ID, LOG_CMD_SYS_SCHED_RESP_SIG_ID, num_bytes, data, true);
}

void log_sys_pre_init(void) {
    log_sys_serial_create_infra();
    log_sys_serial_create_task();
}

void log_sys_post_init(void) {
    log_2_fs_init();
}

/**
 * @brief Public API to initialize a log read session. To be used by cmd_impl_GET_LOGS
 *
 * The sizes of all logfiles are retrieved and cached, @ref log_read is initialized
 *
 * @param[in]   buf              External buffer to save to @ref log_read, written to in @ref logs_read()
 * @param[in]   bytes_to_read    Number of bytes being requested
 * @param[in]   chunk_size       Maximum number of bytes that can be written to @ref buf
 */
int32_t logs_read_begin(uint8_t *buf, uint32_t bytes_to_read, uint16_t chunk_size) {
    if (buf == NULL || bytes_to_read > FS_LOGGING_PARTITION_SIZE || !log_2_flash_initialized) {
        return -1;
    }

    if (xSemaphoreTake(logSysMutex, pdMS_TO_TICKS(LOG_SYS_MUTEX_TIMEOUT_MS)) == pdTRUE) {
        if (log_read.reading) {
            xSemaphoreGive(logSysMutex);
            return -1;
        }

        if (!get_actual_file_sizes(log_read.file_sizes)) {
            xSemaphoreGive(logSysMutex);
            return -1;
        }

        int32_t total_size = 0;

        // In this case, we actually open and retrieve the size of the current logfile, since log_file_current_size
        // doesn't reflect the size of the actual file in flash. log_file_current_size reflects the current size of
        // that file PLUS the number of bytes currently waiting to be written to the file via fs_write_enqueue
        for (int x = 0; x < FS_LOGGING_FILES_NUM; x++) {
            total_size += log_read.file_sizes[x];
        }

        log_read.readfile_index = log_file_current;
        const char *filename = LOG_FILE_NAMES[log_file_current];

        if (fs_open(&log_read.readfile, filename, LOG_SYS_MUTEX_TIMEOUT_MS) != FS_OK) {
            xSemaphoreGive(logSysMutex);
            return -1;
        }

        if (fs_seek(&log_read.readfile, 0, FS_SEEK_END) != FS_OK) {
            xSemaphoreGive(logSysMutex);
            return -1;
        }

        log_read.buf = buf;
        log_read.total_bytes_remaining = bytes_to_read;
        log_read.readfile_bytes_remaining = log_read.file_sizes[log_file_current];
        log_read.chunk_size = chunk_size;

        log_read.reading = true;

        xSemaphoreGive(logSysMutex);

        return MIN((uint32_t)total_size, bytes_to_read);
    }

    return -1;
}

/**
 * @brief Public API to de-initialize a log read session. To be used by cmd_impl_GET_LOGS after @ref logs_read_begin()
 */
void logs_read_end() {
    if (xSemaphoreTake(logSysMutex, pdMS_TO_TICKS(LOG_SYS_MUTEX_TIMEOUT_MS)) == pdTRUE) {
        if (!log_read.reading) {
            xSemaphoreGive(logSysMutex);
            return;
        }

        log_read.reading = false;

        fs_err_t err;
        err = fs_close(&log_read.readfile);

        if (err != FS_OK) {
            LOG_LOG_SYS__FS_FAIL((int8_t)err);
        }

        xSemaphoreGive(logSysMutex);
    }
}

/**
 * @brief Public API to retrieve and write bytes from logfile into external buffer defined with @ref logs_read_begin()
 *
 * Walks backwards through logfiles, starting at the end of the current logfile and reading backwards in order to
 * retrieve most recent logs first
 *
 * State persists between calls via the @ref log_read struct
 *
 * @return Number of bytes written to @ref log_read.buf if successful, -1 otherwise
 */
int32_t logs_read() {
    if (xSemaphoreTake(logSysMutex, pdMS_TO_TICKS(LOG_SYS_MUTEX_TIMEOUT_MS)) == pdTRUE) {
        // If we have reached the beginning of the current logfile
        if (log_read.readfile_bytes_remaining == 0) {
            if (fs_close(&log_read.readfile) != FS_OK) {
                log_read.reading = false;
                xSemaphoreGive(logSysMutex);
                return -1;
            }

            // Move on to the previous logfile before the current one we're reading from
            log_read.readfile_index = (log_read.readfile_index == 0) ? (FS_LOGGING_FILES_NUM - 1) : (log_read.readfile_index - 1);
            const char *filename = LOG_FILE_NAMES[log_read.readfile_index];

            if (fs_open(&log_read.readfile, filename, LOG_SYS_MUTEX_TIMEOUT_MS) != FS_OK) {
                log_read.reading = false;
                xSemaphoreGive(logSysMutex);
                return -1;
            }

            if (fs_seek(&log_read.readfile, 0, FS_SEEK_END) != FS_OK) {
                log_read.reading = false;
                xSemaphoreGive(logSysMutex);
                return -1;
            }

            uint32_t file_size = log_read.file_sizes[log_read.readfile_index];

            if (file_size == 0) {
                xSemaphoreGive(logSysMutex);
                return 0;
            }

            log_read.readfile_bytes_remaining = file_size;
        }

        // Number of bytes to read to the buffer is the minimum of the total bytes that still need to be read, buffer chunk size, and
        // bytes left in current file
        uint32_t bytes_to_read = MIN(MIN(log_read.chunk_size, log_read.readfile_bytes_remaining), log_read.total_bytes_remaining);

        if (fs_seek(&log_read.readfile, -(int32_t)bytes_to_read, FS_SEEK_CUR) != FS_OK) {
            log_read.reading = false;
            xSemaphoreGive(logSysMutex);
            return -1;
        }

        if (fs_read(&log_read.readfile, log_read.buf, bytes_to_read) != FS_OK) {
            log_read.reading = false;
            xSemaphoreGive(logSysMutex);
            return -1;
        }

        if (fs_seek(&log_read.readfile, -(int32_t)bytes_to_read, FS_SEEK_CUR) != FS_OK) {
            log_read.reading = false;
            xSemaphoreGive(logSysMutex);
            return -1;
        }

        log_read.readfile_bytes_remaining -= bytes_to_read;
        log_read.total_bytes_remaining -= bytes_to_read;

        xSemaphoreGive(logSysMutex);

        return (int32_t)bytes_to_read;
    }

    return 0;
}

/**
 * @brief Retrive the current logfile index and all logfile sizes
 *
 * @param[out]  info        Pointer to the external structure where the data should be stored
 *
 * @return true on success, false on failure
 */
bool log_sys_get_info(struct logfile_info_t *info) {
    if (!info || !log_2_flash_initialized) {
        return false;
    }

    if (xSemaphoreTake(logSysMutex, pdMS_TO_TICKS(LOG_SYS_MUTEX_TIMEOUT_MS)) == pdTRUE) {
        info->current_logfile = log_file_current;

        if (!get_actual_file_sizes(info->logfile_sizes)) {
            xSemaphoreGive(logSysMutex);
            return false;
        }

        xSemaphoreGive(logSysMutex);
        return true;
    }

    return false;
}

void log_sys_log_signal(uint8_t log_id, uint8_t sig_id) {
    log_signal_internal(log_id, sig_id, 0, NULL, true);
}

void log_sys_log_signal_with_data(uint8_t log_id, uint8_t sig_id, uint32_t data_len, const void *data_struct, const data_fmt_desc_t *data_desc) {
    static uint8_t buf[512]; // TODO(ALEA-3185): This buffer is not thread safe

    if ((data_struct == NULL) || (data_desc == NULL)) {
        LOG_LOGGING_SYS__ATTEMPTED_TO_LOG_DATA_NULL();
        return;
    }

    // Serialize the response
    uint32_t written_bytes = data_fmt_serialize_struct(data_struct, data_desc, buf, sizeof(buf));

    if (written_bytes != data_len) {
        LOG_LOGGING_SYS__DATA_FMT_WRONG_DATA_LEN();
        return;
    }

    log_signal_internal(log_id, sig_id, written_bytes, buf, true);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Initialize logging to filesystem
 *
 * Create log2fs infrastructure and find the current file to log to (smallest logfile)
 */
static void log_2_fs_init(void) {
#if FEATURE_FLASH_FS
    static StaticSemaphore_t logMutexBuffer;
    logSysMutex = xSemaphoreCreateMutexStatic(&logMutexBuffer);

    const char *filename;

    log_file_current = 0;
    log_file_current_size = 0;

    int32_t smallest = FS_LOGGING_MAX_FILE_SIZE;

    for (int filenum = 0; filenum < FS_LOGGING_FILES_NUM; filenum++) {
        filename = LOG_FILE_NAMES[filenum];
        fs_open(&file, filename, 1000);
        int32_t size = fs_size(&file);

        if (size > 0 && size < smallest) {
            log_file_current = filenum;
            log_file_current_size = (uint32_t)size;
            smallest = (uint32_t)size;
        }

        fs_close(&file);
    }

    log_read.reading = false;
    log_2_flash_initialized = true;
#endif
}

/**
 * @brief Retrieve and write all logfile sizes to an external array
 *
 * This function assumes that the caller already holds @ref logSysMutex
 *
 * @param[out]  arr     A pointer to an external array to write to. Must be of size FS_LOGGING_FILES_NUM
 *
 * @return true on success, false on failure
 */
static bool get_actual_file_sizes(uint32_t *arr) {
    const char *filename;

    for (int filenum = 0; filenum < FS_LOGGING_FILES_NUM; filenum++) {
        filename = LOG_FILE_NAMES[filenum];

        if (fs_open(&file, filename, LOG_SYS_MUTEX_TIMEOUT_MS) != FS_OK) {
            return false;
        }

        int32_t file_size = fs_size(&file);

        if (file_size < 0) {
            return false;
        }

        arr[filenum] = (uint32_t)file_size;

        if (fs_close(&file) != FS_OK) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Queue log messages to be saved to FS
 *
 * Determines if the current logfile has the capacity for the incoming logfile. If the current logfile is
 * at capacity, moves on to the next logfile and zeroes the file
 *
 * @param[in]   buffer          Pointer to a buffer containing a log message
 * @param[in]   buffer_len      Size of the incoming log message
 */
static void fs_log_save(uint8_t *buffer, size_t buffer_len) {
    const char *filename = NULL;
    fs_err_t err;

    if (xSemaphoreTake(logSysMutex, pdMS_TO_TICKS(LOG_SYS_MUTEX_TIMEOUT_MS)) == pdTRUE) {
        // Saving logs to FS is disabled during downlink
        if (log_read.reading) {
            xSemaphoreGive(logSysMutex);
            return;
        }

        // If size of the log message is greater than the remaining capacity of the current logfile
        if (buffer_len > (FS_LOGGING_MAX_FILE_SIZE - log_file_current_size)) {
            log_file_current = (log_file_current == FS_LOGGING_FILES_NUM - 1) ? 0 : log_file_current + 1;
            log_file_current_size = buffer_len;

            filename = LOG_FILE_NAMES[log_file_current];

            err = fs_open(&file, filename, LOG_SYS_MUTEX_TIMEOUT_MS);

            if (err) {
                LOG_LOG_SYS__FS_FAIL((int8_t)err);
                xSemaphoreGive(logSysMutex);
                return;
            }

            err = fs_zero(&file);

            if (err != FS_OK) {
                LOG_LOG_SYS__FS_FAIL((int8_t)err);
                xSemaphoreGive(logSysMutex);
                return;
            }

            err = fs_close(&file);

            if (err != FS_OK) {
                LOG_LOG_SYS__FS_FAIL((int8_t)err);
                xSemaphoreGive(logSysMutex);
                return;
            }
        } else {
            filename = LOG_FILE_NAMES[log_file_current];
            log_file_current_size += buffer_len;
        }

        xSemaphoreGive(logSysMutex);

        err = fs_write_enqueue(buffer, buffer_len, filename, LOG_SYS_MUTEX_TIMEOUT_MS);

        if (err != FS_OK) {
            LOG_LOG_SYS__WRITE_ENQUEUE_FAIL((int8_t)err);
        }
    }
}

/**
 * @brief Check if a log is in the save blacklist (log_sig_pairs)
 *
 * @param[in]   log_id      The log ID to check
 * @param[in]   sig_id      The sig ID of the incoming log
 *
 * @return true if the log is saveable, false if not
 */
static inline bool saveable_log(uint8_t log_id, uint8_t sig_id) {
    for (size_t x = 0; x < n_pairs; x++) {
        if ((log_id == log_sig_pairs[x].log_id) && (sig_id == log_sig_pairs[x].sig_id)) {
            return false;
        }
    }

    return true;
}

static void log_signal_internal(uint8_t log_id, uint8_t sig_id, size_t raw_payload_len, const void *payload_ptr, bool use_rtc) {
    // Determine payload len -> if too large, cap at MAX_PAYLOAD_SIZE
    uint8_t payload_len;

    if (raw_payload_len <= MAX_PAYLOAD_SIZE) {
        payload_len = raw_payload_len;
    } else {
        payload_len = MAX_PAYLOAD_SIZE;
        LOG_LOGGER__PAYLOAD_TOO_LARGE();
    }

    uint8_t message_buf[LOG_DELIMITER_SIZE + HEADER_SIZE + MAX_PAYLOAD_SIZE] = {0}; // TODO(ALEA-3185): This buffer can cause some tasks to overflow

    // Write the log delimiter to the beginning of the message buffer (for log2fs)
    for (int x = 0; x < LOG_DELIMITER_SIZE; x++) {
        message_buf[x] = LOG_DELIMITER[x];
    }

    // Populate the header in the output buffer
    encode_header(log_id, sig_id, payload_len, &message_buf[LOG_DELIMITER_SIZE], use_rtc);

    // If this message has a payload...
    if ((payload_len > 0) && (payload_ptr != NULL)) {
        // Populate the payload data in the output buffer
        memcpy(&message_buf[LOG_DELIMITER_SIZE + HEADER_SIZE], payload_ptr, payload_len);
    }

    const size_t message_size = HEADER_SIZE + payload_len;
    log_sys_serial_write_log(&message_buf[LOG_DELIMITER_SIZE], message_size, pdMS_TO_TICKS(SERIAL_TIMEOUT_MS));

    if (log_2_flash_initialized && saveable_log(log_id, sig_id) /* TODO: get severity level? */) {
        fs_log_save(message_buf, LOG_DELIMITER_SIZE + message_size);
    }
}

/**
 * @brief Encodes the message header.
 *
 * @param[in]  lvl The log level to use (DEBUG, WARNING, INFO, ERROR).
 * @param[in]  log_id The log group ID to use.
 * @param[in]  sig_id The signal ID to use.
 * @param[in]  size The size of the payload (bytes).
 * @param[out] header_ptr Pointer to the top of the message buffer. The header will be placed here.
 */
static void encode_header(uint8_t log_id, uint8_t sig_id, uint8_t size, uint8_t header_ptr[HEADER_SIZE], bool use_rtc) {
    epoch_t current_epoch = 0;

    // Grab a timestamp if the logger is configured to read from the RTC.
    // TODO ALEA-357 Handle RTC errors in the RTC module (shouldn't need this switch)
    if (use_rtc) {
        current_epoch = rtc_get_epoch_time();
    }

    // byte 0-3: epoch (big-endian)
    data_fmt_u32_to_arr_be((uint32_t) current_epoch, header_ptr);
    // byte 4:  log_id
    header_ptr[4] = (uint8_t)log_id;
    // byte 5: sig_id
    header_ptr[5] = (uint8_t)sig_id;
    // byte 6: size
    header_ptr[6] = (uint8_t)size;
}
