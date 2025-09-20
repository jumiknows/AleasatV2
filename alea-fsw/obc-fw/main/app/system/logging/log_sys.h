/**
 * @file log_sys.h
 * @brief OBC logging module
 */

#ifndef LOGGER_H_
#define LOGGER_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Utils
#include "data_fmt.h"

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
#define MAX_PAYLOAD_SIZE 245U /* bytes */

#define FS_LOGGING_PARTITION_SIZE 1*1024*1024
#define FS_LOGGING_FILES_NUM 4 // Number of logging files in file system; if changing, make sure to update @ref LOG_FILE_NAMES & cmd_GET_LOGFILE_INFO too
#define FS_LOGGING_MAX_FILE_SIZE (FS_LOGGING_PARTITION_SIZE / FS_LOGGING_FILES_NUM)

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

struct logfile_info_t {
    uint8_t current_logfile;
    uint32_t logfile_sizes[FS_LOGGING_FILES_NUM];
};

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/**
 * Creates the logging system infrastructure
 *
 * @note must be called before calling any other functions in the logger API
 */

void log_sys_pre_init(void);
void log_sys_post_init(void);

int32_t logs_read_begin(uint8_t *buf, uint32_t bytes_to_read, uint16_t chunk_size);
void logs_read_end();

int32_t logs_read();

bool log_sys_get_info(struct logfile_info_t *info);

/**
 * @brief Logs a signal
 *
 * @param[in] log_id The message ID to use.
 * @param[in] sig_id The signal to log.
 */
void log_sys_log_signal(uint8_t log_id, uint8_t sig_id);

/**
 * @brief Logs a signal with data payload
 *
 * @param[in] log_id     The message ID to use.
 * @param[in] sig_id     The signal to log.
 * @param[in] data_len   The expected length of the data
 * @param[in] data_strct Pointer to a struct containing the signal data
 * @param[in] data_desc  Pointer to the data format descriptor struct
 */
void log_sys_log_signal_with_data(uint8_t log_id, uint8_t sig_id, uint32_t data_len, const void *data_struct, const data_fmt_desc_t *data_desc);

#endif // LOGGER_H_
