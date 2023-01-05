/**
 * @file cmd_sys.h
 * @brief Command System
 */

#ifndef CMD_SYS_H_
#define CMD_SYS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Utils
#include "io_stream.h"
#include "data_fmt.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define CMD_SYS_INPUT_READ_TIMEOUT_MS    100U
#define CMD_SYS_OUTPUT_WRITE_TIMEOUT_MS  100U

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Status codes for cmd_sys functions
 */
typedef enum {
    CMD_SYS_SUCCESS            = 0,  ///< The command system operation was successful
    CMD_SYS_ERR_INVALID_ARGS   = 1,  ///< Invalid arguments were passed to a command system function (e.g. NULL, out of bounds, etc.)
    CMD_SYS_ERR_BUF_TOO_SMALL  = 2,  ///< The buffer passed to a command system function was too small
    CMD_SYS_ERR_ARGS_TOO_LARGE = 3,  ///< The size of the arguments of a command (indicated in the header) is too large
    CMD_SYS_ERR_RESP_TOO_LARGE = 4,  ///< The size of the response data to send is too large
    CMD_SYS_ERR_INVALID_CMD    = 5,  ///< The command ID is out of bounds
    CMD_SYS_ERR_READ_TIMEOUT   = 6,  ///< A timeout occurred reading data from a command input stream
    CMD_SYS_ERR_WRITE_TIMEOUT  = 7,  ///< A timeout occurred writing data to the command output stream
    CMD_SYS_ERR_DATA_FMT       = 8,  ///< An error occurred serializing or deserializing command arguments or response data
    CMD_SYS_ERR_CMD_DNE        = 9,  ///< The command ID does not match a defined commmand
    CMD_SYS_ERR_SCHED          = 10, ///< An error occurred scheduling a command for future execution
    CMD_SYS_ERR_EXEC_TIMEOUT   = 11, ///< A timeout occurred sending a command to the cmd_sys_exec task
} cmd_sys_err_t;

/**
 * @brief Response codes transmitted as the first data byte of a response to a command
 */
typedef enum {
    CMD_SYS_RESP_CODE_SUCCESS       = 1, ///< The command execution was successful
    CMD_SYS_RESP_CODE_SUCCESS_SCHED = 2, ///< The command was successfully scheduled for future execution
    CMD_SYS_RESP_CODE_ERROR         = 3, ///< An error occurred handling the command
    CMD_SYS_RESP_CODE_CMD_DNE       = 4, ///< The command ID does not match a defined commmand
    CMD_SYS_RESP_CODE_NOT_IMPL      = 5, ///< The command ID matches a defined command, but the implementation is missing
} cmd_sys_resp_code_t;

/**
 * @brief Header for command and response messages
 */
typedef struct {
    uint8_t seq_num;
    uint8_t cmd_id;
    uint32_t timestamp;
    uint16_t flags;
    uint32_t data_len;
} cmd_sys_msg_header_t;

/**
 * @brief Core command structure passed between cmd_sys functions so they can access
 * the command header information as well as the input and output streams.
 */
typedef struct {
    const io_istream_t *input;
    const io_ostream_t *output;
    cmd_sys_msg_header_t header;
    uint32_t exec_timestamp;
} cmd_sys_cmd_t;

/**
 * @brief Function implemented by each command
 */
typedef cmd_sys_err_t (*cmd_invoke_t)(const cmd_sys_cmd_t *cmd, const data_fmt_desc_t *args_desc, const data_fmt_desc_t *resp_desc);

/**
 * @brief Specification for a command to serialize / deserialize arguments and response data
 */
typedef struct {
    cmd_invoke_t invoke;
    const data_fmt_desc_t *args;
    const data_fmt_desc_t *resp;
} cmd_sys_cmd_spec_t;

/**
 * @brief Function pointer for callback after command execution
 */
typedef void (*cmd_sys_callback_t)(cmd_sys_err_t status);

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

cmd_sys_err_t cmd_sys_run(cmd_sys_cmd_t *cmd, uint8_t *buf, cmd_sys_callback_t callback, bool invoke_scheduled, bool *queued_for_exec);

cmd_sys_err_t cmd_sys_invoke_cmd(const cmd_sys_cmd_t *cmd);

cmd_sys_err_t cmd_sys_begin_response(const cmd_sys_cmd_t *cmd, cmd_sys_resp_code_t resp_code, uint32_t resp_data_len);
cmd_sys_err_t cmd_sys_finish_response(const cmd_sys_cmd_t *cmd);

cmd_sys_err_t cmd_sys_handle_args(const cmd_sys_cmd_t *cmd, void *args_struct, const data_fmt_desc_t *args_desc, uint32_t args_len, uint8_t *buf);
cmd_sys_err_t cmd_sys_handle_resp(const cmd_sys_cmd_t *cmd, const void *resp_struct, const data_fmt_desc_t *resp_desc, uint32_t resp_len, uint8_t *buf, cmd_sys_resp_code_t resp_code);
cmd_sys_err_t cmd_sys_handle_resp_fields(const cmd_sys_cmd_t *cmd, const void *resp_struct, const data_fmt_desc_t *resp_desc, uint32_t resp_len, uint8_t *buf);

#endif // CMD_SYS_H_
