#include "unity.h"

#include "FreeRTOS.h"
#include "mock_rtos_portable.h"
#include "mock_rtos_portmacro.h"
#include "mock_mpu_prototypes.h"
#include "mock_logger.h"

#include "obc_rtos.h"

/**
 * @brief Tests for the OBC RTOS wrapper non-static functions.
 */

// Tests
void test_task_create_periodic(void);
void test_zero_period_set(void);
void test_zero_period_create(void);
void test_period_change(void);
void test_period_change_failure(void);
void test_period_invalid_id(void);
void test_task_create_failure(void);
void test_task_id_valid(void);
void test_create_too_many_tasks(void);
void test_handle_from_id(void);
void test_id_from_name(void);

// Helpers and misc.
void task_mock(void* pvParameters);

BaseType_t freertos_task_create(TaskFunction_t pvTaskCode, const char* const pcName, uint16_t usStackDepth, void* pvParameters, UBaseType_t uxPriority, TaskHandle_t* pxCreatedTask, int numCalls);

void setUp(void) {
    // Required by Ceedling.
}

/**
 * @brief Runs after each test function executes.
 *
 * Call a function that resets the OBC RTOS infrastructure
 * by resetting the number of tasks created and zeroing out
 * all arrays.
 */
void tearDown(void) {
    unit_test_reset_obc_rtos();
}

/**
 * @brief A mock task function that does nothing.
 */
void task_mock(void* pvParameters) {
}

/**
 * @brief Tests the task_create_periodic API.
 */
void test_task_create_periodic(void) {
    TaskHandle_t handle = NULL;

    // Task create ends up calling the MPU task create and queue send/receive, so expect that it
    // will be called and mock it.
    MPU_xTaskCreate_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    // Create a task
    TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "test_task", 128, NULL, 1, &handle, 200));
}

/**
 * @brief Tests that setting a periodic task's period to 0 fails.
 */
void test_zero_period_set(void) {
    // Mock xTaskCreate and create a task to test with
    TaskHandle_t handle = NULL;
    MPU_xTaskCreate_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "test_task", 128, NULL, 1, &handle, 200));
    TEST_ASSERT_EQUAL_INT32(INVALID_PERIOD, set_task_period(0, 0));
}

/**
 * @brief Tests that creating a periodic task with a period of 0 fails.
 */
void test_zero_period_create(void) {
    TaskHandle_t handle = NULL;
    MPU_xTaskCreate_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    TEST_ASSERT_FALSE(task_create_periodic(&task_mock, "test_task", 128, NULL, 1, &handle, 0));
    TEST_ASSERT_EQUAL_UINT8(0, number_of_tasks());
}

/**
 * @brief Tests that changing task period works for periodic tasks,
 * and fails for non-periodic tasks.
 */
void test_period_change(void) {
    TaskHandle_t handle_periodic    = NULL;
    TaskHandle_t handle_nonperiodic = NULL;

    // Mock the FreeRTOS functions
    MPU_xTaskCreate_IgnoreAndReturn(pdPASS);
    MPU_xTaskAbortDelay_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    // Make periodic and non-periodic tasks
    TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "periodic", 128, NULL, 1, &handle_periodic, 200));
    TEST_ASSERT_TRUE(task_create(&task_mock, "nonperiodic", 128, NULL, 1, &handle_nonperiodic));
    TEST_ASSERT_EQUAL_UINT8(2, number_of_tasks());

    // Check that we can change the period of the periodic task
    uint8_t periodic_id  = 0; // 0 because it was the first task created
    task_period_t period = 0;
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_period_from_id(periodic_id, &period));
    TEST_ASSERT_EQUAL_UINT32(200, period);
    log_str_Ignore();
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, set_task_period(periodic_id, 100));
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_period_from_id(periodic_id, &period));
    TEST_ASSERT_EQUAL_UINT32(100, period);

    // Check that setting the period of a non-periodic task fails
    uint8_t nonperiodic_id = 1; // 1 because it was the second task created
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_period_from_id(nonperiodic_id, &period));
    TEST_ASSERT_EQUAL_UINT32(0, period);
    TEST_ASSERT_EQUAL_INT32(INVALID_PERIOD, set_task_period(nonperiodic_id, 100));
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_period_from_id(nonperiodic_id, &period)); // Ensure the period is indeed zero.
    TEST_ASSERT_EQUAL_UINT32(0, period);
}

/**
 * @brief Tests that when the task period change fails due to the task running,
 * the task period is not actually changed.
 *
 * 	The xTaskAbortDelay FreeRTOS function is used to disregard any existing task delays,
 * 	which allows the task to run again and take on its new delay period. This function can
 * 	fail, depending on the state of the task. When it fails, we intend to leave the old task
 * 	delay in place, and this test ensures this actually happens.
 */
void test_period_change_failure(void) {
    TaskHandle_t handle_periodic    = NULL;
    TaskHandle_t handle_nonperiodic = NULL;

    MPU_xTaskCreate_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    // Make periodic and non-periodic tasks
    TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "periodic", 128, NULL, 1, &handle_periodic, 200));
    TEST_ASSERT_TRUE(task_create(&task_mock, "nonperiodic", 128, NULL, 1, &handle_nonperiodic));

    // Check that if abortDelay fails, the period is not changed
    uint8_t periodic_id  = 0; // 0 because it was the first task created
    task_period_t period = 0;
    MPU_xTaskAbortDelay_IgnoreAndReturn(pdFAIL); // Mock xTaskAbortDelay that always fails
    log_str_Ignore();
    TEST_ASSERT_EQUAL_INT32(TASK_NOT_BLOCKED, set_task_period(periodic_id, 100));
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_period_from_id(periodic_id, &period));
    TEST_ASSERT_EQUAL_UINT32(200, period); // Ensure the original period is unchanged

    // Check that setting the period of a non-periodic task fails
    uint8_t nonperiodic_id = 1; // 1 because it was the second task created
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_period_from_id(nonperiodic_id, &period));
    TEST_ASSERT_EQUAL_UINT32(0, period);
    TEST_ASSERT_EQUAL_INT32(INVALID_PERIOD, set_task_period(nonperiodic_id, 100));
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_period_from_id(nonperiodic_id, &period));
    TEST_ASSERT_EQUAL_UINT32(0, period); // Ensure the period is indeed zero.
}

/**
 * @brief Tests that setting and getting the period of an invalid ID fails.
 */
void test_period_invalid_id(void) {
    // Mock xTaskCreate and create a task to test with
    TaskHandle_t handle = NULL;
    MPU_xTaskCreate_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);
    TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "test_task", 128, NULL, 1, &handle, 200));
    TEST_ASSERT_EQUAL_INT32(INVALID_ID, set_task_period(1, 100));

    // Try to get a period of an ID that doesn't exist
    task_period_t period = 0;
    log_str_Ignore();
    TEST_ASSERT_EQUAL_INT32(INVALID_ID, get_task_period_from_id(1, &period));
}

/**
 * @brief Tests that when a task create fails, the number of tasks is not incremented.
 */
void test_task_create_failure(void) {
    TaskHandle_t handle_1 = NULL;
    TaskHandle_t handle_2 = NULL;

    // Create a task and allow it to succeed
    MPU_xTaskCreate_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);
    TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "test_task_success", 128, NULL, 1, &handle_1, 200));
    TEST_ASSERT_EQUAL_UINT8(1, number_of_tasks()); // Should only have 1 task at this point

    // Create a task and make it fail
    MPU_xTaskCreate_IgnoreAndReturn(pdFAIL);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);
    TEST_ASSERT_FALSE(task_create_periodic(&task_mock, "test_task_fail", 128, NULL, 1, &handle_2, 200));
    TEST_ASSERT_EQUAL_UINT8(1, number_of_tasks()); // Should still only have 1 task

    // Try again, this time the task create should succeed, and we should have two tasks
    MPU_xTaskCreate_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);
    TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "test_task_success_2", 128, NULL, 1, &handle_2, 200));
    TEST_ASSERT_EQUAL_UINT8(2, number_of_tasks());
}

/**
 * @brief Tests that the task_id_valid function operates correctly.
 */
void test_task_id_valid(void) {
    // We haven't created any tasks, so all task IDs should be invalid.
    TEST_ASSERT_FALSE(task_id_valid(0));
    TEST_ASSERT_FALSE(task_id_valid(1));

    // Create a task and check that ID validity is correct.
    TaskHandle_t handle_1 = NULL;
    MPU_xTaskCreate_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);
    TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "test_task", 128, NULL, 1, &handle_1, 200));
    TEST_ASSERT_TRUE(task_id_valid(0));
    TEST_ASSERT_FALSE(task_id_valid(1));
    TEST_ASSERT_FALSE(task_id_valid(123));
}

/**
 * @brief Verify that it is impossible to create too many tasks.
 */
void test_create_too_many_tasks(void) {
    TaskHandle_t handle_1 = NULL;

    for (uint8_t i = 0; i < (MAX_NUM_TASKS + 10); i++) {
        MPU_xTaskCreate_IgnoreAndReturn(pdPASS); // Mock xTaskCreate
        MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
        MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

        uint8_t delay = i + 1; // Note: the delay given to each task is the ID number + 1 so that we
                               // can check it later.
        if (i < MAX_NUM_TASKS) {
            // Below the max number of tasks, creation should succeed.
            TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "test_task", 128, NULL, 1, &handle_1, delay));
        } else {
            // Task create periodic should fail once we have created MAX_NUM_TASKS.
            TEST_ASSERT_FALSE(task_create_periodic(&task_mock, "test_task", 128, NULL, 1, &handle_1, delay));
        }
    }

    // Check that the valid ID check is correct.
    TEST_ASSERT_EQUAL_UINT8(MAX_NUM_TASKS, number_of_tasks());
    TEST_ASSERT_TRUE(task_id_valid(MAX_NUM_TASKS - 1));
    TEST_ASSERT_FALSE(task_id_valid(MAX_NUM_TASKS));

    // The delay given was the task ID + 1, verify that this is true to ensure everything still
    // lines up.
    task_period_t period = 0;
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_period_from_id(0, &period));
    TEST_ASSERT_EQUAL_UINT32(0 + 1, period);
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_period_from_id(2, &period));
    TEST_ASSERT_EQUAL_UINT32(2 + 1, period);
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_period_from_id((MAX_NUM_TASKS - 1), &period));
    TEST_ASSERT_EQUAL_UINT32((MAX_NUM_TASKS - 1) + 1, period);
}

/**
 * @brief Tests that getting task handle from ID works.
 */
void test_handle_from_id(void) {
    TaskHandle_t handle_0 = NULL;
    TaskHandle_t handle_1 = NULL;

    MPU_xTaskCreate_StubWithCallback(&freertos_task_create); // Call the freertos_task_create mock in place of xTaskCreate
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    // Create some tasks
    TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "test_task_0", 128, NULL, 1, &handle_0, 200));
    TEST_ASSERT_TRUE(task_create(&task_mock, "test_task_1", 128, NULL, 1, &handle_1));

    TEST_ASSERT_NOT_NULL(handle_0);
    TEST_ASSERT_NOT_NULL(handle_1);

    TEST_ASSERT_EQUAL_PTR(handle_0, get_task_handle(0));
    TEST_ASSERT_EQUAL_PTR(handle_1, get_task_handle(1));
}

/**
 * @brief Tests that the task ID can be recovered from the task name.
 */
void test_id_from_name(void) {
    TaskHandle_t handle_0 = NULL;
    TaskHandle_t handle_1 = NULL;
    TaskHandle_t handle_2 = NULL;

    MPU_xTaskCreate_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    // Create some tasks successfully
    TEST_ASSERT_TRUE(task_create_periodic(&task_mock, "test_task_0", 128, NULL, 1, &handle_0, 200));
    TEST_ASSERT_TRUE(task_create(&task_mock, "test_task_1", 128, NULL, 1, &handle_1));

    // Fail to create a task
    MPU_xTaskCreate_IgnoreAndReturn(pdFAIL);
    TEST_ASSERT_FALSE(task_create_periodic(&task_mock, "test_task_2", 128, NULL, 1, &handle_2, 200));

    // Check that we have the correct number of tasks
    TEST_ASSERT_EQUAL_UINT8(2, number_of_tasks());

    // Test that we can recover the IDs successfully
    uint8_t id = 100;
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_id_from_name("test_task_0", &id));
    TEST_ASSERT_EQUAL_UINT8(0, id);
    TEST_ASSERT_EQUAL_INT32(OBC_RTOS_OK, get_task_id_from_name("test_task_1", &id));
    TEST_ASSERT_EQUAL_UINT8(1, id);

    // Check that cases where there is no valid ID fail
    TEST_ASSERT_EQUAL_INT32(INVALID_NAME, get_task_id_from_name("test_task_2", &id));
    TEST_ASSERT_EQUAL_UINT8(1, id); // ID should be unchanged from the previous successful call
    TEST_ASSERT_EQUAL_INT32(INVALID_NAME, get_task_id_from_name("nonexistent_task", &id));
    TEST_ASSERT_EQUAL_UINT8(1, id); // ID should be unchanged from the previous successful call
}

/**
 * @brief Mocks FreeRTOS' xTaskCreate function, and alternates what the
 * @ref pxCreatedTask parameter points towards.
 */
BaseType_t freertos_task_create(TaskFunction_t pvTaskCode, const char* const pcName, uint16_t usStackDepth, void* pvParameters, UBaseType_t uxPriority, TaskHandle_t* pxCreatedTask, int numCalls) {
    static uint32_t pointed_0 = 0; // Give ourselves something to point towards, as if FreeRTOS malloc'd it.
    static uint32_t pointed_1 = 0;

    // Flip between which value pxCreatedTask points towards
    if (numCalls % 2 == 0) {
        *pxCreatedTask = (void*)&pointed_0;
    } else {
        *pxCreatedTask = (void*)&pointed_1;
    }

    return pdPASS;
}
