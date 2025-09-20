/**
 * @file telem_error.h
 * @brief Telemetry
 */

#ifndef TELEM_ERROR_H_
#define TELEM_ERROR_H_

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Status codes for telem functions
 */
typedef enum {
    TELEM_SUCCESS                    = 0,  ///< The telem collection operation was successful
    TELEM_ERR_COLLECT_TIMEOUT        = 1,  ///< A timeout occurred while trying to read/collect telem
    TELEM_ERR_FS_TIMEOUT             = 2,  ///< A timeout occurred trying to talk to filesystem
    TELEM_ERR_DATA_FMT               = 3,  ///< An error occurred serializing or deserializing response data
    TELEM_ERR_ENQUEUE_FAIL           = 4,  ///< Enqueueing task to telem_exec queue failed
    TELEM_ERR_EXEC_TIMEOUT           = 5,  ///< Telem unit execution failed to complete in time
    TELEM_ERR_DNE                    = 6,  ///< The specified telem unit does not exist in spec table
    TELEM_ERR_FLASH_WRITE            = 7,  ///< FS operations returned an error
    TELEM_ERR_FLASH_READ             = 8,  ///< FS operations returned an error
    TELEM_ERR_RTC_ERR                = 9,  ///< An error occured when trying to get current epoch from RTC
    TELEM_ERR_FILE_SIZE              = 10, ///< Could not reset overflowing telemetry files
    TELEM_ERR_ENTRY_DNE              = 11, ///< The telemetry entry searched for does not exist
    TELEM_ERR_GEN_HEADER             = 12, ///< Could not generate/write telem header (most likely RTC call failed)
    TELEM_ERR_QUEUE_MAGIC_NUM        = 13, ///< Failed to add outer header information to queue
    TELEM_ERR_SEARCH_TIMEOUT         = 14, ///< Search timeout
    TELEM_ERR_MRAM                   = 15, ///< If the mram errors out when trying to interact with it
} telem_err_t;

#endif // TELEM_ERROR_H_
