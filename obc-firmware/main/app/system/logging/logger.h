/**
 * @file logger.h
 * @brief OBC logging module
 */

#ifndef LOGGER_H_
#define LOGGER_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "log_ids.h"

// FreeRTOS
#include "rtos.h"

// HAL
#include "sys_common.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Maximum number of bytes that can be logged in a single message
 * 
 */
#define MAX_PAYLOAD_SIZE            245U                    /* bytes */

/**
 * @brief Maximum number of bytes in a string that can be logged in a single message
 * 
 * Is one byte larger than @ref MAX_PAYLOAD_SIZE to account for the null terminator,
 *  which is ignored and not logged.
 * 
 */
#define MAX_PAYLOAD_STRING_SIZE     MAX_PAYLOAD_SIZE + 1U    /* bytes */

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Represents the severity of the log message
 * 
 */
typedef enum {
    DEBUG   = 0,  ///< Debug message
    INFO    = 1,  ///< General info message
    WARNING = 2,  ///< Warning message
    ERROR   = 3   ///< Critical error message
} log_level_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/**
 * @brief Logs arbitrary data.
 *
 * @param[in] lvl       The log level to use (@ref log_level_t)
 * @param[in] func_id   The message ID to use.
 * @param[in] size      The size of the payload data (bytes).
 * @param[in] data      The data to log.
 */
void log_data(log_level_t lvl, log_identifier_t func_id, uint8_t size, const void* data);

/**
 * @brief Logs a string
 * 
 * @note Ignores and does not log the null terminator. Adds PDU header with transaction id of 0 (all logs use id = 0)
 *
 * @param[in] lvl       The log level to use (@ref log_level_t)
 * @param[in] func_id   The message ID to use.
 * @param[in] format    The string to log. Supports printf-style format specifiers such as %d, %i, etc.
 */
void log_str(log_level_t lvl, log_identifier_t func_id, char* format, ...);

/**
 * @brief Logs a string without reading the RTC as a side effect (i.e. without timestamp). Ignores and does not
 * log the null terminator.
 *
 * This is suitable for logging errors from within the RTC module, where the time may be known to be
 * bad.
 *
 * @param[in] lvl       The log level to use (@ref log_level_t)
 * @param[in] func_id   The message ID to use.
 * @param[in] format    The string to log. Supports printf-style format specifiers such as %d, %i, etc.
 */
void log_str_without_time(log_level_t lvl, log_identifier_t func_id, char* format, ...);

/**
 * @brief Logs a string without saving it to the filesystem
 *
 * @param[in] lvl       The log level to use (@ref log_level_t)
 * @param[in] func_id   The message ID to use.
 * @param[in] format    The string to log. Supports printf-style format specifiers such as %d, %i, etc.
 */
void log_str_without_saving_to_fs(log_level_t lvl, log_identifier_t func_id, char* format, ...);

#endif // LOGGER_H_
