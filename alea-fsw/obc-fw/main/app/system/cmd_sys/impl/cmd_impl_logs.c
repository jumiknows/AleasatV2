/**
 * @file cmd_impl_logs.c
 * @brief Implementation of log-related commands
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// OBC
#include "obc_filesystem.h"

// Logger
#include "logger.h"
#include "log_sys.h"

#include "obc_watchdog.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define CHUNK_SIZE 800

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

cmd_sys_err_t cmd_impl_GET_LOGS(const cmd_sys_cmd_t *cmd, cmd_GET_LOGS_args_t *args) {
    if (args->size > FS_LOGGING_PARTITION_SIZE) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    uint8_t read_buf[CHUNK_SIZE];
    int32_t bytes_left = logs_read_begin(read_buf, args->size, CHUNK_SIZE);

    if (bytes_left < 0) {
        return CMD_SYS_ERR_INVALID_STATE;
    }

    cmd_sys_err_t err = cmd_sys_begin_response(cmd, CMD_SYS_RESP_CODE_SUCCESS, (uint32_t)bytes_left);

    if (err != CMD_SYS_SUCCESS) {
        return err;
    }

    while (bytes_left > 0) {
        int32_t bytes_read = logs_read(); // Reads into read_buf

        if (bytes_read < 0) { // FS error
            return CMD_SYS_ERR_INVALID_STATE;
        } else if (bytes_read == 0) {
            logs_read_end();
            return CMD_SYS_ERR_INVALID_STATE;
        }

        uint32_t bytes_written = io_stream_write(cmd->output, read_buf, (uint32_t)bytes_read, pdMS_TO_TICKS(CMD_SYS_OUTPUT_WRITE_TIMEOUT_MS), NULL);

        if (bytes_written != (uint32_t)bytes_read) {
            logs_read_end();
            return CMD_SYS_ERR_WRITE_TIMEOUT;
        }

        bytes_left -= bytes_written;

        obc_watchdog_pet(OBC_TASK_ID_CMD_SYS_EXEC);
    }

    logs_read_end();

    return cmd_sys_finish_response(cmd);
}

cmd_sys_resp_code_t cmd_impl_GET_LOGFILE_INFO(const cmd_sys_cmd_t *cmd, cmd_GET_LOGFILE_INFO_resp_t *resp) {
    struct logfile_info_t info;

    if (!log_sys_get_info(&info)) {
        return CMD_SYS_RESP_CODE_ERROR;
    }

    resp->current_logfile = info.current_logfile;

    for (int x = 0; x < FS_LOGGING_FILES_NUM; x++) {
        resp->logfile_sizes[x] = info.logfile_sizes[x];
    }

    return CMD_SYS_RESP_CODE_SUCCESS;
}

