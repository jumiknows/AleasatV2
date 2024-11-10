/**
 * @file test_settings.c
 * @brief Unit tests for the settings table. These tests verify that the configured
 * settings match all of the rules that firmware expects.
 * @author Richard A
 * @date September 24, 2020
 *
 * @note Since the settings table is defined in a header, you need to run ceedling clobber
 * before changes to it are picked up by Ceedling.
 */

#include "unity.h"
#include "settings.h"
#include <string.h>

// The tests
void test_revision_setting(void);
void test_settings_table_values(void);
void test_settings_table_callbacks(void);
void test_settings_nvct_config(void);

/**
 * @brief Tests that the revision count setting exists.
 */
void test_revision_setting(void) {
    bool revision_exists = false;
    for (uint32_t idx = 0; idx < num_settings; idx++) {
        if (strcmp("revision", settings[idx].name) == 0) {
            revision_exists = true;
            break;
        }
    }
    TEST_ASSERT_MESSAGE(revision_exists, "Setting named 'revision' does not exist!");
}

/**
 * @brief Tests that the default values of each setting are premissible
 * given the setting's min and max values. Checks that the min and max
 * restrictions are reasonable.
 */
void test_settings_table_values(void) {
    for (uint32_t idx = 0; idx < num_settings; idx++) {
        setting_t setting;
        memcpy(&setting, &settings[idx], sizeof(setting_t));

        // Check that the setting's default value makes sense.
        TEST_ASSERT_MESSAGE(setting.setting_value >= setting.min, "Setting min value is too small!");
        TEST_ASSERT_MESSAGE(setting.setting_value <= setting.max, "Setting max value is too large!");

        // Check that the min and max restrictions make sense.
        TEST_ASSERT_MESSAGE(setting.max != setting.min, "Setting min and max are equal!");
        TEST_ASSERT_MESSAGE(setting.max > setting.min, "Setting max value is less than or equal to the min!");
    }
}

/**
 * @brief Tests that each mutable setting has a set callback defined
 * and that each immutable setting does not have a set callback.
 */
void test_settings_table_callbacks(void) {
    for (uint32_t idx = 0; idx < num_settings; idx++) {
        setting_t setting;
        memcpy(&setting, &settings[idx], sizeof(setting_t));

        // Mutable settings must have a callback.
        if (setting.mutable) {
            TEST_ASSERT_MESSAGE(setting.set_callback, "Mutable setting does not have a callback!");
        } else {
            TEST_ASSERT_MESSAGE(setting.set_callback == NULL, "Immutable setting has a callback!");
        }
    }
}

/**
 * @brief Tests that each setting's NVCT configuration is correct.
 */
void test_settings_nvct_config(void) {
    uint32_t next_nvct_idx = 0;
    for (uint32_t idx = 0; idx < num_settings; idx++) {
        setting_t setting;
        memcpy(&setting, &settings[idx], sizeof(setting_t));

        // Settings should be configured only with NOT_NVCT or USE_NVCT. Other values are for internal use only.
        TEST_ASSERT_MESSAGE(setting.nvct == NOT_NVCT || setting.nvct == USE_NVCT, "Setting must be configured with NOT_NVCT or USE_NVCT only!");

        // Ensure that NVCT settings are ordered. This also enforces that there are no
        // duplicate NVCT indices used.
        if (setting.nvct == USE_NVCT) {
            TEST_ASSERT_MESSAGE(setting.nvct_idx == next_nvct_idx, "Setting's NVCT index is not unique or in order!");
            next_nvct_idx++;
        }

        // Non-NVCT settings should have an index of 0.
        if (setting.nvct == NOT_NVCT) {
            TEST_ASSERT_MESSAGE(setting.nvct_idx == 0, "Setting not using NVCT does not have an nvct_idx of 0!");
        }
    }
}