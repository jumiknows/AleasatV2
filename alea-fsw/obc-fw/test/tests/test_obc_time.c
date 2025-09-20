#include "unity.h"

#include "obc_time.h"

/**
 * @brief Tests for the obc_time module.
 */

// Tests
void test_is_real_time_valid();
void test_within_time_valid(void);
void test_within_time_equal(void);
void test_within_time_negative_sec(void);
void test_within_time_negative_time(void);
void test_epoch_to_real_time(void);
void test_is_year_valid(void);
void test_is_month_valid(void);
void test_is_day_valid(void);
void test_is_hour_valid(void);
void test_is_minute_valid(void);
void test_is_second_valid(void);

/**
 * @brief A test case that can be easily constructed.
 */
typedef struct {
    real_time_t time;
    bool expected;   // Expected result
    const char* msg; // This should indicate what this case is trying to hit.
} test_input_t;

typedef struct {
    real_time_t time;
    epoch_t expected;
} test_unix_input_t;

/**
 * @brief Test cases. These test the min/max ranges for each time range.
 *
 * They do not test that the correct number of days are detected per month.
 */
test_input_t cases[] = {
    {.time = {.year = 20, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0}, .expected = true, .msg = "Orca time init"},
    {.time = {.year = 19, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0}, .expected = false, .msg = "Orca time init, invalid year (low)"},
    {.time = {.year = 27, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0}, .expected = true, .msg = "Orca time init, max year"},
    {.time = {.year = 20, .month = 0, .day = 1, .hour = 0, .minute = 0, .second = 0}, .expected = false, .msg = "Orca time init, invalid month (low)"},
    {.time = {.year = 20, .month = 13, .day = 1, .hour = 0, .minute = 0, .second = 0}, .expected = false, .msg = "Orca time init, invalid month (high)"},
    {.time = {.year = 20, .month = 1, .day = 1, .hour = 24, .minute = 0, .second = 0}, .expected = false, .msg = "Orca time init, invalid hour (high)"},
    {.time = {.year = 20, .month = 1, .day = 1, .hour = 0, .minute = 60, .second = 0}, .expected = false, .msg = "Orca time init, invalid min (high)"},
    {.time = {.year = 20, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 60}, .expected = false, .msg = "Orca time init, invalid sec (high)"},
    {.time = {.year = 20, .month = 3, .day = 2, .hour = 12, .minute = 32, .second = 4}, .expected = true, .msg = "OK time"},
    {.time = {.year = 20, .month = 3, .day = 29, .hour = 2, .minute = 32, .second = 4}, .expected = true, .msg = "OK time"},
    {.time = {.year = 20, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0}, .expected = true, .msg = "Orca time init"},
    {.time = {.year = 19, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0}, .expected = false, .msg = "Orca time init, invalid year (low)"},
};
const uint32_t num_cases = sizeof(cases) / sizeof(test_input_t);

/**
 * @brief Unix time cases.
 */
test_unix_input_t unix_cases[] = {
    {.time = {.year = 20, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0}, .expected = 1577836800},
    {.time = {.year = 27, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0}, .expected = 1798761600},
    {.time = {.year = 20, .month = 3, .day = 2, .hour = 12, .minute = 32, .second = 4}, .expected = 1583152324},
    {.time = {.year = 20, .month = 3, .day = 29, .hour = 2, .minute = 32, .second = 4}, .expected = 1585449124},
    {.time = {.year = 21, .month = 11, .day = 24, .hour = 10, .minute = 2, .second = 9}, .expected = 1637748129},
};
const uint32_t num_unix_cases = sizeof(unix_cases) / sizeof(test_unix_input_t);

/**
 * @brief Tests is_real_time_valid against several cases.
 */
void test_is_real_time_valid() {
    // Construct arrays of the results, then use a unity array assertion so it's easy
    // to determine the test case that failed.
    // More info:
    // https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md
    bool actual_results[num_cases];
    bool expected_results[num_cases];

    for (uint32_t i = 0; i < num_cases; i++) {
        real_time_t time    = alea_time_init;
        actual_results[i]   = is_real_time_valid(&cases[i].time);
        expected_results[i] = cases[i].expected;
    }
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_results, actual_results, num_cases);
}

/**
 * @brief Tests is_year_valid against several cases.
 */
void test_is_year_valid(void) {
    TEST_ASSERT_TRUE(is_year_valid(27));
    TEST_ASSERT_TRUE(is_year_valid(20));
    // Should fail since an acceptable year is [20,27]
    TEST_ASSERT_FALSE(is_year_valid(19));
    TEST_ASSERT_FALSE(is_year_valid(28));
    TEST_ASSERT_FALSE(is_year_valid(200));
}

/**
 * @brief Tests is_month_valid against several cases.
 */
void test_is_month_valid(void) {
    TEST_ASSERT_TRUE(is_month_valid(1));
    TEST_ASSERT_TRUE(is_month_valid(12));
    TEST_ASSERT_FALSE(is_month_valid(0));
    TEST_ASSERT_FALSE(is_month_valid(13));
    TEST_ASSERT_FALSE(is_month_valid(90));
}

/**
 * @brief Tests is_day_valid against several cases.
 */
void test_is_day_valid(void) {
    TEST_ASSERT_TRUE(is_day_valid(20, 8, 1));
    TEST_ASSERT_TRUE(is_day_valid(20, 9, 30));
    // Leap day!
    TEST_ASSERT_TRUE(is_day_valid(20, 2, 29));
    // Invalid because the year >20.
    TEST_ASSERT_FALSE(is_day_valid(21, 2, 29));
    // Invalid because the day is 32.
    TEST_ASSERT_FALSE(is_day_valid(19, 4, 32));
    // Invalid because the 13th month in unlucky.
    TEST_ASSERT_FALSE(is_day_valid(19, 13, 12));
}

/**
 * @brief Tests is_hour_valid against several cases.
 */
void test_is_hour_valid(void) {
    TEST_ASSERT_TRUE(is_hour_valid(0));
    TEST_ASSERT_TRUE(is_hour_valid(23));
    // hour is [0,24)
    TEST_ASSERT_FALSE(is_hour_valid(24));
    TEST_ASSERT_FALSE(is_hour_valid(49));
    TEST_ASSERT_FALSE(is_hour_valid(230));
}

/**
 * @brief Tests is_minute_valid against several cases.
 */
void test_is_minute_valid(void) {
    TEST_ASSERT_TRUE(is_minute_valid(0));
    TEST_ASSERT_TRUE(is_minute_valid(59));
    // minute is [0,60)
    TEST_ASSERT_FALSE(is_minute_valid(60));
    TEST_ASSERT_FALSE(is_minute_valid(128));
    TEST_ASSERT_FALSE(is_minute_valid(84));
}

/**
 * @brief Tests is_second_valid against several cases.
 */
void test_is_second_valid(void) {
    TEST_ASSERT_TRUE(is_second_valid(0));
    TEST_ASSERT_TRUE(is_second_valid(59));
    // second is [0,60)
    TEST_ASSERT_FALSE(is_second_valid(60));
    TEST_ASSERT_FALSE(is_second_valid(73));
    TEST_ASSERT_FALSE(is_second_valid(122));
}

/**
 * @brief Test that valid cases of within_time work when the times are unequal.
 */
void test_within_time_valid(void) {
    TEST_ASSERT_TRUE(times_are_within(0, 4, 5));
    TEST_ASSERT_TRUE(times_are_within(0, 5, 5));
    TEST_ASSERT_TRUE(times_are_within(1, 10, 10));
    TEST_ASSERT_TRUE(times_are_within(10, 15, 5));
    // 10+5<16 so not within.
    TEST_ASSERT_FALSE(times_are_within(10, 16, 5));
}

/**
 * @brief Test that cases where times are equal return true.
 */
void test_within_time_equal(void) {
    TEST_ASSERT_TRUE(times_are_within(0, 0, 5));
    TEST_ASSERT_TRUE(times_are_within(5, 5, 5));
    TEST_ASSERT_TRUE(times_are_within(0, 0, 0));
    TEST_ASSERT_TRUE(times_are_within(5, 5, 0));
}

/**
 * @brief Test that passing a negative within time fails. It doesn't make sense
 * to try to determine if times are within a negative value of each other.
 */
void test_within_time_negative_sec(void) {
    TEST_ASSERT_FALSE(times_are_within(0, 0, -1));
    TEST_ASSERT_FALSE(times_are_within(2, 5, -1));
}

/**
 * @brief Test that passing a negative time a or b fails.
 *
 * Negative times are invalid, so they should always fail.
 */
void test_within_time_negative_time(void) {
    TEST_ASSERT_FALSE(times_are_within(-1, 0, 5));
    TEST_ASSERT_FALSE(times_are_within(0, -1, 5));
    TEST_ASSERT_FALSE(times_are_within(-1, -1, -1));
}

/**
 * @brief Test when to is after from. The returned value should be positive.
 */
void test_delta_to_after_from(void) {
    real_time_t to   = {.year = 20, .month = 1, .day = 1, .hour = 1, .minute = 0, .second = 0};
    real_time_t from = {.year = 20, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0};
    TEST_ASSERT_EQUAL_INT32_MESSAGE(3600, time_delta(&to, &from), "1 hour or 3600 seconds");
}

/**
 * @brief Test when to is before from. The returned value should be negative.
 */
void test_delta_to_before_from(void) {
    real_time_t to   = {.year = 20, .month = 1, .day = 1, .hour = 1, .minute = 0, .second = 0};
    real_time_t from = {.year = 20, .month = 1, .day = 1, .hour = 1, .minute = 1, .second = 0};
    TEST_ASSERT_EQUAL_INT32_MESSAGE(-60, time_delta(&to, &from), "1 min or 60 seconds");
}

/**
 * @brief Tests the conversion from epoch to real time for every datetime within
 * the OBC valid date range, in increments of 59 seconds.
 */
void test_epoch_to_real_time(void) {
    epoch_t end_epoch = 252460745; // December 31, 2027 23:59:59

    epoch_t e     = 0;
    real_time_t t = {.year = 20, .month = 1, .day = 1, .hour = 0, .minute = 0, .second = 0};
    while (e < end_epoch) {
        real_time_t cnvr = {0};

        TEST_ASSERT_TRUE(epoch_to_real_time(e, &cnvr));
        TEST_ASSERT_EQUAL(e, cnvr.epoch);
        TEST_ASSERT_EQUAL(t.year, cnvr.year);
        TEST_ASSERT_EQUAL(t.month, cnvr.month);
        TEST_ASSERT_EQUAL(t.day, cnvr.day);
        TEST_ASSERT_EQUAL(t.hour, cnvr.hour);
        TEST_ASSERT_EQUAL(t.minute, cnvr.minute);
        TEST_ASSERT_EQUAL(t.second, cnvr.second);

        // Increment the test epoch and real time
        e += 59;
        for (int i = 0; i < 59; i++) {
            t.second++;

            // Adjust for overflow
            if (t.second > MAX_SECONDS) {
                t.second = 0;
                t.minute++;
            }
            if (t.minute > MAX_MINUTES) {
                t.minute = 0;
                t.hour++;
            }
            if (t.hour > MAX_HOURS) {
                t.hour = 0;
                t.day++;
            }

            // Handle leap year
            if (is_leap_year(t.year)) {
                if (t.day > (max_days_in_month_leap_year[t.month - 1])) {
                    t.day = 1;
                    t.month++;
                }
            } else {
                if (t.day > (max_days_in_month[t.month - 1])) {
                    t.day = 1;
                    t.month++;
                }
            }

            if (t.month > MAX_MONTHS) {
                t.month = 1; // Go back to January
                t.year++;
            }
        }
    }
}
