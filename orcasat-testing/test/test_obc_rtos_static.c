#include "unity.h"

#include "FreeRTOS.h"
#include "mock_rtos_portable.h"
#include "mock_rtos_portmacro.h"
#include "mock_mpu_prototypes.h"
#include "mock_logger.h"

#include "obc_rtos.h"

/**
 * @brief Tests for the OBC RTOS wrapper static functions.
 */

// Tests
void test_task_create_periodic(void);
void test_zero_period_set(void);
void test_zero_period_create(void);
void test_period_change(void);
void test_period_change_failure(void);
void test_task_create_failure(void);
void test_period_invalid_id(void);
void test_task_id_valid(void);
void test_create_too_many_tasks(void);
void test_handle_from_id(void);
void test_id_from_name(void);

// Buffers for static tasks
StaticTask_t task_buffer_0;
StaticTask_t task_buffer_1;
StackType_t task_stack_0[configMINIMAL_STACK_SIZE];
StackType_t task_stack_1[configMINIMAL_STACK_SIZE];

// Helpers and misc.
void task_mock(void* pvParameters);

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
    // Task create ends up calling the xTaskCreateStatic, so expect that it will be called and mock
    // it to return a valid pointer.
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    // Create a task statically
    TEST_ASSERT_NOT_NULL(task_create_periodic_static(&task_mock, "test_task", 128, NULL, 1, task_stack_0, &task_buffer_0, 200));
}

/**
 * @brief Tests that setting a periodic task's period to 0 fails.
 */
void test_zero_period_set(void) {
    // Task create ends up calling the xTaskCreateStatic, so expect that it will be called and mock
    // it to return a valid pointer.
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    TEST_ASSERT_NOT_NULL(task_create_periodic_static(&task_mock, "test_task", 128, NULL, 1, task_stack_0, &task_buffer_0, 200));
    TEST_ASSERT_EQUAL_INT32(INVALID_PERIOD, set_task_period(0, 0));
}

/**
 * @brief Tests that creating a periodic task with a period of 0 fails.
 */
void test_zero_period_create(void) {
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    TEST_ASSERT_NULL(task_create_periodic_static(&task_mock, "test_task", 128, NULL, 1, task_stack_0, &task_buffer_0, 0));
    TEST_ASSERT_EQUAL_UINT8(0, number_of_tasks());
}

/**
 * @brief Tests that changing task period works for periodic tasks,
 * and fails for non-periodic tasks.
 */
void test_period_change(void) {
    // Make periodic and non-periodic tasks
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    TEST_ASSERT_NOT_NULL(task_create_periodic_static(&task_mock, "periodic", 128, NULL, 1, task_stack_0, &task_buffer_0, 200));
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)&task_buffer_1);
    TEST_ASSERT_NOT_NULL(task_create_static(&task_mock, "nonperiodic", 128, NULL, 1, task_stack_1, &task_buffer_1));
    TEST_ASSERT_EQUAL_UINT8(2, number_of_tasks());

    // Mock abortDelay that succeeds
    MPU_xTaskAbortDelay_IgnoreAndReturn(pdPASS);

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
    // Make periodic and non-periodic tasks
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    TEST_ASSERT_NOT_NULL(task_create_periodic_static(&task_mock, "periodic", 128, NULL, 1, task_stack_0, &task_buffer_0, 200));
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)&task_buffer_1);
    TEST_ASSERT_NOT_NULL(task_create_static(&task_mock, "nonperiodic", 128, NULL, 1, task_stack_1, &task_buffer_1));

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
 * @brief Tests that setting and getting the period of an invalid ID fails
 */
void test_period_invalid_id(void) {
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    TEST_ASSERT_NOT_NULL(task_create_periodic_static(&task_mock, "periodic", 128, NULL, 1, task_stack_0, &task_buffer_0, 200));

    // Try to change the period of an ID that doesn't exist
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
    // Create a task and allow it to succeed
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    TEST_ASSERT_NOT_NULL(task_create_periodic_static(&task_mock, "periodic", 128, NULL, 1, task_stack_0, &task_buffer_0, 200));
    TEST_ASSERT_EQUAL_UINT8(1, number_of_tasks()); // Should only have 1 task at this point

    // Create a task and make it fail
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)NULL); // Return NULL, which is done if task creation fails for some reason
    TEST_ASSERT_NULL(task_create_static(&task_mock, "nonperiodic", 128, NULL, 1, task_stack_1, &task_buffer_1));
    TEST_ASSERT_EQUAL_UINT8(1, number_of_tasks()); // Should still only have 1 task

    // Try again, this time the task create should succeed, and we should have two tasks
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)&task_buffer_1);
    TEST_ASSERT_NOT_NULL(task_create_static(&task_mock, "nonperiodic", 128, NULL, 1, task_stack_1, &task_buffer_1));
    TEST_ASSERT_EQUAL_UINT8(2, number_of_tasks());
}

/**
 * @brief Tests that the task_id_valid function operates correctly.
 */
void test_task_id_valid(void) {
    // We haven't created any tasks, so all task IDs should be invalid.
    TEST_ASSERT_FALSE(task_id_valid(0));
    TEST_ASSERT_FALSE(task_id_valid(1));
    TEST_ASSERT_FALSE(task_id_valid(MAX_NUM_TASKS));

    // Create a task and check that ID validity is correct.
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

    TEST_ASSERT_NOT_NULL(task_create_periodic_static(&task_mock, "periodic", 128, NULL, 1, task_stack_0, &task_buffer_0, 200));
    TEST_ASSERT_TRUE(task_id_valid(0));
    TEST_ASSERT_FALSE(task_id_valid(1));
    TEST_ASSERT_FALSE(task_id_valid(123));
}

/**
 * @brief Verify that it is impossible to create too many tasks.
 */
void test_create_too_many_tasks(void) {
    for (uint8_t i = 0; i < (MAX_NUM_TASKS + 10); i++) {
        MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
        MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
        MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);

        uint8_t delay = i + 1; // Note: the delay given to each task is the ID number + 1 so that we
                               // can check it later.
        if (i < MAX_NUM_TASKS) {
            // Below the max number of tasks, creation should succeed.
            TEST_ASSERT_NOT_NULL(task_create_periodic_static(&task_mock, "periodic", 128, NULL, 1, task_stack_0, &task_buffer_0, delay));
        } else {
            // Task create periodic should fail once we have created MAX_NUM_TASKS.
            TEST_ASSERT_NULL(task_create_periodic_static(&task_mock, "periodic", 128, NULL, 1, task_stack_0, &task_buffer_0, delay));
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
    // Create some tasks
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);
    TaskHandle_t task_0 = task_create_periodic_static(&task_mock, "task_0", 128, NULL, 1, task_stack_0, &task_buffer_0, 200);
    TEST_ASSERT_NOT_NULL(task_0);

    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)&task_buffer_1);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);
    TaskHandle_t task_1 = task_create_periodic_static(&task_mock, "task_1", 128, NULL, 1, task_stack_1, &task_buffer_1, 200);
    TEST_ASSERT_NOT_NULL(task_1);
    TEST_ASSERT_EQUAL_UINT8(2, number_of_tasks());

    // Check that the handle can be recovered
    TEST_ASSERT_EQUAL_PTR(task_0, get_task_handle(0));
    TEST_ASSERT_EQUAL_PTR(task_1, get_task_handle(1));
}

/**
 * @brief Tests that the task ID can be recovered from the task name.
 */
void test_id_from_name(void) {
    // Create some tasks successfully
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xDEADBEEF);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);
    TEST_ASSERT_NOT_NULL(task_create_periodic_static(&task_mock, "test_task_0", 128, NULL, 1, task_stack_0, &task_buffer_0, 200));
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)0xFACECAFE);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);
    TEST_ASSERT_NOT_NULL(task_create_static(&task_mock, "test_task_1", 128, NULL, 1, task_stack_1, &task_buffer_1));

    // Fail to create a task
    MPU_xTaskCreateStatic_IgnoreAndReturn((void*)NULL);
    MPU_xQueueGenericSend_IgnoreAndReturn(pdPASS);
    MPU_xQueueGenericReceive_IgnoreAndReturn(pdPASS);
    TEST_ASSERT_NULL(task_create_periodic_static(&task_mock, "test_task_2", 128, NULL, 1, task_stack_0, &task_buffer_0, 200));

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
