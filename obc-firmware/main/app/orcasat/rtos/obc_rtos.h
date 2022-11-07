/**
 * @file obc_rtos.h
 * @brief Wrappers for FreeRTOS functions.
 * @date Dec 14, 2019
 * @author Lana B, Richard A
 *
 * The OBC RTOS infrastructure wraps standard FreeRTOS functions like xTaskCreate(), allowing us
 * to track details that need to be associated with a particular task.
 *
 * When tasks are created using @ref task_create and @ref task_create_static,
 * each task also gets the following information associated with it:
 *     - an ID, used to index an array of task information
 *     - period_ms, which periodic tasks should use to control their delay as an argument to
 * vTaskDelay
 *     - The task name string
 *     - A pointer to the regular pvParameters argument, which is a void pointer to any information
 * that a task may use however it needs to.
 *
 * There is a _periodic counterpart to both task_create and task_create_static. The periodic API
 * allows a configurable task period to be set at task creation time. Tasks created with the
 * non-periodic API must not call vTaskDelay or similar, and should instead rely on blocks on RTOS
 * primitives such as semaphores, rather than task delays.
 *
 * This set of information is contained in the @ref task_params_t structure, which is passed in to
 * each task created with this API. This gives the task access to the values, and since these values
 * are stored per-task but outside of the task, it allows us to do things like reconfigure the delay
 * of a task from outside the task itself.
 *
 * GUIDELINES
 * 	- Prefer the _static API over the non-static API.
 * 	- A periodic task is any that uses vTaskDelay on its task period. Non-periodic tasks may use
 * 	  vTaskDelay or similar, but can't use it on their task period (which is set to 0 at creation
 * time).
 */

#ifndef ORCASAT_TASKS_OBC_RTOS_H_
#define ORCASAT_TASKS_OBC_RTOS_H_

#include <stdbool.h>
#include "obc_error.h"
#include "rtos.h"

/**
 * @brief The maximum number of tasks that can be created with the obc_rtos API.
 *
 * When adding a new task, please increment this number.
 */
#define MAX_NUM_TASKS 16

/**
 * @brief Getters for values of the @ref task_params_t structure, suitable
 * for use within tasks created with this API.
 *
 * For example, if a task has its pvParameters parameter called pvParameters and it
 * needs to get the period_ms value for use in vTaskDelay, the following call can be
 * made:
 * 			vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD_MS(pvParameters)));
 *
 * 	WD_TASK_ID: gets the task ID.
 * 	TASK_PERIOD_MS: gets the task's period.
 */
#define WD_TASK_ID(PARAMS)     (*((struct task_parameters*)PARAMS)).task_id
#define TASK_PERIOD_MS(PARAMS) (*((struct task_parameters*)PARAMS)).period_ms

/**
 * @brief Task period type.
 */
typedef int32_t task_period_t;

/**
 * @brief OBC task parameter structure. This structure is passed to each task created using the OBC
 * task creation infrastructure.
 *
 * It includes the OBC task ID, an optional period that is configurable, a pointer to the task name,
 * and a pointer to any custom parameters the user may have passed in during task creation.
 */
typedef struct task_parameters {
    /**
     * @brief Task ID for internal tracking.
     */
    uint8_t task_id;

    /**
     * Period in ms. If set to 0 at task creation time, the API will not allow it to be changed
     * further. A period of 0 indicates a task that is not periodic, and should therefore never call
     * vTaskDelay or similar.
     */
    task_period_t period_ms;

    /**
     * @brief Pointer to the task name. Useful for debugging, such as printing the names of all
     * tasks.
     */
    const char* task_name;

    /**
     * @brief User parameters. Point this towards a structure if a task requires parameters from
     * elsewhere.
     */
    void* parameters;
} task_params_t;

#ifdef CEEDLING_TEST
/**
 * @brief A function that resets infrastructure, only to be called during unit testing with
 * Ceedling.
 */
void unit_test_reset_obc_rtos(void);
#endif

/**
 * @brief Initializes the RTOS features.
 */
void obc_rtos_create_infra(void);

/**
 * @brief Gets the number of tasks created using the obc_rtos API.
 *
 * @return The number of tasks.
 */
uint8_t number_of_tasks(void);

/**
 * @brief Returns true if the task ID is valid.
 */
bool task_id_valid(uint8_t task_id);

/**
 * @brief Prints out the created tasks.
 */
void print_tasks(void);

/**
 * @brief Prints out the status of the RTOS infrastructure.
 */
void print_rtos_status(void);

/**
 * @brief Allows the period of a periodic task to be adjusted.
 */
rtos_err_t set_task_period(uint8_t id, task_period_t period_ms);

/**
 * @brief Allows the period of currently running task to be adjusted.
 */
rtos_err_t set_current_task_period(task_period_t period_ms);

/**
 * @brief Gets the FreeRTOS task handle from the task ID.
 */
TaskHandle_t get_task_handle(uint8_t id);

/**
 * @brief Gets the FreeRTOS task handle from the task name.
 */
TaskHandle_t get_task_handle_from_name(const char* task_name);

/**
 * @brief Gets the task ID from the FreeRTOS task handle
 */
bool get_task_id_from_handle(TaskHandle_t handle, uint8_t* id);

/**
 * @brief Gets the task ID from the task's name.
 */
rtos_err_t get_task_id_from_name(const char* name, uint8_t* id);

/**
 * @brief Gets the task period from its ID.
 */
rtos_err_t get_task_period_from_id(uint8_t id, task_period_t* period_ms);

/**
 * @brief Suspends or resumes task by name.
 */
rtos_err_t suspend_task(const char* task_name, bool suspend);

/**
 * @brief Prints the state of a task by name.
 */
rtos_err_t print_task_state(const char* task_name);

/**
 * @brief Creates an aperiodic task with static storage allocation.
 *
 * Prefer this function over task_create.
 */
TaskHandle_t task_create_static(TaskFunction_t pxTaskCode, const char* const pcName, const uint32_t ulStackDepth, void* const pvParameters, UBaseType_t uxPriority, StackType_t* const puxStackBuffer,
                                StaticTask_t* const pxTaskBuffer);

/**
 * @brief Creates a periodic task with static storage allocation.
 *
 * Prefer this function over task_create_periodic.
 */
TaskHandle_t task_create_periodic_static(TaskFunction_t pxTaskCode, const char* const pcName, const uint32_t ulStackDepth, void* const pvParameters, UBaseType_t uxPriority,
                                         StackType_t* const puxStackBuffer, StaticTask_t* const pxTaskBuffer, uint32_t period_ms);

#endif /* ORCASAT_TASKS_OBC_RTOS_H_ */
