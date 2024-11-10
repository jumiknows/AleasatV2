#include "unity.h"

#include "obc_error.h"
#include "rtc_cmds.h"
#include "obc_time.h"
#include "obc_utils.h"
#include "obc_glibc.h"
#include <string.h>

#include "mock_logger.h"
#include "mock_obc_rtc.h"
#include "mock_rtc_state.h"

void test_extract_rtc();

/**
 * @brief Tests various combinations of extracting the RTC and whether or not to
 * update the backup epoch a command string.
 *
 * Tests that:
 * 	- The RTCs selected match the arguments
 * 	- The backup epoch output is true, if appropriate
 * 	- Special values such as "all" are parsed correctly
 *
 * When testing the "act" behaviour, the command will determine which RTCs are currently
 * active from the RTC state handler. In these cases, mock @ref get_active_rtc() and check
 * that the value returned matches the mocked active RTC.
 */
void test_extract_rtc() {
    log_str_Ignore(); // Mock, just ignore any logs
    cmd_argument_t cmd;

    typedef struct {
        const char* arg;          // The string argument, as passed in by command
        active_rtc_t active;      // The active RTCs to select
        bool backup;              // Whether or not the backup should be used
        bool expect;              // Expected result from extract_rtc
        active_rtc_t active_mock; // Result to return from the mocked active RTCs.
        bool use_active_mock;     // Whether or not to use the mock for get_active_rtc
    } extract_rtc_test_t;

    // The test cases
    extract_rtc_test_t tests[] = {
        {.arg = "a_b_bk", .active = BOTH, .backup = true, .expect = true, .active_mock = BOTH, .use_active_mock = false},
        {.arg = "a_b", .active = BOTH, .backup = false, .expect = true, .active_mock = BOTH, .use_active_mock = false},
        {.arg = "all", .active = BOTH, .backup = true, .expect = true, .active_mock = BOTH, .use_active_mock = false},
        {.arg = "a", .active = RTC_A, .backup = false, .expect = true, .active_mock = BOTH, .use_active_mock = false},
        {.arg = "b", .active = RTC_B, .backup = false, .expect = true, .active_mock = BOTH, .use_active_mock = false},
        {.arg = "foo", .active = BACKUP, .backup = false, .expect = false, .active_mock = BOTH, .use_active_mock = false},
        {.arg = "a_bk", .active = RTC_A, .backup = true, .expect = true, .active_mock = BOTH, .use_active_mock = false},
        {.arg = "b_bk", .active = RTC_B, .backup = true, .expect = true, .active_mock = BOTH, .use_active_mock = false},
        {.arg = "bk", .active = BACKUP, .backup = true, .expect = true, .active_mock = BOTH, .use_active_mock = false},

        // Test the active behaviour, the .active result returned should equal the .active_mock
        // value
        {.arg = "act_bk", .active = BOTH, .backup = true, .expect = true, .active_mock = BOTH, .use_active_mock = true},
        {.arg = "act", .active = BOTH, .backup = false, .expect = true, .active_mock = BOTH, .use_active_mock = true},
        {.arg = "act", .active = RTC_A, .backup = false, .expect = true, .active_mock = RTC_A, .use_active_mock = true},
        {.arg = "act", .active = RTC_B, .backup = false, .expect = true, .active_mock = RTC_B, .use_active_mock = true},
        {.arg = "act", .active = BACKUP, .backup = false, .expect = true, .active_mock = BACKUP, .use_active_mock = true},
    };

    const uint32_t rtc_num_cases = sizeof(tests) / sizeof(extract_rtc_test_t);

    // Loop over each case, verify that the active, backup flag, and output from extract_rtc() is
    // correct.
    for (uint32_t i = 0; i < rtc_num_cases; i++) {
        // Setup the outputs to defaults
        active_rtc_t rtcs = BACKUP;
        bool use_backup   = false;

        strcpy((char*)&cmd, tests[i].arg);

        // If we are testing "active" behaviour, mock the get_active_rtc function to return the
        // desired value from the test case.
        if (tests[i].use_active_mock) {
            get_active_rtc_IgnoreAndReturn(tests[i].active_mock);
        }

        // Run extract_rtc, then check that the RTC to choose output and the backup are correct
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(tests[i].expect, extract_rtc(&cmd, &rtcs, &use_backup), tests[i].arg);
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(tests[i].active, rtcs,
                                        tests[i].arg); // Incorrect RTC was selected
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(tests[i].backup, use_backup,
                                        tests[i].arg); // Use of backup was parsed incorrectly

        // If we are testing "active," check that the output matches the mocked value
        if (tests[i].use_active_mock) {
            TEST_ASSERT_EQUAL_UINT8_MESSAGE(tests[i].active_mock, rtcs, tests[i].arg);
        }
    }
}
