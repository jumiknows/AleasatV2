/**
 * @file log_sys.c
 * @brief OBC logging module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "log_sys.h"
#include "logger.h"

// OBC
#include "obc_serial_tx.h"
#include "obc_rtc.h"
#include "obc_utils.h"

// Utils
#include "data_fmt.h"
#include "io_stream.h"

// FreeRTOS
#include "rtos.h"

// HAL
#include "tms_uart.h"

// Third-Party
#include "printf.h"

// Standard Library
#include <stdarg.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define HEADER_SIZE                 7U     ///< Size of header in bytes
#define LOGGER_MUTEX_TIMEOUT_MS     500U   ///< Logger mutex acquire timeout in milliseconds

#define SERIAL_TIMEOUT_MS           100U

#define LOG_PRINTF_LOG_ID 0
#define LOG_PRINTF_SIG_ID 0

#define LOG_CMD_SYS_SCHED_RESP_LOG_ID 67
#define LOG_CMD_SYS_SCHED_RESP_SIG_ID 0

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief Mutex to prevent multiple tasks from logging at the same time.
 *
 * If one task was swapped in while another was using the logger, one or more of the following bad
 * things could happen:
 *     1. The message from the beginning task could be interleaved with the message from the
 *        later task in the UART transmission or file write.
 *     2. The @ref use_rtc status could become invalid.
 *     3. General corruption of the message to be logged.
 *
 * This is a recursive mutex, meaning it can be taken multiple times by the same task. This allows
 * nested logger calls, such as the logger logging a message when it finds a payload too large.
 */
static SemaphoreHandle_t logger_mutex = NULL;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void log_signal_internal(uint8_t log_id, uint8_t sig_id, size_t raw_payload_len, const void* payload_ptr, bool use_rtc);
static void encode_header(uint8_t log_id, uint8_t sig_id, uint8_t size, uint8_t header_ptr[HEADER_SIZE], bool use_rtc);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/* This is a special LOG_ function used as a debug printf */
void LOG_PRINTF(char* format, ...) {
    log_LOG_PRINTF__reserved_data_t log_data = { 0 };

    va_list args_ptr;
    va_start(args_ptr, format);
    vsnprintf((char*)log_data.message, sizeof(log_data.message), format, args_ptr);
    va_end(args_ptr);

    const data_fmt_field_desc_t data_field_descs[1] = {
      { .type = DATA_FMT_FIELD_TYPE_U8, .struct_offset = offsetof(log_LOG_PRINTF__reserved_data_t, message), .array_len = sizeof(log_data.message) },
    };

    const data_fmt_desc_t data_desc = { .fields = data_field_descs, .count = 1 };

    log_sys_log_signal_with_data(LOG_PRINTF_LOG_ID, LOG_PRINTF_SIG_ID, sizeof(log_data.message), (const void *)&log_data, &data_desc);
}

/* This is a special LOG_ function used for cmd system schedule responses */
void LOG_CMD_SYS_SCHED_RESP(uint32_t num_bytes, const uint8_t* data) {
    log_signal_internal(LOG_CMD_SYS_SCHED_RESP_LOG_ID, LOG_CMD_SYS_SCHED_RESP_SIG_ID, num_bytes, data, true);
}

void logger_create_infra(void) {
    static StaticSemaphore_t static_logger_mutex;
    logger_mutex = xSemaphoreCreateRecursiveMutexStatic(&static_logger_mutex);
}

void log_sys_log_signal(uint8_t log_id, uint8_t sig_id) {
    log_signal_internal(log_id, sig_id, 0, NULL, true);
}

void log_sys_log_signal_with_data(uint8_t log_id, uint8_t sig_id, uint32_t data_len, const void *data_struct, const data_fmt_desc_t *data_desc) {
    static uint8_t buf[512];

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

static void log_signal_internal(uint8_t log_id, uint8_t sig_id, size_t raw_payload_len, const void* payload_ptr, bool use_rtc) {
    // Determine payload len -> if too large, cap at MAX_PAYLOAD_SIZE
    uint8_t payload_len;
    if (raw_payload_len <= MAX_PAYLOAD_SIZE) {
        payload_len = raw_payload_len;
    } else {
        payload_len = MAX_PAYLOAD_SIZE;
        LOG_LOGGER__PAYLOAD_TOO_LARGE();
    }

    uint8_t message_buf[HEADER_SIZE + MAX_PAYLOAD_SIZE] = {0};

    // Populate the header in the output buffer
    encode_header(log_id, sig_id, payload_len, &message_buf[0], use_rtc);

    // If this message has a payload...
    if ((payload_len > 0) && (payload_ptr != NULL)) {
        // Populate the payload data in the output buffer
        memcpy(&message_buf[HEADER_SIZE], payload_ptr, payload_len);
    }

    if (xSemaphoreTake(logger_mutex, pdMS_TO_TICKS(LOGGER_MUTEX_TIMEOUT_MS)) == pdPASS) {
        // Write log message to serial
        const size_t message_size = HEADER_SIZE + payload_len;
        io_stream_write(&obc_serial_logs_out, message_buf, message_size, pdMS_TO_TICKS(SERIAL_TIMEOUT_MS), NULL);

        xSemaphoreGive(logger_mutex);
    }

    // TODO ALEA-861 send message to filesystem
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
        current_epoch = get_epoch_time();
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
