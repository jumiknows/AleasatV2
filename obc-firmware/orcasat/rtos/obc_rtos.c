/**
 * @file obc_rtos.c
 * @brief Wrappers for FreeRTOS functions.
 * @date Dec 14, 2019
 * @author Lana B, Richard A
 *
 * For more information, please see obc_rtos.h
 */

#include <stdbool.h>
#include <string.h>
#include "rtos.h"
#include "obc_rtos.h"
#include "logger.h"
#include "stdbool.h"

/**
 * @brief Timeout for mutex grab.
 */
#define MUTEX_TIMEOUT_MS 200

/**
 * @brief The number of tasks created using obc_rtos.c.
 */
uint8_t num_tasks = 0;

/**
 * @brief Array of task handle pointers, indexed by task ID.
 *
 * A task handle is just a void pointer that FreeRTOS knows how to use.
 */
static TaskHandle_t task_handles[MAX_NUM_TASKS];

/**
 * @brief Semaphore to protect access to task_handles array.
 */
SemaphoreHandle_t xTaskArrayMutexHandle;
StaticSemaphore_t xTaskArrayStaticMutex;

/**
 * @brief Array of task parameters.
 */
task_params_t t_params[MAX_NUM_TASKS] = {'\0'};

/**
 * @brief Wrapper for errors that could occur at runtime.
 *
 * The information in this structure can be queried to determine what went wrong.
 *
 * These errors are active true. Take care not to reset them from false (default, inactive)
 * unless there is a problem.
 */
struct obc_rtos_errors {
    /**
     * @brief This will be set if the application tried to create too many tasks.
     *
     * In this case, some earlier tasks will have been overwritten in our task table, but
     * they will still run. This will likely cause a watchdog timeout.
     */
    bool too_many_tasks;

    /**
     * @brief This will be set if freeRTOS task create fails, which can occur if there
     * is not enough memory to create a new task.
     */
    bool rtos_task_create_failed;
};

struct obc_rtos_errors rtos_errors = {.too_many_tasks = false, .rtos_task_create_failed = false};

// Private Function Prototypes
static TaskHandle_t _task_create_static(TaskFunction_t pxTaskCode, const char* const pcName, const uint32_t ulStackDepth, void* const pvParameters, UBaseType_t uxPriority,
                                        StackType_t* const puxStackBuffer, StaticTask_t* const pxTaskBuffer, uint32_t period_ms);
static task_params_t* set_task_params(uint8_t task_id, uint32_t period_ms, const char* const task_name, void* pvParameters);
static bool ok_to_create_new_task(void);
static uint8_t add_task(void);

/**
 * @brief resets and zeros out all of the internal structures during testing with Ceedling.
 *
 * This is meant to be called after every test function runs. Effectively, it resets all
 * of the OBC RTOS infrastructure so each test acts as if it's running right after boot.
 *
 * We define this function to handle testing instead of exposing the structures directly,
 * so that no other code on the OBC can directly access them and potentially do unsafe things.
 *
 * CEEDLING_TEST is a #define that is incorporated only when Ceedling builds the code. We
 * only make this function visible when running unit tests, as it's not applicable to flight.
 * It is defined in the @ref project.yml file that controls Ceedling.
 */
#ifdef CEEDLING_TEST
#include <string.h>
void unit_test_reset_obc_rtos(void) {
    num_tasks = 0; // Re-initialize the number of tasks.

    // Clear out the arrays
    uint8_t zero = 0;
    memcpy((void*)&t_params, &zero, (sizeof(task_params_t) * MAX_NUM_TASKS));
    memcpy((void*)&task_handles, &zero, (sizeof(TaskHandle_t*) * MAX_NUM_TASKS));
}
#endif

/**
 * @brief Initializes the RTOS features.
 */
void obc_rtos_create_infra(void) {
    xTaskArrayMutexHandle = xSemaphoreCreateMutexStatic(&xTaskArrayStaticMutex);
}

/**
 * @brief Gets the number of tasks created using the obc_rtos API.
 *
 * @return The number of tasks.
 */
uint8_t number_of_tasks(void) {
    return num_tasks;
}

/**
 * @brief Checks if the given task ID is valid.
 *
 * Valid IDs are IDs that have been allocated.
 *
 * @param[in] task_id The task ID to examine.
 */
bool task_id_valid(uint8_t task_id) {
    return (task_id < num_tasks);
}

/**
 * @brief Gets the task handle from its ID.
 *
 * @param[in] The ORCASat task ID.
 *
 * @return The FreeRTOS task handle, or NULL if the id is invalid or
 * semaphore could not be grabbed.
 */
TaskHandle_t get_task_handle(uint8_t id) {
    if (xSemaphoreTake(xTaskArrayMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        if (task_id_valid(id)) {
            xSemaphoreGive(xTaskArrayMutexHandle);
            return task_handles[id];
        }

        xSemaphoreGive(xTaskArrayMutexHandle);
    }

    return NULL;
}

/**
 * @brief Gets the task handle from its name.
 *
 * @param[in] The ORCASat task name.
 *
 * @return The FreeRTOS task handle, or NULL if the id is invalid or
 * semaphore could not be grabbed.
 */
TaskHandle_t get_task_handle_from_name(const char* task_name) {
    uint8_t id = 0;
    if (get_task_id_from_name(task_name, &id) == OBC_RTOS_OK) {
        return get_task_handle(id);
    } else {
        return NULL;
    }
}

/**
 * @brief Gets the task ID from its task handle.
 *
 * @param[in] The task handle
 * @param[out] id The ID returned if a task with the desired handle can be found.
 *
 * @return true if handle was found, false if handle was not found or
 * semaphore could not be grabbed.
 */
bool get_task_id_from_handle(TaskHandle_t handle, uint8_t* id) {
    if (xSemaphoreTake(xTaskArrayMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        uint8_t i;
        for (i = 0; i < num_tasks; i++) {
            if (task_handles[i] == handle) {
                xSemaphoreGive(xTaskArrayMutexHandle);
                *id = i;
                return true;
            }
        }
        xSemaphoreGive(xTaskArrayMutexHandle);
    }
    return false;
}

/**
 * @brief Get the task ID from the name.
 *
 * Tasks may not have the same ID between different platforms or different builds of firmware,
 * because IDs are allocated sequentially every time a task create function is called. When code
 * is rearranged or if certain tasks are not created on platforms, the IDs of each task may shift
 * around.
 *
 * For this reason, the best way to refer to a task is by its name, which probably won't change
 * much. This function gets the ID from the name.
 *
 * @param[in] name The task name to search for.
 * @param[out] id The ID returned if a task with the desired name can be found.
 *
 * @return OBC_RTOS_OK if the value in @ref id is usable, INVALID_NAME if no tasks with the desired
 * name can be found.
 */
rtos_err_t get_task_id_from_name(const char* name, uint8_t* id) {
    rtos_err_t res = INVALID_NAME;
    uint8_t i      = 0;
    for (i = 0; i < num_tasks; i++) {
        if (strcmp(name, t_params[i].task_name) == 0) {
            *id = i;
            res = OBC_RTOS_OK;
        }
    }
    return res;
}

/**
 * @brief Gets the task period from its ID.
 *
 * @param[in] id The ORCASat task ID.
 * @param[out] period_ms The period of the task, if True is returned.
 *
 * @return OBC_RTOS_OK if @ref period_ms is valid, otherwise INVALID_ID.
 */
rtos_err_t get_task_period_from_id(uint8_t id, task_period_t* period_ms) {
    if (task_id_valid(id)) {
        *period_ms = t_params[id].period_ms;
        return OBC_RTOS_OK;
    }
    log_str(ERROR, LOG_OBC_TASK, true, "Invalid ID %d", id);
    return INVALID_ID; // Invalid ID was provided
}

/**
 * @brief Change the task period of a task by ID.
 *
 * If the task has a nonzero period, its period will be changed, then it will be brought out of
 * the blocked state (so it will execute), and then the new delay will be used.
 *
 * If the desired task is running at the time of this call, this function will return false and the
 * task's period will not be changed. If this happens, this function can be safely called again.
 *
 * @param[in] id The task ID of the task that will have its period adjusted.
 * @param[in] period_ms The periods in ms that will be set for the task.
 *
 * @retval OBC_RTOS_OK if the period was changed, otherwise TASK_NOT_BLOCKED, INVALID_PERIOD, or
 * INVALID_ID.
 */
rtos_err_t set_task_period(uint8_t id, task_period_t period_ms) {
    rtos_err_t res = OBC_RTOS_OK;
    // Only do this on valid IDs where the task is already periodic and we are requesting a
    // reasonable value.
    if (task_id_valid(id)) {
        if ((t_params[id].period_ms > 0) && (period_ms > 0)) {
            task_period_t old_period_ms = t_params[id].period_ms;
            t_params[id].period_ms      = period_ms;           // Change the period
            TaskHandle_t handle         = get_task_handle(id); // Note: we don't need to check null here because the ID is
                                                               // guaranteed to be valid if we get inside this if statement.

            // Abort the delay, which if the task is not currently running, will cause the task to
            // run and take on the changed period.
            BaseType_t val = xTaskAbortDelay(handle);
            if (val != pdPASS) {
                // Task was not in the blocked state when we tried to abort its delay. Go back to
                // the old period to be safe.
                res                    = TASK_NOT_BLOCKED;
                t_params[id].period_ms = old_period_ms; // Return the period to its original value
                log_str(ERROR, LOG_OBC_TASK, true, "ID %d %s", t_params[id].task_id, t_params[id].task_name);
            } else {
                res = OBC_RTOS_OK;
                log_str(INFO, LOG_OBC_TASK, true, "ID %d %s %d ok.", t_params[id].task_id, t_params[id].task_name, t_params[id].period_ms);
            }
        } else { // Period not valid
            res = INVALID_PERIOD;
        }
    } else { // ID not valid
        res = INVALID_ID;
    }
    return res;
}

/**
 * @brief Change the task period of the currently running task
 *
 * If the task has a nonzero period, its period will be changed.
 * The task keeps running, the new period is applied after current iteration is complete.
 *
 * @param[in] period_ms The periods in ms that will be set for the task.
 *
 * @retval OBC_RTOS_OK if the period was changed, otherwise INVALID_PERIOD, or INVALID_ID.
 */
rtos_err_t set_current_task_period(task_period_t period_ms) {
    rtos_err_t res      = OBC_RTOS_OK;
    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    uint8_t id          = 0;
    if (get_task_id_from_handle(handle, &id)) {
        if ((t_params[id].period_ms > 0) && (period_ms > 0)) {
            t_params[id].period_ms = period_ms;
        } else {
            res = INVALID_PERIOD;
        }
    } else {
        res = INVALID_ID;
    }
    return res;
}

/**
 * @brief Suspends or resumes task by name
 *
 * Works with either periodic or aperiodic tasks.
 * Calls to suspend or resume are not accumulative. Repeated calls to suspend will only suspend
 * once.
 *
 * @param[in] task_name The name of the task to suspend or resume
 * @param[in] suspend A bool value to either suspend if true or resume if false
 *
 * @retval OBC_RTOS_OK if the task is successfully suspended or resumed, otherwise INVALID_NAME.
 */
rtos_err_t suspend_task(const char* task_name, bool suspend) {
    rtos_err_t res                   = OBC_RTOS_OK;
    TaskHandle_t task_suspend_resume = get_task_handle_from_name(task_name);
    if (task_suspend_resume != NULL) {
        if (suspend) {
            vTaskSuspend(task_suspend_resume);
            log_str(INFO, LOG_OBC_TASK, false, "Task %s suspended", task_name);
        } else {
            vTaskResume(task_suspend_resume);
            log_str(INFO, LOG_OBC_TASK, false, "Task %s resumed", task_name);
        }
    } else {
        res = INVALID_NAME;
    }
    return res;
}

/**
 * @brief Prints the state of a task by name.
 *
 * @param[in] task_name The name of the task to check.
 *
 * @retval OBC_RTOS_OK if the task is successfully checked, otherwise INVALID_NAME.
 */
rtos_err_t print_task_state(const char* task_name) {
    rtos_err_t res           = OBC_RTOS_OK;
    TaskHandle_t task_handle = get_task_handle_from_name(task_name);
    if (task_handle != NULL) {
        switch (eTaskGetState(task_handle)) {
            case eReady:
                log_str(INFO, LOG_OBC_TASK, false, "Task %s ready", task_name);
                break;
            case eRunning:
                log_str(INFO, LOG_OBC_TASK, false, "Task %s running", task_name);
                break;
            case eBlocked:
                log_str(INFO, LOG_OBC_TASK, false, "Task %s blocked", task_name);
                break;
            case eSuspended:
                log_str(INFO, LOG_OBC_TASK, false, "Task %s suspended", task_name);
                break;
            case eDeleted:
                log_str(INFO, LOG_OBC_TASK, false, "Task %s deleted", task_name);
                break;
            default:
                log_str(ERROR, LOG_OBC_TASK, false, "Task %s state error", task_name);
                break;
        }
    } else {
        res = INVALID_NAME;
    }
    return res;
}

/**
 * @brief Prints the task name, period, priority, and ID for all tasks registered in the system.
 */
void print_tasks(void) {
    log_str(INFO, LOG_GET_TASKS_CMD, true, "num_tasks %d", num_tasks);
    uint8_t i = 0;
    for (i = 0; i < num_tasks; i++) {
        log_str(INFO, LOG_GET_TASKS_CMD, false /* don't log to filesystem */, "ID %d, %s", t_params[i].task_id, t_params[i].task_name);
        log_str(INFO, LOG_GET_TASKS_CMD, false /* don't log to filesystem */, "per %d, prio %d, HWM %d", t_params[i].period_ms, uxTaskPriorityGet(get_task_handle(i)),
                uxTaskGetStackHighWaterMark(get_task_handle(i)));
    }
}

/**
 * @brief Prints out the status of the RTOS infrastructure.
 *
 * In particular, this will indicate whether the application is trying to create too many tasks.
 */
void print_rtos_status(void) {
    if ((rtos_errors.too_many_tasks == false) && (rtos_errors.rtos_task_create_failed == false)) {
        log_str(INFO, LOG_OBC_TASK, true, "OK");
    } else {
        if (rtos_errors.too_many_tasks) {
            log_str(ERROR, LOG_OBC_TASK, true, "Too many tasks.");
        }
        if (rtos_errors.rtos_task_create_failed) {
            log_str(ERROR, LOG_OBC_TASK, true, "Task create failed.");
        }
    }
}

/**
 * @brief 	Wrapper for xTaskCreateStatic, creates a task that has access to a period that can be
 * changed from outside the task itself.
 *
 *          If this function returns pdFALSE, there is not enough memory for FreeRTOS to create the
 * new task or MAX_NUM_TASKS needs to be increased.
 *
 * @param pxTaskCode: same as pxTaskCode parameter of xTaskCreateStatic
 * @param pcName: same as pcName parameter of xTaskCreateStatic. Cannot contain spaces.
 * @param ulStackDepth: same as ulStackDepth parameter of xTaskCreateStatic
 * @param pvParameters: same as pvParameters parameter of xTaskCreateStatic
 * @param uxPriority: same as uxPriority parameter of xTaskCreateStatic
 * @param puxStackBuffer: same as puxStackBuffer parameter of xTaskCreateStatic
 * @param pxTaskBuffer: same as pxTaskBuffer parameter of xTaskCreateStatic
 * @param period_ms: The period the task may use. Must be greater than zero.
 * @return the task handle if the creation succeeds, or NULL if it fails or a period of 0 is
 * requested.
 */
TaskHandle_t task_create_periodic_static(TaskFunction_t pxTaskCode, const char* const pcName, const uint32_t ulStackDepth, void* const pvParameters, UBaseType_t uxPriority,
                                         StackType_t* const puxStackBuffer, StaticTask_t* const pxTaskBuffer, uint32_t period_ms) {
    if (period_ms > 0) {
        return _task_create_static(pxTaskCode, pcName, ulStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer, period_ms);
    } else {
        return NULL;
    }
}

/**
 * @brief 	Wrapper for xTaskCreateStatic, adds the task to the list of tasks for watchdog to
 * monitor. This function should be used for creating all static tasks that are being monitored by
 * the watchdog.
 *
 * @param pxTaskCode: same as pxTaskCode parameter of xTaskCreateStatic
 * @param pcName: same as pcName parameter of xTaskCreateStatic. Cannot contain spaces.
 * @param ulStackDepth: same as ulStackDepth parameter of xTaskCreateStatic
 * @param pvParameters: same as pvParameters parameter of xTaskCreateStatic
 * @param uxPriority: same as uxPriority parameter of xTaskCreateStatic
 * @param puxStackBuffer: same as puxStackBuffer parameter of xTaskCreateStatic
 * @param pxTaskBuffer: same as pxTaskBuffer parameter of xTaskCreateStatic
 * @return task handle if the task could be created, otherwise NULL.
 */
TaskHandle_t task_create_static(TaskFunction_t pxTaskCode, const char* const pcName, const uint32_t ulStackDepth, void* const pvParameters, UBaseType_t uxPriority, StackType_t* const puxStackBuffer,
                                StaticTask_t* const pxTaskBuffer) {
    return _task_create_static(pxTaskCode, pcName, ulStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer, 0);
}

// ------------- Private Functions -------------

/**
 * @brief 	Wrapper for xTaskCreateStatic, adds the task to the list of tasks for watchdog to
 * monitor. This function should be used for creating all static tasks that are being monitored by
 * the watchdog.
 *
 *          If this function returns NULL, there is not enough memory for FreeRTOS to create the new
 * task or MAX_NUM_TASKS needs to be increased.
 *
 * @param pxTaskCode: same as pxTaskCode parameter of xTaskCreateStatic
 * @param pcName: same as pcName parameter of xTaskCreateStatic
 * @param ulStackDepth: same as ulStackDepth parameter of xTaskCreateStatic
 * @param pvParameters: same as pvParameters parameter of xTaskCreateStatic
 * @param uxPriority: same as uxPriority parameter of xTaskCreateStatic
 * @param puxStackBuffer: same as puxStackBuffer parameter of xTaskCreateStatic
 * @param pxTaskBuffer: same as pxTaskBuffer parameter of xTaskCreateStatic
 * @param period_ms: The period the task may use.
 * @return the the task handle if creation succeeded, otherwise NULL.
 */
static TaskHandle_t _task_create_static(TaskFunction_t pxTaskCode, const char* const pcName, const uint32_t ulStackDepth, void* const pvParameters, UBaseType_t uxPriority,
                                        StackType_t* const puxStackBuffer, StaticTask_t* const pxTaskBuffer, uint32_t period_ms) {
    if (!ok_to_create_new_task()) {
        return NULL;
    }

    uint8_t task_id = number_of_tasks(); // Task ID is zero indexed, so the first-ever task ID will
                                         // be 0 (the number of created tasks at this point)
    task_params_t* params_full = set_task_params(task_id, period_ms, pcName, pvParameters);

    if (xSemaphoreTake(xTaskArrayMutexHandle, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        task_handles[task_id] = xTaskCreateStatic(pxTaskCode, pcName, ulStackDepth, params_full, uxPriority, puxStackBuffer, pxTaskBuffer);

        if (task_handles[task_id] != NULL) {
            add_task(); // Now that we've added our task, increment num_tasks
        }

        xSemaphoreGive(xTaskArrayMutexHandle);
        return task_handles[task_id];
    }

    return NULL;
}

/**
 * @brief Sets parameters for task_id in t_params and returns a reference to the param
 *
 * @param task_id: WD task-id must be less than num_tasks
 * @param period_ms: An optional period for the task to use during delays.
 * @param task_name: Pointer to the task name.
 * @param pvParameters: a pointer to the original parameters passed to the task
 * @return a pointer to where params are stored in the t_params array
 */
static task_params_t* set_task_params(uint8_t task_id, uint32_t period_ms, const char* const task_name, void* pvParameters) {
    t_params[task_id].task_id    = task_id;
    t_params[task_id].period_ms  = period_ms;
    t_params[task_id].task_name  = task_name;
    t_params[task_id].parameters = pvParameters;
    return &t_params[task_id];
}

/**
 * @brief Checks that there is enough room in the arrays to create a new task.
 *
 * @return True if it is safe to create a new task, false otherwise.
 */
static bool ok_to_create_new_task(void) {
    if (number_of_tasks() >= MAX_NUM_TASKS) {
        rtos_errors.too_many_tasks = true;
        return false;
    }
    return true;
}

/**
 * @brief Safely increments the number of tasks in the system so we don't overflow the
 * pre-allocated storage for task information. The value returned by this function is
 * the next free task slot in the obc_rtos task information storage, or the last free slot.
 *
 * @return The next free task slot, or (MAX_NUM_TASKS - 1), which is the highest task ID allowable.
 */
static uint8_t add_task(void) {
    if (number_of_tasks() <= (MAX_NUM_TASKS - 1)) {
        return num_tasks++;
    } else {
        /*
         * We should never end up here. If we do, MAX_NUM_TASKS needs to be incremented.
         */
        return num_tasks;
    }
}
