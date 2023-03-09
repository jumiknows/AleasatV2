/**
 * @file logger.c
 * @brief OBC logging module
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "logger.h"

// OBC
#include "obc_serial_tx.h"
#include "obc_rtc.h"

// Utils
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

static bool take_mutex(void);
static void release_mutex(void);
static void encode_header(log_level_t lvl, log_identifier_t func_id, uint8_t size, uint8_t header_ptr[HEADER_SIZE], bool use_rtc);
static void log_unexpected_payload_size(log_identifier_t func_id);
static void log_unexpected_msg_id(log_identifier_t func_id);
static void log_to_file(log_level_t lvl, uint8_t* data, uint32_t size);
static void log_data_internal(log_level_t lvl, log_identifier_t func_id, bool write_to_file, uint8_t size, const void* data, bool use_rtc);
static void log_str_internal(log_level_t lvl, log_identifier_t func_id, bool write_to_file, char* format, va_list va, bool use_rtc);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void log_data(log_level_t lvl, log_identifier_t func_id, uint8_t size, const void* data) {
    log_data_internal(lvl, func_id, true, size, data, true);
}

void log_str(log_level_t lvl, log_identifier_t func_id, char* format, ...) {
    // format str
    va_list ap;
    va_start(ap, format);

    log_str_internal(lvl, func_id, true, format, ap, true);
    va_end(ap);
}

void log_str_without_time(log_level_t lvl, log_identifier_t func_id, char* format, ...) {
    // format str
    va_list ap;
    va_start(ap, format);

    log_str_internal(lvl, func_id, true, format, ap, false);
    va_end(ap);
}

void log_str_without_saving_to_fs(log_level_t lvl, log_identifier_t func_id, char* format, ...) {
    // format str
    va_list ap;
    va_start(ap, format);

    log_str_internal(lvl, func_id, false, format, ap, true);
    va_end(ap);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Checks for initialization and grabs the logger mutex.
 * If not initialized, it will be created.
 *
 * @return True if the mutex could be grabbed within the allotted time. False otherwise.
 */
static bool take_mutex(void) {
    static StaticSemaphore_t logger_mutex_buffer;

    // Create the mutex if it doesn't exist yet
    if (logger_mutex == NULL) {
        logger_mutex = xSemaphoreCreateRecursiveMutexStatic(&logger_mutex_buffer);
    }

    return (xSemaphoreTake(logger_mutex, pdMS_TO_TICKS(LOGGER_MUTEX_TIMEOUT_MS)) == pdPASS);
}

/**
 * @brief Releases the logger mutex
 */
static void release_mutex(void) {
    xSemaphoreGive(logger_mutex);
}

/**
 * @brief Encodes the message header.
 *
 * @param[in] lvl The log level to use (DEBUG, WARNING, INFO, ERROR).
 * @param[in] func_id The message ID to use.
 * @param[in] size The size of the payload (bytes).
 * @param[out] header_ptr Pointer to the top of the message buffer. The header will be placed here.
 */
static void encode_header(log_level_t lvl, log_identifier_t func_id, uint8_t size, uint8_t header_ptr[HEADER_SIZE], bool use_rtc) {
    epoch_t current_epoch = 0;

    // Grab a timestamp if the logger is configured to read from the RTC.
    if (use_rtc) {
        current_epoch = get_epoch_time();
    }

    // byte 0-3: epoch (big-endian)
    header_ptr[0] = (current_epoch >> 24) & 0xff;
    header_ptr[1] = (current_epoch >> 16) & 0xff;
    header_ptr[2] = (current_epoch >>  8) & 0xff;
    header_ptr[3] = (current_epoch >>  0) & 0xff;
    // byte 4:  func_id[5], ..., func_id[0], lvl[1], lvl[0]
    header_ptr[4] = ((uint8_t)((uint8_t)func_id << 2) & 0b11111100) | (lvl & 0b00000011);
    // byte 5: padding[3], ..., padding[0], func_id[9], ..., func_id[6]
    header_ptr[5] = ((uint8_t)(func_id >> 6)) & 0b00001111;
    // byte 6: size[7], ..., size[0]
    header_ptr[6] = (size & 0xff);

    if (func_id > MAX_LOG_ID_VALUE) {
        log_unexpected_msg_id(func_id);
    }
}

/**
 * @brief Logs an unexpected payload error message
 *
 * @param[in] func_id  message id of the log request, to be incorporated into the error message
 *
 */

static void log_unexpected_payload_size(log_identifier_t func_id) {
    log_str(ERROR, LOG_UNEXPECTED_PAYLOAD_LEN, "ID: %d payload too large", func_id);
}

/**
 * @brief Logs an unexpected message ID
 *
 * @param[in] func_id  message id of the log request, to be incorporated into the error message
 *
 */
static void log_unexpected_msg_id(log_identifier_t func_id) {
    log_str(ERROR, LOG_UNEXPECTED_MSG_ID, "ID %d too large", func_id);
}

static void log_to_file(log_level_t lvl, uint8_t* data, uint32_t size) {
#if 0 // TODO re-enable this as part of logger refactor ALEA-774
    // Don't do anything until the filesystem has been initialized, this is here
    // since some prints are called prior to the fs_init has finished
    if (!fs_initialized) {
        return;
    }

    char file_path[LFS_NAME_MAX] = { 0 };
    if (lvl == ERROR) {
        make_file_path(ERR_LOG_FILENAME, file_path);
    } else if (lvl == INFO) {
        make_file_path(SYS_LOG_FILENAME, file_path);
    } else {
        return;
    }

    fs_write((const char*)file_path, 0, data, size, LFS_SEEK_END);
#endif
}

/**
 * @brief Logs arbitrary data.
 *
 * @param[in] lvl The log level to use (ERROR, INFO, DEBUG).
 * @param[in] func_id The message ID to use.
 * @param[in] write_to_file (write to file if true, if else don't write to file)
 * @param[in] size The size of the payload data (bytes).
 * @param[in] data The data to log. 25 bytes will be logged.
 */
static void log_data_internal(log_level_t lvl, log_identifier_t func_id, bool write_to_file, uint8_t size, const void* data, bool use_rtc) {
    uint8_t message[HEADER_SIZE + MAX_PAYLOAD_SIZE] = {'\0'};

    uint8_t payload_size;
    if (size <= MAX_PAYLOAD_SIZE) {
        payload_size = size;
    } else {
        payload_size = MAX_PAYLOAD_SIZE;
        log_unexpected_payload_size(func_id);
    }

    if (take_mutex()) {
        encode_header(lvl, func_id, payload_size, &message[0], use_rtc);

        // Encode the payload
        memcpy(&message[HEADER_SIZE], data, sizeof(uint8_t) * payload_size);
        io_stream_write(&obc_serial_logs_out, message, HEADER_SIZE + payload_size, pdMS_TO_TICKS(SERIAL_TIMEOUT_MS), NULL);

        release_mutex();
    }

    // Send the data and log to flash
    if (write_to_file) {
        log_to_file(lvl, &message[0], HEADER_SIZE + payload_size);
    }
}

/**
 * @brief Logs a 25-character string. Ignores and does not log the null terminator.
 *
 * @param[in] lvl The log level to use (DEBUG, WARNING, INFO, ERROR).
 * @param[in] func_id The message ID to use.
 * @param[in] write_to_file (write to file if true, if else don't write to file)
 * @param[in] format The string to log. Supports printf-style format specifiers such as %d, %i, etc.
 * @param[in] va_list The variable length arguments
 */
static void log_str_internal(log_level_t lvl, log_identifier_t func_id, bool write_to_file, char* format, va_list va, bool use_rtc) {
    unsigned char str[MAX_PAYLOAD_STRING_SIZE] = {"\0"}; // Allow space for vsnprintf's inserted null terminator, which will be discarded in the logged message
    uint32_t str_len = orca_vsnprintf((char*)str, MAX_PAYLOAD_STRING_SIZE, format, va); // Note: required len ignores null terminator if it exists

    log_data_internal(lvl, func_id, write_to_file, str_len, &str[0], use_rtc);
}
