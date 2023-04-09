/**
 * @file obc_watchdog.c
 * @brief Handler for the watchdog that allows detection of locked up tasks to prevent the watchdog
 * from being pet.
 * @author Lana
 * @date Aug 5, 2019
 */

#include "obc_watchdog.h"
#include "gio.h"
#include "logger.h"
#include "obc_hardwaredefs.h"
#include "rtos.h"
#include "obc_rtos.h"
#include "obc_task_info.h"

TaskHandle_t xWatchdogPetTaskHandle     = NULL;
static SemaphoreHandle_t watchdog_mutex = NULL;

static watchdog_task_status_t tasks_status[MAX_NUM_TASKS];

static bool tasks_statuses_valid(void);
static void pet_watchdog(void);

/**
 * @brief Creates the mutex required for the watchdog pet table. This must be done before any
 * tasks call @ref set_task_status(), and therefore before any tasks are created with the obc_rtos
 * wrappers.
 */
void wd_create_infra(void) {
    static StaticSemaphore_t watchdog_mutexBuffer;
    watchdog_mutex = xSemaphoreCreateMutexStatic(&watchdog_mutexBuffer);

    // Initialize all statuses to unknown. When tasks begin running, they will set their status to
    // alive.
    uint32_t i;
    for (i = 0; i < MAX_NUM_TASKS; i++) {
        set_task_status(i, task_unknown);
    }
}

void wd_start_task(void) {
    static StaticTask_t wd_task_buf                        = {NULL};
    static StackType_t wd_task_stack[WDPT_TASK_STACK_SIZE] = {NULL};

    xWatchdogPetTaskHandle = task_create_periodic_static(&vWatchdogPetTask, "WD", WDPT_TASK_STACK_SIZE, NULL, WATCHDOG_TASK_DEFAULT_PRIORITY, wd_task_stack, &wd_task_buf, WD_PET_PERIOD_MS);
}

/**
 * @brief Triggers a system reset by suspending the watchdog pet task.
 *
 * Suspending the WDPT ensures that the watchdog will time out, triggering a reset. Since
 * it could take up to 90 seconds for the reset to actually occur, take the watchdog mutex
 * to ensure the WDPT isn't suspended while holding this mutex. Tasks across the system require
 * the watchdog mutex, and would generate many errors if they could not access it, which is bad
 * even if we are trying to do a full system reset.
 */
void watchdog_reset(void) {
    xSemaphoreTake(watchdog_mutex, portMAX_DELAY); // Take the mutex to ensure WDPT doesn't have it
    log_signal(INFO, LOG_WATCHDOG, LOG_WATCHDOG__SUSPENDING_WDPT);
    vTaskSuspend(xWatchdogPetTaskHandle);
    xSemaphoreGive(watchdog_mutex);
}

/**
 * @brief This task is responsible for tickling the watchdog pin if all tasks have been determined
 * to be running or blocked. This is the "Watchdog Pet Task" (WDPT).
 *
 * @pre @ref wd_create_infra() has been called.
 * @warning This should be the second task that is created, after the main task.
 */
void vWatchdogPetTask(void* pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD_MS(pvParameters)));
    while (1) {
        if (tasks_statuses_valid()) {
            log_signal(DEBUG, LOG_WATCHDOG, LOG_WATCHDOG__PETTING);
            pet_watchdog();
        } else {
            log_signal(ERROR, LOG_WATCHDOG, LOG_WATCHDOG__UNKNOWN_TASK);
        }
        vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD_MS(pvParameters)));
    }
}

/**
 * @brief Setter for task status
 *
 * @pre @ref wd_create_infra() has been called and the WDPT has been started.
 *
 * @param task_id: id of the required task
 * @param status: status to be set for the task_id task
 * @return true if watchdog mutex was taken successfully, false otherwise
 */
bool set_task_status(task_id_t task_id, watchdog_task_status_t status) {
    if (xSemaphoreTake(watchdog_mutex, pdMS_TO_TICKS(WD_MUTEX_WAIT_MS)) == pdTRUE) {
        tasks_status[task_id] = status;
        xSemaphoreGive(watchdog_mutex);
        return true;
    } else {
        log_signal(ERROR, LOG_WATCHDOG, LOG_WATCHDOG__FAILED_TO_TAKE_MUTEX_IN_SET_STATUS);
        return false;
    }
}

/**
 * @brief Getter for task status
 *
 * @param task_id: id of the requried task
 * @param [out] task_status: to store the task-status struct
 * @return true if watchdog mutex was taken successfully, false otherwise
 */
bool get_task_status(task_id_t task_id, watchdog_task_status_t* task_status) {
    if (xSemaphoreTake(watchdog_mutex, pdMS_TO_TICKS(WD_MUTEX_WAIT_MS)) == pdTRUE) {
        *task_status = tasks_status[task_id];
        xSemaphoreGive(watchdog_mutex);
        return true;
    } else {
        log_signal(ERROR, LOG_WATCHDOG, LOG_WATCHDOG__FAILED_TO_TAKE_MUTEX_IN_GET_STATUS);
        return false;
    }
}

/**
 * @brief Wrapper for vTaskSuspend, tells the watchdog that the task does not need to be monitored.
 *          This function should be used for suspending all tasks that are being monitored by the
 * watchdog.
 *
 * @param xTaskToSuspend: handle to the task to suspend
 */
void task_suspend_wd(TaskHandle_t xTaskToSuspend) {
    uint32_t i;
    if (xSemaphoreTake(watchdog_mutex, pdMS_TO_TICKS(WD_MUTEX_WAIT_MS)) == pdTRUE) {
        for (i = 0; i < number_of_tasks(); i++) {
            if (get_task_handle(i) == xTaskToSuspend) {
                if (xTaskToSuspend == xTaskGetCurrentTaskHandle()) {
                    vTaskSuspend(NULL);
                } else {
                    vTaskSuspend(xTaskToSuspend);
                }
                tasks_status[i] = task_asleep;
                break;
            }
        }
        xSemaphoreGive(watchdog_mutex);
    } else {
        log_signal(ERROR, LOG_WATCHDOG, LOG_WATCHDOG__FAILED_TO_TAKE_MUTEX_IN_SUSPEND);
    }
}

/**
 * @brief 	Wrapper for vTaskResume, tells the watchdog that the task needs to be monitored.
 *			This function should be used for resuming all tasks that are being monitored by the
 *watchdog.
 *
 * @param xTaskToResume: handle to the task to resume
 */
void task_resume_wd(TaskHandle_t xTaskToResume) {
    uint32_t i;
    if (xSemaphoreTake(watchdog_mutex, pdMS_TO_TICKS(WD_MUTEX_WAIT_MS)) == pdTRUE) {
        for (i = 0; i < number_of_tasks(); i++) {
            if (get_task_handle(i) == xTaskToResume) {
                vTaskResume(xTaskToResume);
                tasks_status[i] = task_alive;
                break;
            }
        }
        xSemaphoreGive(watchdog_mutex);
    } else {
        log_signal(ERROR, LOG_WATCHDOG, LOG_WATCHDOG__FAILED_TO_TAKE_MUTEX_IN_RESUME);
    }
}

// ------------------------------- PRIVATE FUNC -------------------------------

/**
 * @brief Sends a pulse to pet the watchdog
 */
static void pet_watchdog(void) {
#if defined(PLATFORM_OBC) && !defined(PLATFORM_ALEA_V1) // TODO remove when ALEA watchdog is fixed (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/61)
    gioSetBit(OBC_WD_PORT, OBC_WD_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(WD_PET_PULSE_MS));
    gioSetBit(OBC_WD_PORT, OBC_WD_PIN, 0);
#endif
}

/**
 * @brief Checks statuses of all the tasks to ensure none of them are stuck or starving
 *
 * @return 	true if all tasks have statuses ALIVE or ASLEEP, false if one or more tasks have UNKNOWN
 * status
 */
static bool tasks_statuses_valid(void) {
    bool is_correct = true;
    uint32_t i;
    for (i = 0; i < number_of_tasks(); i++) {
        if (get_task_handle(i) != xWatchdogPetTaskHandle) {
            watchdog_task_status_t status = task_unknown;
            bool got_mutex                = get_task_status(i, &status);
            if (!got_mutex) {
                is_correct = false;
            } else if (status == task_unknown) {
                char* task_name = pcTaskGetName(get_task_handle(i));
                log_signal_with_data(ERROR, LOG_WATCHDOG, LOG_WATCHDOG__DID_NOT_CLEAR_UNKNOWN, strlen(task_name), task_name);
                is_correct = false;
            } else if (status == task_alive) {
                set_task_status(i, task_unknown);
            } else {
            }
        }
    }
    return is_correct;
}
