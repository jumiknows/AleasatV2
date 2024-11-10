/**
 * @file test_obc_settings.h
 * @brief Unit tests for the settings API.
 * @author Richard A
 * @date Apr 25, 2020
 */

#include "unity.h"
#include "obc_settings.h"
#include "FreeRTOS.h"
#include "mock_rtos_portable.h"
#include "mock_rtos_portmacro.h"
#include "mock_mpu_prototypes.h"
#include "mock_logger.h"
#include "mock_nvct.h"

// The tests
void test_invalid_name(void);
void test_setting_mutability(void);
void test_setting_values(void);

/**
 * @brief Tests that setting/getting invalid setting names returns the correct error.
 */
void test_invalid_name(void) {
    uint32_t result_uint = 0;
    TEST_ASSERT_EQUAL_UINT32(INVALID_SETTING_NAME, get_uint32_from_internal("does_not_exist", &result_uint));
    TEST_ASSERT_EQUAL_UINT32(INVALID_SETTING_NAME, set_uint32_from_command("does_not_exist", 123));
    TEST_ASSERT_EQUAL_UINT32(result_uint, 0); // Ensure result is unchanged since the settings don't exist.
}

/**
 * @brief Tests that setting mutability is respected by the set_uint32() API.
 */
void test_setting_mutability(void) {
    // Setup: initialize the settings and NVCT as we do at startup.
    // This is required for sets and gets to work, because they need
    // to know the revision count setting's index, which is determined
    // at startup.
    MPU_xQueueCreateMutexStatic_IgnoreAndReturn(pdPASS);
    init_nvct_IgnoreAndReturn(NVCT_SUCCESS);
    MPU_xQueueTakeMutexRecursive_IgnoreAndReturn(pdPASS);
    MPU_xQueueGiveMutexRecursive_IgnoreAndReturn(pdPASS);
    get_nvct_value_IgnoreAndReturn(NVCT_SUCCESS);
    update_settings_from_nvct();

    uint32_t result_uint = 0;

    ///////////////////////////
    // SECTION 1: When allowed to change immutable (check_mutable == false) settings,
    // ensure that both setting types can be set and that values update.
    ///////////////////////////

    // Set up expectations for NVCT API calls - ignore the index but check the other parameters.
    // Test a mutable setting with the _internal API.
    set_nvct_value_ExpectAndReturn(true, 0 /* ignored */, 123, NVCT_SUCCESS); // The NVCT set call for the setting.
    set_nvct_value_IgnoreArg_index();
    set_nvct_value_ExpectAndReturn(false, 0 /* ignored */, 1, NVCT_SUCCESS); // The NVCT set call for the revision count.
    set_nvct_value_IgnoreArg_index();
    TEST_ASSERT_EQUAL_UINT32(SETTING_OK, set_uint32_from_internal("test_mutable", 123));

    // Test an immutable setting with the _internal API.
    set_nvct_value_ExpectAndReturn(false, 0 /* ignored */, 456, NVCT_SUCCESS); // The NVCT set call for the setting.
    set_nvct_value_IgnoreArg_index();
    set_nvct_value_ExpectAndReturn(false, 0 /* ignored */, 2, NVCT_SUCCESS); // The NVCT set for the revision count.
    set_nvct_value_IgnoreArg_index();
    TEST_ASSERT_EQUAL_UINT32(SETTING_OK, set_uint32_from_internal("test_immutable", 456));

    // Read back the settings and ensure they were both changed.
    get_nvct_value_IgnoreAndReturn(NVCT_SUCCESS);
    TEST_ASSERT_EQUAL_UINT32(SETTING_OK, get_uint32_from_internal("test_mutable", &result_uint));
    TEST_ASSERT_EQUAL_UINT32(123, result_uint);
    TEST_ASSERT_EQUAL_UINT32(SETTING_OK, get_uint32_from_internal("test_immutable", &result_uint));
    TEST_ASSERT_EQUAL_UINT32(456, result_uint);

    ///////////////////////////
    // SECTION 2: Check that mutable settings can still be set from command.
    ///////////////////////////
    set_nvct_value_ExpectAndReturn(true, 0 /* ignored */, 789, NVCT_SUCCESS); // The NVCT set call for the setting.
    set_nvct_value_IgnoreArg_index();
    set_nvct_value_ExpectAndReturn(false, 0 /* ignored */, 3, NVCT_SUCCESS); // The NVCT set for the revision count.
    set_nvct_value_IgnoreArg_index();
    TEST_ASSERT_EQUAL_UINT32(SETTING_OK, set_uint32_from_command("test_mutable", 789));
    get_nvct_value_IgnoreAndReturn(NVCT_SUCCESS);
    setting_nvct_t nvct_status;
    TEST_ASSERT_EQUAL_UINT32(SETTING_OK, get_uint32_from_command("test_mutable", &result_uint, &nvct_status));
    TEST_ASSERT_EQUAL_UINT32(789, result_uint);

    ///////////////////////////
    // SECTION 3: Check that immutable settings can't be set from command.
    ///////////////////////////
    TEST_ASSERT_EQUAL_UINT32(SETTING_IMMUTABLE, set_uint32_from_command("test_immutable", 123));
    TEST_ASSERT_EQUAL_UINT32(SETTING_OK, get_uint32_from_command("test_immutable", &result_uint, &nvct_status));
    TEST_ASSERT_EQUAL_UINT32(456, result_uint);
}

/**
 * @brief Tests that invalid setting values are rejected.
 */
void test_setting_values(void) {
    // Ceedling doesn't reset changed globals between tests. So get the setting's value
    // as a starting point. It won't necessarily be the default if another test changed it.
    uint32_t initial_result = 0;
    TEST_ASSERT_EQUAL_UINT32(SETTING_OK, get_uint32_from_internal("test_mutable", &initial_result));

    // Test that a value too large is rejected. Max is 123456.
    uint32_t after_change = 0;
    setting_nvct_t nvct_status;
    TEST_ASSERT_EQUAL_UINT32(INVALID_SETTING_VALUE, set_uint32_from_command("test_mutable", 123457));
    TEST_ASSERT_EQUAL_UINT32(SETTING_OK, get_uint32_from_command("test_mutable", &after_change, &nvct_status));
    TEST_ASSERT_EQUAL_UINT32(initial_result, after_change);

    // Test that a value too small is rejected. Min is 100.
    TEST_ASSERT_EQUAL_UINT32(INVALID_SETTING_VALUE, set_uint32_from_command("test_mutable", 1));
    TEST_ASSERT_EQUAL_UINT32(SETTING_OK, get_uint32_from_command("test_mutable", &after_change, &nvct_status));
    TEST_ASSERT_EQUAL_UINT32(initial_result, after_change);
}
