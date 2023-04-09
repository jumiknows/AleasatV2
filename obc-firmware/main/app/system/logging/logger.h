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
 * Creates the logging system infrastructure
 * 
 * @note must be called before calling any other functions in the logger API
 */
void logger_create_infra(void);

/**
 * @brief Logs a signal, without reading time from the RTC
 *
 * @param[in] lvl               The log level to use (@ref log_level_t)
 * @param[in] log_id            The message ID to use.
 * @param[in] sig_id            The signal to log.
 */
void log_signal_without_time(log_level_t lvl, log_id_t log_id, signal_id_t sig_id);

/**
 * @brief Logs a signal without time, with data payload
 *
 * @param[in] lvl               The log level to use (@ref log_level_t)
 * @param[in] log_id            The message ID to use.
 * @param[in] sig_id            The signal to log.
 * @param[in] payload_len       The payload length in bytes
 * @param[in] payload_ptr       Pointer to the payload
 */
void log_signal_without_time_with_data(log_level_t lvl, log_id_t log_id, signal_id_t sig_id, size_t payload_len, void* payload_ptr);

/**
 * @brief Logs a signal
 *
 * @param[in] lvl               The log level to use (@ref log_level_t)
 * @param[in] log_id            The message ID to use.
 * @param[in] sig_id            The signal to log.
 */
void log_signal(log_level_t lvl, log_id_t log_id, signal_id_t sig_id);

/**
 * @brief Logs a signal with data payload
 *
 * @param[in] lvl               The log level to use (@ref log_level_t)
 * @param[in] log_id            The message ID to use.
 * @param[in] sig_id            The signal to log.
 * @param[in] payload_len       The payload length in bytes
 * @param[in] payload_ptr       Pointer to the payload
 */
void log_signal_with_data(log_level_t lvl, log_id_t log_id, signal_id_t sig_id, size_t payload_len, const void* payload_ptr);

#endif // LOGGER_H_
