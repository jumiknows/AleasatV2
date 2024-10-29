/**
 * @file cmd_impl_rtos.c
 * @brief Implementation of commands related to the RTOS
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// OBC
#include "obc_rtos.h"
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

cmd_sys_err_t cmd_impl_GET_MIN_STACK_SPACE(const cmd_sys_cmd_t *cmd) {
    cmd_sys_err_t err = CMD_SYS_SUCCESS;

    // Although the FreeRTOS API returns a uint32_t, we know in practice none of our stack sizes come close
    // to the limit of a uint16_t so we will use that datatype so save on communication bandwidth
    uint16_t min_stack_space = 0;
    uint8_t resp_buf[sizeof(min_stack_space) * OBC_TASK_COUNT] = { 0 };
    uint8_t *resp_buf_ptr = resp_buf;

    // Iterate over the tasks and record the minimum free stack space (called the high watermark by FreeRTOS)
    for (uint8_t task_id = 0; task_id < OBC_TASK_COUNT; task_id++) {
        TaskHandle_t task_handle = obc_rtos_get_task_handle((obc_task_id_t)task_id);

        if (task_handle == NULL) {
            err = cmd_sys_begin_response(cmd, CMD_SYS_RESP_CODE_ERROR, 0);

            if (err != CMD_SYS_SUCCESS) {
                return err;
            }

            err = cmd_sys_finish_response(cmd);
            return err;
        }

        min_stack_space = (uint16_t)uxTaskGetStackHighWaterMark(task_handle);
        data_fmt_u16_to_arr_be(min_stack_space, resp_buf_ptr);
        resp_buf_ptr += sizeof(min_stack_space);
    }

    // Send response
    err = cmd_sys_begin_response(cmd, CMD_SYS_RESP_CODE_SUCCESS, sizeof(resp_buf));

    if (err != CMD_SYS_SUCCESS) {
        return err;
    }

    uint32_t bytes_written = io_stream_write(cmd->output, resp_buf, sizeof(resp_buf), pdMS_TO_TICKS(CMD_SYS_OUTPUT_WRITE_TIMEOUT_MS), NULL);

    if (bytes_written != sizeof(resp_buf)) {
        return CMD_SYS_ERR_WRITE_TIMEOUT;
    }

    err = cmd_sys_finish_response(cmd);

    if (err != CMD_SYS_SUCCESS) {
        return err;
    }

    return err;
}

cmd_sys_err_t cmd_impl_CAPTURE_RTOS_TRACE(const cmd_sys_cmd_t *cmd, cmd_CAPTURE_RTOS_TRACE_args_t *args) {
    static uint32_t trace_entries[256] = { 0 };

    uint32_t len = MIN(LEN(trace_entries), args->length);
    obc_rtos_trace_freeze(trace_entries, len);

    // Send response
    cmd_sys_err_t err = CMD_SYS_SUCCESS;

    err = cmd_sys_begin_response(cmd, CMD_SYS_RESP_CODE_SUCCESS, (len * sizeof(trace_entries[0])));

    if (err != CMD_SYS_SUCCESS) {
        return err;
    }

    // Send data 8 entries at a time
    uint8_t buf[8 * sizeof(trace_entries[0])];
    uint8_t buf_idx = 0;

    for (uint32_t i = 0; i < len; i++) {
        data_fmt_u32_to_arr_be(trace_entries[i], &buf[buf_idx]);
        buf_idx += sizeof(trace_entries[0]);

        if ((buf_idx == sizeof(buf)) || (i == (len - 1))) {
            // buf is full OR last entry is processed
            if (io_stream_write(cmd->output, buf, buf_idx, pdMS_TO_TICKS(CMD_SYS_OUTPUT_WRITE_TIMEOUT_MS), NULL) != buf_idx) {
                return CMD_SYS_ERR_WRITE_TIMEOUT;
            }

            buf_idx = 0;
        }
    }

    err = cmd_sys_finish_response(cmd);

    if (err != CMD_SYS_SUCCESS) {
        return err;
    }

    return err;
}

cmd_sys_resp_code_t cmd_impl_RTOS_STATE(const cmd_sys_cmd_t *cmd) {
    // TODO: requires work from ALEA-828
    return CMD_SYS_RESP_CODE_NOT_IMPL;
}

cmd_sys_resp_code_t cmd_impl_TASK_RUNTIME(const cmd_sys_cmd_t *cmd, cmd_TASK_RUNTIME_args_t *args, cmd_TASK_RUNTIME_resp_t *resp) {
    // First get task in arg's run time
    TaskHandle_t arg_task_handle = obc_rtos_get_task_handle((obc_task_id_t)args->task);
    TaskStatus_t arg_task_details;

    if (arg_task_handle == NULL) { // Check if task id out of bounds
        return CMD_SYS_RESP_CODE_ERROR;
    }

    vTaskGetInfo(arg_task_handle, &arg_task_details, pdFALSE, eRunning);

    resp->runtime =  arg_task_details.ulRunTimeCounter; // ARG task run time

    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_CPU_USAGE(const cmd_sys_cmd_t *cmd, cmd_CPU_USAGE_args_t *args, cmd_CPU_USAGE_resp_t *resp) {
    // First get task in arg's run time
    TaskHandle_t arg_task_handle = obc_rtos_get_task_handle((obc_task_id_t)args->task);
    TaskStatus_t arg_task_details;

    if (arg_task_handle == NULL) { // Check if task id out of bounds
        return CMD_SYS_RESP_CODE_ERROR;
    }

    vTaskGetInfo(arg_task_handle, &arg_task_details, pdFALSE, eRunning);
    uint32_t arg_task_run_time = arg_task_details.ulRunTimeCounter; // ARG task run time

    uint64_t all_tasks_run_time = 0;
    // This is uint64_t as we don't want two nearly overflowing uint32_t's adding and overflowing the uint32_t

    for (uint8_t task_id = 0; task_id < OBC_TASK_COUNT; task_id++) {
        TaskHandle_t task_handle = obc_rtos_get_task_handle((obc_task_id_t)task_id);
        TaskStatus_t task_details;

        vTaskGetInfo(task_handle, &task_details, pdFALSE, eRunning);
        // Now task_details should be full of details
        all_tasks_run_time += task_details.ulRunTimeCounter;
    }

    float all_tasksF = (float)all_tasks_run_time;
    float arg_taskF = (float)arg_task_run_time;

    float ratio = arg_taskF / all_tasksF; // Ratio will always be between 0.0 and 1.0

    resp->usage_pct = (uint8_t)(ratio * 100);

    return CMD_SYS_RESP_CODE_SUCCESS;
}
