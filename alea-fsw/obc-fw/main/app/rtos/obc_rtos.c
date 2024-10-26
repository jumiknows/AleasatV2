/**
 * @file obc_rtos.c
 * @brief RTOS management
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_rtos.h"

// OBC
#include "obc_tasks_gen.h"
#include "obc_watchdog.h"

// Utils
#include "circ_buf.h"
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include "string.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define TRACE_LENGTH 256U

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    circ_buf_t circ_buf;
    uint32_t last_time_us;
} obc_rtos_trace_t;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static TaskHandle_t create_task(obc_task_id_t id, TaskFunction_t task_func, void *const params, obc_watchdog_action_t watchdog_action,
                                bool privileged);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static TaskHandle_t task_handles[OBC_TASK_COUNT] = { 0 };

static uint32_t trace_entries[TRACE_LENGTH];
static obc_rtos_trace_t trace_info = {
    .circ_buf = {
        .buf       = trace_entries,
        .count     = LEN(trace_entries),
        .idx       = 0,
        .item_size = sizeof(trace_entries[0])
    },
    .last_time_us = 0
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void obc_rtos_init(void) {
    TaskHandle_t idle_task_handle = xTaskGetIdleTaskHandle();
    task_handles[OBC_TASK_ID_IDLE] = idle_task_handle;

    obc_task_hook_t idle_hook = OBC_TASK_SPEC_TABLE[OBC_TASK_ID_IDLE].hook_func;
    vTaskSetApplicationTaskTag(idle_task_handle, idle_hook);
}

/**
 * @brief Instantiate the FreeRTOS task for the given ID.
 *
 * All resources for the task (stack and buffer) have been statically allocated
 * in the generated file: obc_tasks_gen.c
 *
 * @param id               Task ID (see obc_tasks_ids_gen.h)
 * @param task_func        Pointer to the task function
 * @param params           Data that will be passed as the only argument to the task function
 * @param watchdog_action  Action for the software watchdog to take if the task stops responding
 * @return TaskHandle_t    FreeRTOS handle to the task
 */
TaskHandle_t obc_rtos_create_task(obc_task_id_t id, TaskFunction_t task_func, void *const params, obc_watchdog_action_t watchdog_action) {
    return create_task(id, task_func, params, watchdog_action, false);
}

/**
 * @brief Instantiate the FreeRTOS task for the given ID.
 *
 * All resources for the task (stack and buffer) have been statically allocated
 * in the generated file: obc_tasks_gen.c
 *
 * The task will be created in privileged mode (with portPRIVILEGE_BIT set).
 *
 * @param id               Task ID (see obc_tasks_ids_gen.h)
 * @param task_func        Pointer to the task function
 * @param params           Data that will be passed as the only argument to the task function
 * @param watchdog_action  Action for the software watchdog to take if the task stops responding
 * @return TaskHandle_t    FreeRTOS handle to the task
 */
TaskHandle_t obc_rtos_create_task_privileged(obc_task_id_t id, TaskFunction_t task_func, void *const params, obc_watchdog_action_t watchdog_action) {
    return create_task(id, task_func, params, watchdog_action, true);
}

/**
 * @brief Centralized handler for task hook functions.
 *
 * @param id  ID of the task whose hook was called
 * @param arg Parameter identifying what triggered the hook
 * @return BaseType_t General-purpose return value that will be passed up the call chain
 */
BaseType_t obc_rtos_task_hook(obc_task_id_t id, void *arg) {
    // Event is a single bit
    uint32_t event = (uint32_t)((bool)((uintptr_t)arg));
    uint32_t time = SYSTEM_TIME_US();
    uint32_t delta_t = (time - trace_info.last_time_us);

    uint32_t trace_entry = (delta_t << 6) | (event << 5) | (uint32_t)id;

    circ_buf_insert(&(trace_info.circ_buf), &trace_entry);

    trace_info.last_time_us = time;
    return 0;
}

/**
 * @brief Retrieve the FreeRTOS handle for a task given its ID
 */
TaskHandle_t obc_rtos_get_task_handle(obc_task_id_t id) {
    return task_handles[id];
}

/**
 * @brief Retrieve pointers to the memory allocated for the IDLE task.
 *
 * @param[out] task_buffer Load with a pointer to the IDLE task buffer
 * @param[out] stack       Load with a pointer to the IDLE stack
 * @param[out] stack_size  Load with the IDLE stack size
 */
void obc_rtos_get_idle_task_memory(StaticTask_t **task_buffer, StackType_t **stack, uint32_t *stack_size) {
    const obc_task_spec_t *task_spec = &OBC_TASK_SPEC_TABLE[OBC_TASK_ID_IDLE];

    *task_buffer = task_spec->task_buffer;
    *stack       = task_spec->stack;
    *stack_size  = task_spec->stack_size;
}

uint32_t obc_rtos_trace_freeze(uint32_t *trace_dest, uint32_t len) {
    taskENTER_CRITICAL();
    uint32_t copied = circ_buf_copy_newest(&(trace_info.circ_buf), trace_dest, len);
    taskEXIT_CRITICAL();

    return copied;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static TaskHandle_t create_task(obc_task_id_t id, TaskFunction_t task_func, void *const params, obc_watchdog_action_t watchdog_action,
                                bool privileged) {
    const obc_task_spec_t *task_spec = &OBC_TASK_SPEC_TABLE[id];

    obc_watchdog_watch_task(id, watchdog_action);

    uint32_t priority = task_spec->priority;

    if (privileged) {
        priority |= portPRIVILEGE_BIT;
    }

    TaskHandle_t handle = xTaskCreateStatic(task_func, task_spec->name, task_spec->stack_size, params, priority, task_spec->stack,
                                            task_spec->task_buffer);
    vTaskSetApplicationTaskTag(handle, task_spec->hook_func);

    task_handles[id] = handle;

    return handle;
}
