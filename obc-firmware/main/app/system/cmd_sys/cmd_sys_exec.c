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
#include "obc_watchdog.h"
#include "obc_task_info.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define CMD_QUEUE_LENGTH     2U
#define CMD_QUEUE_ITEM_SIZE  sizeof(cmd_sys_exec_queue_item_t)

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    const cmd_sys_cmd_t *cmd;
    cmd_sys_callback_t callback;
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
void cmd_sys_exec_create_infra(void) {
    cmd_queue = xQueueCreateStatic(CMD_QUEUE_LENGTH, CMD_QUEUE_ITEM_SIZE, cmd_queue_storage, &cmd_queue_buf);
}

/**
 * @brief Start the command system task for executing commands
 */
void cmd_sys_exec_start_task(void) {
    static StaticTask_t task_buffer                             = { 0 };
    static StackType_t task_stack[CMD_SYS_EXEC_TASK_STACK_SIZE] = { 0 };

    task_create_static(&cmd_sys_exec_task, "cmd_sys_exec", CMD_SYS_EXEC_TASK_STACK_SIZE, NULL, CMD_SYS_EXEC_TASK_PRIORITY, task_stack, &task_buffer);
}

/**
 * @brief Added a command to the queue for execution
 * 
 * @param[in] cmd           Pointer to command struct to add to the queue. The pointer itself will be stored in the queue
 *                          so the object pointed to must remain valid and unmodified until the callback is invoked.
 * @param[in] callback      Function that is called after the command is executed and passed the return value of the call
 *                          to cmd_sys_invoke_cmd
 * @param[in] timeout_ticks Timeout (ticks) to wait for space in the queue
 * 
 * @return Status code:
 *            - CMD_SYS_SUCCESS if the command was added to the queue successfully
 *            - CMD_SYS_ERR_EXEC_TIMEOUT if a timeout occurred waiting for space in the queue
 */
cmd_sys_err_t cmd_sys_exec_enqueue(const cmd_sys_cmd_t *cmd, cmd_sys_callback_t callback, uint32_t timeout_ticks) {
    cmd_sys_exec_queue_item_t queue_item = {
        .cmd      = cmd,
        .callback = callback,
    };

    cmd_sys_err_t err = CMD_SYS_ERR_EXEC_TIMEOUT;

    if (xQueueSend(cmd_queue, &queue_item, timeout_ticks) == pdTRUE) {
        err = CMD_SYS_SUCCESS;
    }

    return err;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static void cmd_sys_exec_task(void *pvParameters) {
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    cmd_sys_exec_queue_item_t queue_item;

    while (1) {
        set_task_status(wd_task_id, task_asleep);
        if (xQueueReceive(cmd_queue, &queue_item, portMAX_DELAY) == pdTRUE) {
            set_task_status(wd_task_id, task_alive);

            cmd_sys_err_t err = cmd_sys_invoke_cmd(queue_item.cmd);

            if (queue_item.callback != NULL) {
                queue_item.callback(err);
            }
        }
    }
}
