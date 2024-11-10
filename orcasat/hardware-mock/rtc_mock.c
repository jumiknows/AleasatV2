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

#include "FreeRTOS.h"
#include "rtc_mock.h"
#include "obc_task_info.h"
#include "rtos_task.h"
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
    rtc_set_current_time_mock(&orca_time_init);

    BaseType_t err = xTaskCreate(&xStepMockRealTimeTask, "rtc_mock", configMINIMAL_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, NULL);

    if (err != pdPASS) {
        log_str(ERROR, RTC_LOG, true, "RTC mock init failed.");
    } else {
        log_str(INFO, RTC_LOG, false, "RTC mock init OK");
    }
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
