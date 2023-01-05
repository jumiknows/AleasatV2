/**
 * @file cmd_sys_imm.c
 * @brief Command System for immediate commands
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_imm.h"

// Command System
#include "cmd_sys.h"
#include "cmd_sys_sched.h"

// Serial
#include "obc_serial_socket.h"

// OBC
#include "obc_task_info.h"
#include "obc_rtos.h"
#include "obc_watchdog.h"
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define NOTIFICATION_INDEX 1U // index 0 is used by stream/message buffers
                              // see https://www.freertos.org/RTOS-task-notifications.html

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void cmd_sys_imm_task(void* pvParameters);
static void cmd_sys_imm_exec_callback(cmd_sys_err_t status);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static TaskHandle_t task_handle = NULL;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Start the command system task for immediate commands
 */
void cmd_sys_imm_start_task(void) {
    static StaticTask_t task_buffer = { 0 };
    static StackType_t task_stack[CMD_SYS_IMM_TASK_STACK_SIZE];

    task_handle = task_create_static(&cmd_sys_imm_task, "cmd_sys_imm", CMD_SYS_IMM_TASK_STACK_SIZE, NULL, CMD_SYS_IMM_TASK_PRIORITY, task_stack, &task_buffer);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Command system task for immediate commands
 * 
 * @param pvParameters Task parameters (see obc_rtos)
 */
static void cmd_sys_imm_task(void* pvParameters) {
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    static uint8_t buf[CMD_SYS_SCHED_MAX_DATA_SIZE] = { 0 };

    static cmd_sys_cmd_t cmd = { 0 };
    cmd.input = &obc_serial_socket_in;
    cmd.output = &obc_serial_socket_out;

    // TODO: ALEA-862 eventually mark task awake
    set_task_status(wd_task_id, task_asleep);

    while (1) {
        bool queued_for_exec = false;

        // Run the command system 
        cmd_sys_err_t err = cmd_sys_run(&cmd, buf, &cmd_sys_imm_exec_callback, false, &queued_for_exec);

        if (err == CMD_SYS_SUCCESS) {
            if (queued_for_exec) {
                // Wait for a notification from the cmd_sys_exec task
                uint32_t notification_value = 0;
                if (xTaskNotifyWaitIndexed(NOTIFICATION_INDEX, 0U, 0xFFFFFFFFU, &notification_value, portMAX_DELAY) == pdTRUE) {
                    err = (cmd_sys_err_t)notification_value;
                    // TODO: ALEA-857 do something with err
                }
            }
        }
    }
}

static void cmd_sys_imm_exec_callback(cmd_sys_err_t status) {
    xTaskNotifyIndexed(task_handle, NOTIFICATION_INDEX, (uint32_t)status, eSetValueWithOverwrite);
}
