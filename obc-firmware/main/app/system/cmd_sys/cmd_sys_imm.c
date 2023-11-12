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

// Ground Station
#include "gndstn_link.h"

// OBC
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

#define CMD_SYS_IMM_POLL_PERIOD_MS  1000U
#define CMD_SYS_IMM_EXEC_TIMEOUT_MS 60000U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void cmd_sys_imm_task(void* pvParameters);
static void exec_wait_callback(void);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Start the command system task for immediate commands
 */
void cmd_sys_imm_start_task(void) {
    obc_rtos_create_task(OBC_TASK_ID_CMD_SYS_IMM, &cmd_sys_imm_task, NULL, OBC_WATCHDOG_ACTION_ALLOW);
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
    static uint8_t buf[CMD_SYS_SCHED_MAX_DATA_SIZE] = { 0 };

    static cmd_sys_cmd_t cmd = { 0 };
    cmd.input = &gndstn_uplink_socket;
    cmd.output = &gndstn_downlink_socket;

    while (1) {
        obc_watchdog_pet(OBC_TASK_ID_CMD_SYS_IMM);

        cmd_sys_err_t err = cmd_sys_recv_header(&cmd, buf, pdMS_TO_TICKS(CMD_SYS_IMM_POLL_PERIOD_MS));
        if (err == CMD_SYS_SUCCESS) {
            if (cmd.header.timestamp == CMD_SYS_TIMESTAMP_IMMEDIATE) {
                err = cmd_sys_execute(&cmd, pdMS_TO_TICKS(CMD_SYS_IMM_POLL_PERIOD_MS), pdMS_TO_TICKS(CMD_SYS_IMM_EXEC_TIMEOUT_MS), &exec_wait_callback);
            } else {
                err = cmd_sys_schedule_cmd(&cmd, buf);
            }

            // TODO: ALEA-857 do something with err
        }
    }
}

static void exec_wait_callback(void) {
    // Pet the watchdog while we're waiting for the command to finish executing
    obc_watchdog_pet(OBC_TASK_ID_CMD_SYS_IMM);
}
