/**
 * @file obc_error.h
 * @brief Error typedefs for all modules within the OBC codebase.
 * @date November 11, 2019
 * @author ORCASat and ALEASAT C&DH Teams
 *
 * The design doc for this component is here:
 * https://docs.google.com/document/d/1sThAB_rCVfnkXroXrcDbNBuco_RZUgcAvjrwXqbucLE/edit#
 *
 * Every module that has to report errors will have a typedef'd enum in this file. This scheme
 * allows all errors from inter-dependent modules (such as i2c and the GPIO expander driver) to be
 * seen in one place.
 */

#ifndef ORCASAT_SYSTEM_OBC_ERROR_H_
#define ORCASAT_SYSTEM_OBC_ERROR_H_

#include "obc_misra.h"

OBC_MISRA_CHECK_OFF
#include "lfs.h"
OBC_MISRA_CHECK_ON

/**
 * @brief Return value for flash functions, indicating any errors.
 */
typedef enum flash_err {
    /**
     * @brief Indicates that there were no IO errors.
     */
    FLASH_OK = 0,

    /**
     * @brief Indicates there was a low-level error in the MIBSPI driver.
     */
    FLASH_MIBSPI_ERR = -1,

    /**
     * @brief Indicates a timeout occurred while erasing, and the flash is still
     * reporting a busy state.
     */
    FLASH_ERASE_TIMEOUT_ERR = -2,

    /**
     * @brief Indicates a timeout occurred while writing, and the flash is still
     * reporting a busy state.
     */
    FLASH_WRITE_TIMEOUT_ERR = -3,

    /**
     * @brief Indicates that either the address passed to the flash driver is
     * outside of bounds, or the operation would require writing outside of
     * these bounds
     */
    FLASH_ADDR_OR_SIZE_TOO_LARGE_ERR = -4,

    /**
     * @brief Indicates that the address is not correctly aligned
     */
    FLASH_ADDR_UNALIGNED_ERR = -5,

    /**
     * @brief Indicates than an invalid read/write size was provided as argument
     */
    FLASH_INVALID_SIZE_ERR = -6,

#ifdef PLATFORM_LAUNCHPAD_1224
    FLASH_MOCK_ERR = -7
#endif
#ifdef PLATFORM_ALEA_V1
    FLASH_MOCK_ERR = -7
#endif
} flash_err_t;

/**
 * @brief Return value for mram functions, indicating any errors.
 */
typedef enum mram_err {
    /**
     * @brief Indicates that there were no IO errors.
     */
    MRAM_OK = 0,

    /**
     * @brief Indicates there was a request to access MRAM that does not exist.
     */
    MRAM_INDEX_OUT_OF_BOUND = -1,

    /**
     * @brief Indicates there was a low-level error in the MIBSPI driver.
     */
    MRAM_MIBSPI_ERR = -2,

    /**
     * @brief Indicates there was an error attempting to grab the MIBSPI mutex.
     */
    MRAM_MIBSPI_MUTEX_GRAB_ERR = -3
} mram_err_t;

/**
 * @brief Filesystem error codes, propagated up from LittleFS.
 */
typedef enum fs_err {
    /* OBC filesystem errors */
    FS_TEST_READ_FAILURE    = 3,                   // 3     : Test function failed to read from file (for FS test)
    FS_TEST_WRITE_FAILURE   = 2,                   // 2     : Test function failed to write to file (for FS test)
    FS_TEST_STRINGS_UNEQUAL = 1,                   // 1     : Test strings do not match (for FS test)
    /* Errors propagated up from LFS */
    FS_OK                   = LFS_ERR_OK,          // 0 	: No error
    FS_IO_ERR               = LFS_ERR_IO,          // -5 	: Error during device operation
    FS_CORRUPT_ERR          = LFS_ERR_CORRUPT,     // -84 	: Corrupted
    FS_NOENT_ERR            = LFS_ERR_NOENT,       // -2 	: No directory entry
    FS_EXIST_ERR            = LFS_ERR_EXIST,       // -17 	: Entry already exists
    FS_NOTDIR_ERR           = LFS_ERR_NOTDIR,      // -20 	: Entry is not a dir
    FS_ISDIR_ERR            = LFS_ERR_ISDIR,       // -21 	: Entry is a dir
    FS_NOTEMPTY_ERR         = LFS_ERR_NOTEMPTY,    // -39 	: Dir is not empty
    FS_BADF_ERR             = LFS_ERR_BADF,        // -9 	: Bad file number
    FS_FBIG_ERR             = LFS_ERR_FBIG,        // -27 	: File too large
    FS_INVAL_ERR            = LFS_ERR_INVAL,       // -22 	: Invalid parameter
    FS_NOSPC_ERR            = LFS_ERR_NOSPC,       // -28 	: No space left on device
    FS_NOMEM_ERR            = LFS_ERR_NOMEM,       // -12 	: No more memory available
    FS_NOATTR_ERR           = LFS_ERR_NOATTR,      // -61 	: No data/attr available
    FS_NAMETOOLONG_ERR      = LFS_ERR_NAMETOOLONG, // -36 	: File name too long
} fs_err_t;

/**
 * @brief Return values for temperature sensor function.
 */
typedef enum {

    /**
     * @brief Indicates that the operation was performed successfully.
     */
    TEMP_SUCCESS = 0,

    /**
     * @brief Indicates problem with temp sensor operation.
     */
    TEMP_FAILURE = -1,

    /**
     * @brief Indicates that an I2C error occurred when communicating with the temperature sensor.
     */
    TEMP_I2C_ERR = -2
} temp_err_t;

/**
 * @brief Return value for MIBSPI functions, indicating any errors.
 */
typedef enum mibspi_err {
    /**
     * @brief Indicates that there were no IO errors.
     */
    MIBSPI_NO_ERR = 0,

    /**
     * @brief Indicates that an overflow has occurred in the RX buffer,
     * and unread data was overwritten.
     */
    MIBSPI_RX_OVERFLOW = -1,

    /**
     * @brief Indicates there was a mismatch in the data that was transmitted
     * and the internal TX data.
     */
    MIBSPI_TX_BITERR = -2,

    /**
     * @brief Indicates a data length error has occurred.
     */
    MIBSPI_DATALEN_ERR = -3,

    /**
     * @brief Indicates a timeout has occurred in the MIBSPI transaction.
     */
    MIBSPI_TIMEOUT = -4,

    /**
     * @brief Indicates a failure to grab the MIBSPI mutex. Most likely a timeout.
     */
    MIBSPI_MUTEX_GRAB_ERR = -5,

    /**
     * @brief Unknown error occurred.
     */
    MIBSPI_UNKNOWN_ERR = -6
} mibspi_err_t;

/**
 * @brief Return codes from the OBC scheduler in system/scheduler.
 */
typedef enum {
    /**
     * @brief Scheduler function succeeded.
     */
    SCHEDULER_SUCCESS = 0,

    /**
     * @brief Indicates that the desired arguments are too large.
     */
    ARG_SIZE_EXCEEDS_MAX_ARG_SIZE = 1,

    /**
     * @brief Init function failed to create action queue.
     */
    FAILED_TO_CREATE_ACTIONS_QUEUE = 2,

    /**
     * @brief Init function failed to create mutex.
     */
    FAILED_TO_CREATE_ACTIONS_MUTEX = 3,

    /**
     * @brief Init function failed to create scheduler task.
     */
    FAILED_TO_CREATE_SCHEDULER_TASK = 4,

    /**
     * @brief Init function failed to create executer task.
     */
    FAILED_TO_CREATE_EXECUTER_TASK = 5,

    /**
     * @brief Init function failed to create sync task.
     */
    FAILED_TO_CREATE_SYNC_TASK = 6,

    /**
     * @brief Init function failed to create sort task.
     */
    FAILED_TO_CREATE_SORT_TASK = 7,

    /**
     * @brief Maximum number of actions reached, action will not be added.
     */
    REACHED_MAX_NUM_ACTIVE_ACTIONS = 8,

    /**
     * @brief Scheduler tried to remove an unknown action ID.
     */
    UNDEFINED_ACTION_ID = 9,

    /**
     * @brief Invalid Realtime.
     */
    INVALID_REALTIME = 10,

    /**
     * @brief Scheduler failed to read time, so failed to schedule the action.
     */
    TIME_FAILURE = 11,

    /**
     * @brief Action has reached its terminal state.
     */
    NO_LONGER_ACTIVE = 12,

    /**
     * @brief Action is not periodic.
     */
    NOT_PERIODIC = 13,
} scheduler_err_t;

/**
 * Return codes for RTC functions.
 */
typedef enum rtc_errors {
    /**
     * @brief Returned upon success of most RTC functions.
     */
    RTC_NO_ERR = 1,

    /**
     * @brief Returned when the desired RTC alarm set time is valid.
     */
    RTC_ALARM_VALID = 2,

    /**
     * @brief Returned if time data was not read from RTC yet
     */
    RTC_NO_SAVED_TIME = 3,

    /**
     * @brief RTC cannot acquire a mutex.
     */
    RTC_MUTEX_GRAB_ERR = -1,

    /**
     * @brief RTC cannot acquire the MIBSPI mutex.
     */
    RTC_MIBSPI_MUTEX_GRAB_ERR = -2,

    /**
     * @brief RTC communications experienced a MIBSPI error.
     */
    RTC_MIBSPI_ERR = -3,

    /**
     * @brief Indicates that the RTC clock was restarted due to power loss.
     */
    RTC_CLK_RESTARTED = -4,

    /**
     * @brief Indicates that the scheduled alarm year is invalid.
     */
    RTC_ALARM_YEAR_INVALID = -5,

    /**
     * @brief Indicates that the scheduled alarm month is invalid.
     */
    RTC_ALARM_MONTH_INVALID = -6,

    /**
     * @brief Indicates that the scheduled alarm day is invalid.
     */
    RTC_ALARM_DAY_TIME_INVALID = -7,

    /**
     * @brief Indicates that the scheduled alarm date is invalid.
     */
    RTC_ALARM_DATE_INVALID = -8,

    /**
     * @brief Indicates that the scheduled alarm time is invalid.
     */
    RTC_ALARM_INVALID = -9,

    /**
     * @brief Indicates that the RTC mock could not be initialized.
     */
    RTC_MOCK_FAILED_TO_CREATE_STEP_TASK = -10,

    /**
     * @brief Indicates that the RTC has been disabled, and that no communication will be made with
     * it.
     */
    RTC_DISABLED = -11,

    /**
     * @brief Indicates that the epoch could not be calculated from the RTC's real time. This likely
     * also indicates an invalid real time.
     */
    RTC_EPOCH_INVALID = -12,

    /**
     * @brief Indicates that the real time is not valid.
     */
    RTC_REAL_TIME_INVALID = -13,
} rtc_err_t;

/**
 * @brief Return codes from the telemetry logging system.
 */
typedef enum {
    /**
     * @brief Telemetry function operated correctly.
     */
    TELEM_OK = 0,

    /**
     * @brief Telemetry system cannot start a telemetry logging action.
     */
    TELEM_START_FAILED = -1

} telem_err_t;

/**
 * @brief Error or return codes from OBC RTOS infrastructure.
 */
typedef enum {
    /**
     * @brief Operation succeeded.
     */
    OBC_RTOS_OK = 0,

    /**
     * @brief Requested period was invalid. Generally, this occurs when attempting to change the
     * period of a nonperiodic task.
     */
    INVALID_PERIOD,

    /**
     * @brief Requested ID was invalid. This occurs when the requested ID does not correspond to a
     * created task.
     */
    INVALID_ID,

    /**
     * @brief The task could not be suspended because it was running. Try changing the period again.
     */
    TASK_NOT_BLOCKED,

    /**
     * @brief The name provided cannot be mapped to an ID.
     */
    INVALID_NAME,

} rtos_err_t;

/**
 * @brief Error or return codes from OBC CMD SYS.
 */
typedef enum {
    /**
     * @brief Indicates that the desired command was found in the regular
     * (not critical) command table.
     */
    OBC_COMMAND_FOUND,

    /**
     * @brief Indicates that a critical command was found, and should be
     * executed immediately without using the scheduler.
     */
    OBC_COMMAND_CRITICAL,

    /**
     * @brief Invalid command syntax.
     */
    OBC_CMD_SYNTAX_ERROR,

    /**
     * @brief Unkown command name.
     */
    OBC_CMD_NO_MATCHING_CMD_NAME,

    /**
     * @brief Failed to schedule the action with the scheduler.
     */
    OBC_CMD_FAILED_TO_SCHEDULE,

    /**
     * @brief Indicates that an immediate command has been scheduled correctly.
     */
    OBC_IMMEDIATE_SUCCESS,

    /**
     * @brief Indicates that a scheduled command has been parsed correctly.
     */
    OBC_SCHEDULED_SUCCESS,

    /**
     * @brief Indicates that a critical command was executed.
     */
    OBC_CRITICAL_SUCCESS,

    /**
     * @brief Indicates that the command did not have an interval.
     */
    OBC_CMD_NO_INTERVAL,

    /**
     * @brief Indicates that there was a time conversion error when processing the command.
     */
    OBC_CMD_TIME_ERR,

    /**
     * @brief Indicates that the sequence number of the command is invalid. It is not 0
     * or not the next expected sequence number.
     */
    OBC_SEQ_NUM_INVALID

} obc_cmd_err_t;

/**
 * @brief Error or return codes for the UART.
 */
typedef enum {
    /**
     * @brief UART operation succeeded.
     */
    UART_OK = 0,

    /**
     * @brief UART transmit queue is full, not all characters will be transmitted.
     */
    UART_TX_Q_FULL = -1,

    /**
     * @brief UART RX buffer overflow error.
     */
    UART_RX_OVERFLOW = -2,

    /**
     * @brief UART send-and-receive RX timeout error.
     */
    UART_RX_TIMEOUT = -3
} uart_err_t;

/**
 * @brief OEM7X parser result status
 */
typedef enum {
    /**
     * @brief No detected errors
     */
    GPS_PARSE_SUCCESS = 0,

    /**
     * @brief Failed to parse all expected fields
     */
    GPS_PARSE_MATCH_COUNT = -1,

    /**
     * @brief Missing sync '#' character
     */
    GPS_PARSE_NO_SYNC = -2,

    /**
     * @brief Wrong message type
     */
    GPS_PARSE_MSG_TYPE = -3,

    /**
     * @brief No command response found
     */
    GPS_PARSE_NO_CMD_RESP = -4,

    /**
     * @brief Command returned error code (not "OK")
     */
    GPS_PARSE_CMD_RESP_ERROR = -5,

    /**
     * @brief CRC verification failed
     */
    GPS_PARSE_CRC_ERROR = -6
} gps_parse_err_t;

/**
 * @brief Errors for settings.
 */
typedef enum {
    /**
     * @brief Setting the setting succeeded.
     */
    SETTING_OK = 1,
    /**
     * @brief The setting doesn't exist.
     */
    INVALID_SETTING_NAME,
    /**
     * @brief The desired value for the setting was not within the allowable range.
     */
    INVALID_SETTING_VALUE,
    /**
     * @brief The setting set operation failed for a reason internal to the module that uses
     * the setting.
     */
    SETTING_CALLBACK_FAILED,
    /**
     * @brief The setting is not settable by operators. It is only settable from within firmware.
     */
    SETTING_IMMUTABLE,
    /**
     * @brief The settings mutex could not be grabbed.
     */
    SETTING_MUTEX_ERR,
    /**
     * @brief The setting could not be updated in NVCT.
     */
    SETTING_NVCT_UPDATE_ERR,
    /**
     * @brief A null pointer was provided to a mandatory setting API argument.
     */
    SETTING_NULL_ERR,
} setting_err_t;

/**
 * @brief Error for CFDP (IN PROGRESS)
 */
typedef enum {
    /**
     * @brief CFDP operation successful
     */
    CFDP_OK = 0,
    /**
     * @brief The data provided to the function is
     * too large.
     */
    CFDP_ERR_TOO_LARGE = 1
} cfdp_err_t;

/**
 * @typedef nvct_err_enum_t
 * @brief nvct error codes
 */
typedef enum {
    NVCT_SUCCESS,
    /**
     * @brief The requested index is outside the allowable range.
     */
    NVCT_INDEX_OUT_OF_BOUND,
    /**
     * @brief No table with matching firmware version has been found,
     * so NVCT can't be read or written from the user API.
     */
    NVCT_NO_TABLE_WITH_MATCHING_FIRMWARE_VERSION,
    /**
     * @brief The MRAM indicated a hardware error while writing.
     */
    NVCT_FAILED_TO_WRITE_TO_MRAM_ERROR,
    /**
     * @brief The MRAM indicated a hardware error while reading.
     */
    NVCT_FAILED_TO_READ_FROM_MRAM_ERROR,
    /**
     * @brief The value was read from storage but the CRC check failed.
     */
    NVCT_CRC_ERROR,
    /**
     * @brief An invalid table index was given.
     */
    NVCT_TABLE_INDEX_ERROR,
} nvct_err_enum_t;

#endif /* ORCASAT_SYSTEM_OBC_ERROR_H_ */
