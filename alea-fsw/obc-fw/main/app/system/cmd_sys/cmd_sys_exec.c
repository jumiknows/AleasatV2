/**
 * @file cmd_sys_exe.c
 * @brief Command system executor
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_exec.h"

// Command System
#include "cmd_sys.h"

// OBC
#include "obc_rtc.h"
#include "obc_watchdog.h"
#include "obc_rtos.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define CMD_QUEUE_LENGTH     2U
#define CMD_QUEUE_ITEM_SIZE  sizeof(cmd_sys_exec_queue_item_t)

#define CMD_SYS_EXEC_POLL_PERIOD_MS 1000U

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    cmd_sys_cmd_t *cmd;
    cmd_sys_exec_callback_t callback;
    void *arg;
} cmd_sys_exec_queue_item_t;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void cmd_sys_exec_task(void *pvParameters);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static QueueHandle_t cmd_queue = NULL;
static uint8_t cmd_queue_storage[CMD_QUEUE_LENGTH * CMD_QUEUE_ITEM_SIZE] = { 0 };
static StaticQueue_t cmd_queue_buf = { 0 };

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize FreeRTOS data structures for the command system executor
 */
void cmd_sys_exec_pre_init(void) {
    cmd_queue = xQueueCreateStatic(CMD_QUEUE_LENGTH, CMD_QUEUE_ITEM_SIZE, cmd_queue_storage, &cmd_queue_buf);
    obc_rtos_create_task(OBC_TASK_ID_CMD_SYS_EXEC, &cmd_sys_exec_task, NULL, OBC_WATCHDOG_ACTION_ALLOW);
}

/**
 * @brief Added a command to the queue for execution
 *
 * @param[in] cmd           Pointer to command struct to add to the queue. The pointer itself will be stored in the queue
 *                          so the object pointed to must remain valid and unmodified until the callback is invoked.
 * @param[in] callback      Optional function that is called after the command is executed and passed the return value of the call
 *                          to cmd_sys_invoke_cmd
 * @param[in] arg           Argument that will be passed to the callback
 * @param[in] timeout_ticks Timeout (ticks) to wait for space in the queue
 *
 * @return Status code:
 *            - CMD_SYS_SUCCESS if the command was added to the queue successfully
 *            - CMD_SYS_ERR_EXEC_TIMEOUT if a timeout occurred waiting for space in the queue
 */
cmd_sys_err_t cmd_sys_exec_enqueue(cmd_sys_cmd_t *cmd, cmd_sys_exec_callback_t callback, void *arg, uint32_t timeout_ticks) {
    cmd_sys_exec_queue_item_t queue_item = {
        .cmd      = cmd,
        .callback = callback,
        .arg      = arg
    };

    cmd_sys_err_t err = CMD_SYS_ERR_EXEC_Q_TIMEOUT;

    if (xQueueSend(cmd_queue, &queue_item, timeout_ticks) == pdTRUE) {
        err = CMD_SYS_SUCCESS;
    }

    return err;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static void cmd_sys_exec_task(void *pvParameters) {
    cmd_sys_exec_queue_item_t queue_item;

    vPortTaskUsesFPU();

    while (1) {

        obc_watchdog_pet(OBC_TASK_ID_CMD_SYS_EXEC);

        if (xQueueReceive(cmd_queue, &queue_item, pdMS_TO_TICKS(CMD_SYS_EXEC_POLL_PERIOD_MS)) == pdTRUE) {
            // Set exec_timestamp field of command
            uint32_t exec_timestamp = rtc_get_epoch_time();
            queue_item.cmd->exec_timestamp = exec_timestamp;

            cmd_sys_err_t err = cmd_sys_invoke_cmd(queue_item.cmd);

            if (queue_item.callback != NULL) {
                queue_item.callback(err, queue_item.arg);
            }
        }
    }
}
