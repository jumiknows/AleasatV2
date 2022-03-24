/**
 * @file hang.c
 * @brief Tasks that hangs for testing purposes.
 * @author Victor Leon
 * @date May 19, 2020
 */

#include "obc_watchdog.h"
#include "obc_gpio.h"
#include "rtos.h"
#include "blinky.h"
#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"
#include "obc_task_info.h"
#include "obc_rtos.h"
#include "hang.h"

// Definition
#define HANG_TASK_PERIOD_MS 1000 // Run the do-nothing task every second

// Variables
static task_id_t wd_task_id;

// Private Functions
static void vHangTask(void* pvParameters);

/**
 * @brief Starts the do-nothing task. Once a command is issued,
 * causes task to hang.
 */
void hang_start_task(void) {
    static StaticTask_t hang_task_buf                        = {NULL};
    static StackType_t hang_task_stack[HANG_TASK_STACK_SIZE] = {NULL};
    task_create_periodic_static(&vHangTask, "hang_task", HANG_TASK_STACK_SIZE, NULL, HANG_TASK_DEFAULT_PRIORITY, hang_task_stack, &hang_task_buf, 1000);
}

/**
 * @brief Task that runs every second. This task will only hang when a command is issued, setting
 * the task status to alive.
 *
 * @param pvParameters: Any parameters passed to this task when creating it.
 */
static void vHangTask(void* pvParameters) {
    wd_task_id                              = WD_TASK_ID(pvParameters);
    watchdog_task_status_t hang_task_status = task_unknown;
    set_task_status(wd_task_id, task_asleep);

    while (1) {
        bool err = get_task_status(wd_task_id, &hang_task_status);
        if (hang_task_status == task_alive) {
            while (1) {
                // Do Nothing, hang task
            }
        }
        vTaskDelay(pdMS_TO_TICKS(HANG_TASK_PERIOD_MS)); // Run every second
    }
}

/**
 * @brief Updates the vHangTask task status to alive. Used to link with a command.
 */
bool set_hang_task_status_alive(void) {
    return set_task_status(wd_task_id, task_alive);
}
