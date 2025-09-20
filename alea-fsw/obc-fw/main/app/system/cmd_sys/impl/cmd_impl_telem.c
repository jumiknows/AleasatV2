
/**
 * @file cmd_impl_telem.c
 * @brief Implementation of telem commands.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// OBC
#include "obc_featuredefs.h"
#include "telem.h"
#include "telem_gen.h"
#include "telem_error.h"
#include "lfs.h"
#include "obc_filesystem.h"

// Logger
#include "logger.h"

// Utils
#include "obc_utils.h"
#include "obc_watchdog.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

cmd_sys_err_t cmd_impl_GET_TELEMETRY(const cmd_sys_cmd_t *cmd, cmd_GET_TELEMETRY_args_t *args) {
    // Telemetry collection is disabled during downlink to prevent file read/write conflicts
    telem_collect_disable();

    char filename[17] = "";
    convert_priority_to_filename(filename, args->priority);

    lfs_file_t file = { 0 };

    if (fs_open(&file, filename, 1000) != FS_OK) {
        telem_collect_enable();
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    int32_t file_size = fs_size(&file);

    uint16_t chunk_size = 800;
    int32_t bytes_left = MIN(args->size, file_size);
    uint8_t read_buf[800];

    cmd_sys_err_t err = cmd_sys_begin_response(cmd, CMD_SYS_RESP_CODE_SUCCESS, bytes_left);

    if (err != CMD_SYS_SUCCESS) {
        fs_close(&file);
        telem_collect_enable();
        return err;
    }

    if (fs_seek(&file, 0, FS_SEEK_END) != FS_OK) {
        telem_collect_enable();
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    while (bytes_left > 0) {
        int32_t bytes_to_read = MIN(chunk_size, bytes_left);

        if (fs_seek(&file, -bytes_to_read, FS_SEEK_CUR) != FS_OK) {
            telem_collect_enable();
            return CMD_SYS_ERR_INVALID_ARGS;
        }

        if (fs_read(&file, read_buf, bytes_to_read) != FS_OK) {
            telem_collect_enable();
            return CMD_SYS_ERR_INVALID_ARGS;
        }

        uint32_t bytes_written = io_stream_write(cmd->output, read_buf, bytes_to_read, pdMS_TO_TICKS(CMD_SYS_OUTPUT_WRITE_TIMEOUT_MS), NULL);

        if (bytes_written != bytes_to_read) {
            fs_close(&file);
            telem_collect_enable();
            return CMD_SYS_ERR_WRITE_TIMEOUT;
        }

        if (fs_seek(&file, -bytes_to_read, FS_SEEK_CUR) != FS_OK) {
            telem_collect_enable();
            return CMD_SYS_ERR_INVALID_ARGS;
        }

        bytes_left -= bytes_to_read;

        obc_watchdog_pet(OBC_TASK_ID_CMD_SYS_EXEC);
    }

    // Truncate what was read
    int32_t new_file_size = file_size - MIN(args->size, file_size);
    fs_truncate(&file, new_file_size);

    fs_close(&file);
    telem_collect_enable();

    return cmd_sys_finish_response(cmd);
}
