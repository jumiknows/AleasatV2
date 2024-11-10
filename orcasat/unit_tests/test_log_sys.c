/*
 * test_log_sys.c
 *
 *  Created on: Jun 9, 2019
 *      Author: Richard
 */

#include "logger.h"
#include "obc_watchdog.h"
#include "FreeRTOS.h"
#include "rtos_task.h"
#include "unit_tests.h"

TaskHandle_t xLogSysTaskHandle = NULL;

/* Logging system test task
 * 	- for use in Houston bringup
 * 	- make valid error/info/debug prints
 * 	- hit some payload length edge cases
 */
void vLogSysTestTask(void* pvParameters) {
    uint32_t wd_task_id = WD_TASK_ID(pvParameters);
    uint32_t test_var   = 234;
    set_task_status(wd_task_id, task_asleep);
    while (1) {
        // Test simple error/info/debug prints
        log_str(DEBUG, 0, false, "Debug message %d!", test_var);
        test_var++;
        vTaskDelay(pdMS_TO_TICKS(4000));
        log_str(INFO, 1, false, "Info message! %d", test_var);
        test_var++;
        vTaskDelay(pdMS_TO_TICKS(4000));
        log_str(ERROR, 3, true, "Error message! %d", test_var);
        vTaskDelay(pdMS_TO_TICKS(4000));
        test_var++;

        // ---- Test the valid limits ----
        // Vsnprintf'd payload at maximum size
        test_var = 12345;
        log_str(INFO, 5, false, "Vsnprintf payload ok----- %d", test_var);
        vTaskDelay(pdMS_TO_TICKS(4000));
        test_var++;

        // Maximum ID
        log_str(INFO, 1023, false, "Maximum ID");
        vTaskDelay(pdMS_TO_TICKS(4000));

        // ---- Error cases ----
        // Test huge payload
        log_str(ERROR, 6, true, "0123456789012345678901234567890123456789");
        vTaskDelay(pdMS_TO_TICKS(4000));

        // Test huge payload where vsnprintf causes the overflow:
        test_var = 12345;
        log_str(DEBUG, 11, false, "Vsnprintf payload overflow %d",
                test_var); // Should see: Vsnprintf payload overflow 1234 after the error is logged
        vTaskDelay(pdMS_TO_TICKS(4000));

        // Test message ID that's too big
        log_str(INFO, 1025, false,
                "Should be ID 1"); // Should see: ID wrap to 1 and not mess with the other fields
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}
