/**
 * @file rtc_mock.c
 * @brief A mocked RTC that uses a FreeRTOS task to increment the time
 *
 * On Launchpads without access to a hardware RTC, we enable the mocked RTC.
 * This task runs silently in the background and increments the time, and is
 * accessed through the obc_rtc API as if it were a dedicated RTC.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "rtc_mock.h"
#include "obc_task_info.h"
#include "rtos.h"
#include "obc_time.h"
#include "logger.h"
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define ALARM_MUTEX_TIMEOUT_MS 500U

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    uint32_t timestamp;
    rtc_alarm_cb_t callback;
    void *callback_arg;
} alarm_info_t;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void xStepMockRealTimeTask(void* pvParameters);
static void handle_alarm(void);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static real_time_t mock_real_time;

static SemaphoreHandle_t alarm_mutex = NULL;

static alarm_info_t next_alarm = {
    .timestamp = 0,
    .callback = NULL,
    .callback_arg = NULL,
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the RTC mock task.
 *
 * The RTC mock task increments @ref mock_real_time in the same pattern as the RTC hardware does.
 */
void rtc_init_mock(void) {
    static StaticTask_t step_mock_rtc_task_buf = { 0 };
    static StackType_t step_mock_rtc_task_stack[RTC_MOCK_STACK_SIZE];

    static StaticSemaphore_t alarm_mutex_buf = { 0 };

    rtc_set_current_time_mock(&orca_time_init);

    alarm_mutex = xSemaphoreCreateMutexStatic(&alarm_mutex_buf);

    xTaskCreateStatic(&xStepMockRealTimeTask, "rtc_mock", RTC_MOCK_STACK_SIZE, NULL, RTC_MOCK_TASK_PRIORITY, step_mock_rtc_task_stack, &step_mock_rtc_task_buf);
}

/**
 * @brief Copies the current mock real time into @ref curr_time.
 *
 * @param curr_time[out] The current time.
 * @return RTC_NO_ERR.
 */
rtc_err_t rtc_get_current_time_mock(real_time_t* curr_time) {
    memcpy(curr_time, &mock_real_time, sizeof(real_time_t));
    return RTC_NO_ERR;
}

/**
 * @brief Sets the current mock time.
 *
 * @param[in] curr_time The time to set.
 * @return RTC_NO_ERR
 */
rtc_err_t rtc_set_current_time_mock(const real_time_t* curr_time) {
    memcpy(&mock_real_time, curr_time, sizeof(real_time_t));
    return RTC_NO_ERR;
}

/**
 * @brief Gets the startup status.
 * @return RTC_NO_ERR.
 */
rtc_err_t rtc_get_startup_status_mock(void) {
    /* could return different status values here randomly */
    return RTC_NO_ERR;
}

rtc_err_t rtc_set_alarm_mock(uint32_t timestamp, rtc_alarm_cb_t cb, void *cb_arg) {
    if (xSemaphoreTake(alarm_mutex, pdMS_TO_TICKS(ALARM_MUTEX_TIMEOUT_MS)) == pdFALSE) {
        return RTC_MUTEX_GRAB_ERR;
    }

    next_alarm.timestamp    = timestamp;
    next_alarm.callback     = cb;
    next_alarm.callback_arg = cb_arg;

    xSemaphoreGive(alarm_mutex);

    return RTC_NO_ERR;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief The task, calls the increment function once per second.
 */
static void xStepMockRealTimeTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
        increment_real_time(&mock_real_time);
        handle_alarm();
    }
}

/**
 * @brief Check if an alarm is set and call the alarm callback if the current
 * time matches the alarm time.
 */
static void handle_alarm(void) {
    if (xSemaphoreTake(alarm_mutex, pdMS_TO_TICKS(ALARM_MUTEX_TIMEOUT_MS)) == pdFALSE) {
        return;
    }

    if ((next_alarm.callback != NULL) && (next_alarm.timestamp > 0)) {
        if (mock_real_time.epoch == next_alarm.timestamp) {
            next_alarm.callback(next_alarm.callback_arg);

            // Clear alarm
            next_alarm.timestamp = 0;
            next_alarm.callback = NULL;
            next_alarm.callback_arg = NULL;
        }
    }

    xSemaphoreGive(alarm_mutex);
}
