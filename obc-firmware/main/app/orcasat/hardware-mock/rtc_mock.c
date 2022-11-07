/**
 * @file rtc_mock.c
 * @brief A mocked RTC that uses a FreeRTOS task to increment the time.
 * @author Alireza A, Richard A
 * @date April 19, 2019
 *
 * On LaunchPads without access to a hardware RTC, we enable the mocked RTC.
 * This task runs silently in the background and increments the time, and is
 * accessed through the obc_rtc API as if it were a dedicated RTC.
 */

#include "rtc_mock.h"
#include "obc_task_info.h"
#include "rtos.h"
#include "obc_time.h"
#include "logger.h"
#include <string.h>

real_time_t mock_real_time;

static void xStepMockRealTimeTask(void* pvParameters);

/**
 * @brief Initializes the RTC mock task.
 *
 * The RTC mock task increments @ref mock_real_time in the same pattern as the RTC hardware does.
 */
void rtc_init_mock(void) {
    static StaticTask_t step_mock_rtc_task_buf;
    static StackType_t step_mock_rtc_task_stack[configMINIMAL_STACK_SIZE];

    rtc_set_current_time_mock(&orca_time_init);

    xTaskCreateStatic(&xStepMockRealTimeTask, "rtc_mock", configMINIMAL_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, step_mock_rtc_task_stack, &step_mock_rtc_task_buf);
}

/**
 * @brief The task, calls the increment function once per second.
 */
static void xStepMockRealTimeTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
        increment_real_time(&mock_real_time);
    }
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
