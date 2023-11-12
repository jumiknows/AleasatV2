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

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static TaskHandle_t create_task(obc_task_id_t id, TaskFunction_t task_func, void* const params, obc_watchdog_action_t watchdog_action, bool privileged);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static TaskHandle_t task_handles[OBC_TASK_COUNT] = { 0 };

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

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
TaskHandle_t obc_rtos_create_task(obc_task_id_t id, TaskFunction_t task_func, void* const params, obc_watchdog_action_t watchdog_action) {
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
TaskHandle_t obc_rtos_create_task_privileged(obc_task_id_t id, TaskFunction_t task_func, void* const params, obc_watchdog_action_t watchdog_action) {
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
    // TODO ALEA-967
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
void obc_rtos_get_idle_task_memory(StaticTask_t** task_buffer, StackType_t** stack, uint32_t* stack_size) {
    const obc_task_spec_t *task_spec = &OBC_TASK_SPEC_TABLE[OBC_TASK_ID_IDLE];

    *task_buffer = task_spec->task_buffer;
    *stack       = task_spec->stack;
    *stack_size  = task_spec->stack_size;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static TaskHandle_t create_task(obc_task_id_t id, TaskFunction_t task_func, void* const params, obc_watchdog_action_t watchdog_action, bool privileged) {
    const obc_task_spec_t *task_spec = &OBC_TASK_SPEC_TABLE[id];

    obc_watchdog_watch_task(id, watchdog_action);

    uint32_t priority = task_spec->priority;
    if (privileged) {
        priority |= portPRIVILEGE_BIT;
    }

    TaskHandle_t handle = xTaskCreateStatic(task_func, task_spec->name, task_spec->stack_size, params, priority, task_spec->stack, task_spec->task_buffer);
    vTaskSetApplicationTaskTag(handle, task_spec->hook_func);

    task_handles[id] = handle;

    return handle;
}
