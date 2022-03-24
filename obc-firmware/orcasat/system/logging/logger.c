/*******************************************************
 * logger.c
 *
 * Created on: May 11, 2019
 *  Author(s): Alireza.A, Julian M
 *
 *******************************************************/

#include <stdarg.h>
#include <string.h>

#include "logger.h"
#include "obc_uart.h"
#include "obc_rtc.h"
#include "printf.h"
#include "filesystem.h"
#include "rtos.h"

#define HEADER_SIZE             7 // bytes
#define MAX_STRING_LENGTH       MAX_PAYLOAD_SIZE + 1
#define MAX_MSG_ID              1023
#define LOGGER_MUTEX_TIMEOUT_MS 500

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
SemaphoreHandle_t logger_mutex = NULL;
StaticSemaphore_t logger_mutex_buffer;

static bool get_mutex(void);
static void encode_header(log_level_t lvl, log_identifier_t func_id, log_payload_size_t size, uint8_t header_ptr[HEADER_SIZE]);
static void log_unexpected_payload_size(log_identifier_t func_id);
static void log_unexpected_msg_id(log_identifier_t func_id);
static void update_metadata(log_level_t lvl);
static void log_to_file(log_level_t lvl, uint8_t* data, uint32_t size);
static void log_data_internal(log_level_t lvl, log_identifier_t func_id, bool write_to_file, log_payload_size_t size, void* data);
static void log_str_internal(log_level_t lvl, log_identifier_t func_id, bool write_to_file, char* format, va_list va);

/**
 * @brief Indicates whether log messages will attempt to read the RTC to create a timestamp.
 *
 * In certain situations, such as when trying to log failures with the RTC itself, it doesn't
 * make sense to try to read the RTC (since we know it's broken). In these cases, the code
 * should set this flag to false, and downstream code should use it to determine whether or not
 * to read from the RTC.
 *
 * Default: true (read from RTC).
 */
bool use_rtc = true;

uint32_t num_logged_debug = 0;
uint32_t num_logged_info  = 0;
uint32_t num_logged_error = 0;
uint32_t num_logged_warning = 0;

/**
 * @brief Logs arbitrary data. Only logs the first 31 bytes of the data.
 *
 *  @param[in] lvl The log level to use (DEBUG, WARNING, INFO, ERROR).
 *  @param[in] func_id The message ID to use.
 *  @param[in] write_to_file (write to file if true, if else don't write to file)
 *  @param[in] size The size of the payload data (bytes).
 *  @param[in] data The data to log. 24 bytes will be logged.
 */
void log_data(log_level_t lvl, log_identifier_t func_id, bool write_to_file, log_payload_size_t size, void* data) {
    log_data_internal(lvl, func_id, write_to_file, size, data);
}

/**
 * @brief Logs a 31-character string. Ignores and does not log the null terminator. Adds PDU header with transaction id of 0 (all logs use id = 0)
 *
 *  @param[in] lvl The log level to use (DEBUG, WARNING, INFO, ERROR).
 *  @param[in] func_id The message ID to use.
 *  @param[in] write_to_file (write to file if true, if else don't write to file)
 *  @param[in] format The string to log. Supports printf-style format specifiers such as %d, %i, etc.
 */
void log_str(log_level_t lvl, log_identifier_t func_id, bool write_to_file, char* format, ...) {
    // format str
    va_list ap;
    /* Waiving MISRA check due to positive result for 6.3. This is caused by (int) in stdarg.h. (RA) */
    OBC_MISRA_CHECK_OFF
    va_start(ap, format);
    OBC_MISRA_CHECK_ON

    log_str_internal(lvl, func_id, write_to_file, format, ap);
    va_end(ap);
}

/**
 * @brief Logs arbitrary data. Only logs the first 31 bytes of the data.
 *
 *  @param[in] lvl The log level to use (ERROR, INFO, DEBUG).
 *  @param[in] func_id The message ID to use.
 *  @param[in] write_to_file (write to file if true, if else don't write to file)
 *  @param[in] size The size of the payload data (bytes).
 *  @param[in] data The data to log. 25 bytes will be logged.
 */
static void log_data_internal(log_level_t lvl, log_identifier_t func_id, bool write_to_file, log_payload_size_t size, void* data) {
    uint8_t message[HEADER_SIZE + MAX_PAYLOAD_SIZE] = {'\0'};

    log_payload_size_t payload_size;
    if (size <= MAX_PAYLOAD_SIZE) {
        payload_size = size;
    } else {
        payload_size = MAX_PAYLOAD_SIZE;
        log_unexpected_payload_size(func_id);
    }

    if (get_mutex()) {
        encode_header(lvl, func_id, payload_size, &message[0]);

        // Encode the payload
        memcpy(&message[HEADER_SIZE], data, sizeof(uint8_t) * payload_size);
        serial_send((uint8_t*)message, HEADER_SIZE + payload_size);

        update_metadata(lvl);

        xSemaphoreGive(logger_mutex);
    }

    // Send the data and log to flash
    if (write_to_file) {
        log_to_file(lvl, &message[0], HEADER_SIZE + payload_size);
    }
}

/**
 * @brief Logs a 25-character string. Ignores and does not log the null terminator.
 *
 *  @param[in] lvl The log level to use (DEBUG, WARNING, INFO, ERROR).
 *  @param[in] func_id The message ID to use.
 *  @param[in] write_to_file (write to file if true, if else don't write to file)
 *  @param[in] format The string to log. Supports printf-style format specifiers such as %d, %i, etc.
 *  @param[in] va_list The variable length arguments
 */
static void log_str_internal(log_level_t lvl, log_identifier_t func_id, bool write_to_file, char* format, va_list va) {
    unsigned char str[MAX_STRING_LENGTH]                                   = {"\0"}; // Allow space for vsnprintf's inserted null terminator, which will be discarded in the logged message
    uint8_t message[HEADER_SIZE + MAX_PAYLOAD_SIZE] = {'\0'};

    log_payload_size_t payload_size;

    uint32_t required_len = orca_vsnprintf((char*)str, MAX_STRING_LENGTH, format, va); // Note: required len ignores null terminator if it exists

    if (required_len <= MAX_PAYLOAD_SIZE) {
        payload_size = required_len;
    } else {
        payload_size = MAX_PAYLOAD_SIZE;
        log_unexpected_payload_size(func_id);
    }

    if (get_mutex()) {
        // encode the header
        encode_header(lvl, func_id, payload_size, &message[0]);

        // encode the payload
        memcpy(&message[HEADER_SIZE], str, sizeof(unsigned char) * payload_size);
        serial_send((uint8_t*)message, HEADER_SIZE + payload_size);

        update_metadata(lvl);

        xSemaphoreGive(logger_mutex);
    }

    // Send the data and log to flash
    if (write_to_file) {
        log_to_file(lvl, message, HEADER_SIZE + payload_size);
    }
}

/**
 * @brief Logs a 31-character string without reading the RTC as a side effect. Ignores and does not
 * log the null terminator.
 *
 * This is suitable for logging errors from within the RTC module, where the time may be known to be
 * bad.
 *
 *  @param[in] lvl The log level to use (DEBUG, WARNING, INFO, ERROR).
 *  @param[in] func_id The message ID to use.
 *  @param[in] write_to_file (write to file if true, if else don't write to file)
 *  @param[in] format The string to log. Supports printf-style format specifiers such as %d, %i, etc.
 */
void log_str_no_time(log_level_t lvl, log_identifier_t func_id, bool write_to_file, char* format, ...) {
    char str[MAX_STRING_LENGTH] = {"\0"}; // Allow space for vsnprintf's inserted null terminator, which will be discarded in the logged message
    use_rtc                     = false;

    // format str
    va_list ap;
    /* Waiving MISRA check due to positive result for 6.3. This is caused by (int) in stdarg.h. (RA) */
    OBC_MISRA_CHECK_OFF
    va_start(ap, format);
    OBC_MISRA_CHECK_ON

    orca_vsnprintf(str, MAX_STRING_LENGTH, format, ap); // Note: required len ignores null terminator if it exists
    va_end(ap);

    log_str(lvl, func_id, write_to_file, str);
    use_rtc = true;
}

/**
 * @brief Checks for initialization and grabs the logger mutex.
 * If not initialized, it will be created.
 *
 * @return True if the mutex could be grabbed within the allotted time. False otherwise.
 */
static bool get_mutex(void) {
    // Create the mutex if it doesn't exist yet
    if (logger_mutex == NULL) {
        logger_mutex = xSemaphoreCreateRecursiveMutexStatic(&logger_mutex_buffer);
    }

    return xSemaphoreTake(logger_mutex, pdMS_TO_TICKS(LOGGER_MUTEX_TIMEOUT_MS)) == pdPASS;
}

/**
 * @brief Encodes the message header.
 *
 *  @param[in] lvl The log level to use (DEBUG, WARNING, INFO, ERROR).
 *  @param[in] func_id The message ID to use.
 *  @param[in] size The size of the payload (bytes).
 *  @param[out] header_ptr Pointer to the top of the message buffer. The header will be placed here.
 */
static void encode_header(log_level_t lvl, log_identifier_t func_id, log_payload_size_t size, uint8_t header_ptr[HEADER_SIZE]) {
    epoch_t current_epoch = 0;

    // Grab a timestamp if the logger is configured to read from the RTC.
    if (use_rtc) {
        current_epoch = get_epoch_time();
    }

    // byte 0: current_epoch[7], ..., current_epoch[0]
    header_ptr[0] = (current_epoch & 0xff);
    // byte 1: current_epoch[15], ..., current_epoch[8]
    header_ptr[1] = (current_epoch >> 8) & 0xff;
    // byte 2: current_epoch[23], ..., current_epoch[16]
    header_ptr[2] = (current_epoch >> 16) & 0xff;
    // byte 3: current_epoch[31], ..., current_epoch[24]
    header_ptr[3] = (current_epoch >> 24) & 0xff;
    // byte 4:  func_id[5], ..., func_id[0], lvl[1], lvl[0]
    header_ptr[4] = ((uint8_t)((uint8_t)func_id << 2) & 0b11111100) | (lvl & 0b00000011);
    // byte 5: padding[3], ..., padding[0], func_id[9], ..., func_id[6]
    header_ptr[5] = ((uint8_t)(func_id >> 6)) & 0b00001111;
    // byte 6: size[7], ..., size[0]
    header_ptr[6] = (size & 0xff);

    if (func_id > MAX_MSG_ID) {
        log_unexpected_msg_id(func_id);
    }
}

/**
 * log_unexpected_payload_size
 *
 * Logs an unexpected payload error message
 *
 * 	id: message id of the log request, to be incorporated into the error message
 *
 */

static void log_unexpected_payload_size(log_identifier_t func_id) {
    log_str(ERROR, UNEXPECTED_PAYLOAD_LEN, true, "ID: %d payload too large", func_id);
}

/**
 * log_unexpected_msg_id
 *
 * Logs an unexpected message ID
 *
 * 	id: message id of the log request, to be incorporated into the error message
 *
 */
static void log_unexpected_msg_id(log_identifier_t func_id) {
    log_str(ERROR, UNEXPECTED_MSG_ID, true, "ID: %d too large, using: %d", func_id, (func_id - (MAX_MSG_ID + 1)));
}

/**
 * update_metadata
 *
 * Updates statistics associated with the logging system including
 *  - number of debug messages logged
 *  - number of info messages logged
 *  - number of error messages logged
 *  - number of warning messages logged
 *
 *  lvl: Log Level (DEBUG, WARNING, INFO, ERROR).
 *
 */
static void update_metadata(log_level_t lvl) {
    switch (lvl) {
        case DEBUG:
            num_logged_debug += 1;
            break;
        case INFO:
            num_logged_info += 1;
            break;
        case ERROR:
            num_logged_error += 1;
            break;
        case WARNING:
        	num_logged_warning += 1;
        	break;
        default:
            break;
    }
}

static void log_to_file(log_level_t lvl, uint8_t* data, uint32_t size) {
    // Don't do anything until the filesystem has been initialized, this is here
    // since some prints are called prior to the fs_init has finished
    if (!fs_initialized) {
        return;
    }

    char file_path[LFS_NAME_MAX] = {};
    if (lvl == ERROR) {
        make_file_path("err_log", file_path);
    } else if (lvl == INFO) {
        make_file_path("sys_log", file_path);
    } else {
        return;
    }

    fs_write((const char*)file_path, 0, data, size, LFS_SEEK_END);
}
