/**
 * @file obc_time.c
 * @brief Time conversion functions.
 * @author Andrada Zoltan, Richard Arthurs
 * @date January 26, 2020
 *
 * See obc_time.h for more info.
 */

#include "obc_time.h"
#include "printf.h"
#include <string.h>

/**
 * @brief The number of seconds in a time range.
 */
#define SECONDS_IN_YEAR   31536000
#define SECONDS_IN_DAY    86400
#define SECONDS_IN_HOUR   3600
#define SECONDS_IN_MINUTE 60

/**
 * @brief The number of days in a month for regular years.
 *
 * Index 0 is January, index 11 is December.
 */
const uint8_t max_days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
 * @brief The number of days in a month for leap years.
 *
 * Index 0 is January, index 11 is December.
 */
const uint8_t max_days_in_month_leap_year[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
 * @brief The 3-letter text abreviation for each month.
 */
const char month_to_string[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

/**
 * @brief Cumulative seconds per month for converting @ref real_time_t to a rough epoch with one
 * second resolution.
 *
 * Month 0 is January and the count for February is for a non-leap year.
 *
 * The RTC gives the month and the day. With the month, use this array to determine how many seconds
 * all months before this one have contributed to the time.
 */
const uint32_t cumulative_seconds_in_month[12] = {0, 2678400, 5097600, 7776000, 10368000, 13046400, 15638400, 18316800, 20995200, 23587200, 26265600, 28857600};

/**
 * @brief ORCASat time initialization to 12:00:00 on January 1, 2020.
 */
const real_time_t alea_time_init = {year: 20, month: 1, day: 1, hour: 0, minute: 0, second: 0, epoch: 0};

/**
 * @brief Ten second time useful for simple delays.
 */
const real_time_t ten_sec_time = {year: 20, month: 1, day: 1, hour: 0, minute: 0, second: 10, epoch: 0};

/**
 * @brief Invalid or no epoch indicator.
 *
 * Since epochs start at 0, -1 is therefore an invalid epoch. This can be used
 * to indicate that no epoch has been calculated, or that the application tried
 * to calculate an epoch from an invalid real_time_t.
 */
const epoch_t no_epoch = -1;

/**
 * @brief Converts real_time_t to Orca epoch.
 *
 * The base time is 12:00:00 PM on January 1, 2020.
 *
 * Algorithm:
 *  - Figure out how many complete years since 2020 have passed. Add that * number of seconds in a
 * year.
 *  - Figure out how many complete months have occurred. Add the cumulative number of seconds to get
 * to that month.
 *  - Do the same for days/hours/minutes.
 *  - Add in leap years if applicable.
 *
 *  @warning This conversion is only valid up to 2027 end of year. Leap years after this point are
 * not handled by the conversion.
 *
 *  @param[in] real_time The real_time to convert to epoch.
 *  @return the epoch, the number of seconds since 12:00:00 on January 1, 2020. Returns -1
 * (no_epoch) if an invalid real_time_t was given.
 */
epoch_t real_time_to_epoch(const real_time_t *real_time) {
    if (is_real_time_valid(real_time) == false) {
        return no_epoch;
    }

    // Naming convention: time_base (base being the lowest expected value)
    epoch_t the_time         = 0;
    uint32_t years_past_2020 = real_time->year - START_DECADE; // The year, relative to our start of 2020.
    uint8_t month_1          = real_time->month;
    uint8_t day_1            = real_time->day;
    uint8_t hour_0           = real_time->hour;
    uint8_t minute_0         = real_time->minute;
    uint8_t second_0         = real_time->second;

    // Calculate the epoch, assuming it is not a leap year
    the_time =
        (years_past_2020 * SECONDS_IN_YEAR) + (cumulative_seconds_in_month[(month_1 - 1)]) + ((day_1 - 1) * SECONDS_IN_DAY) + (hour_0 * SECONDS_IN_HOUR) +
        (minute_0 * SECONDS_IN_MINUTE) + second_0;

    /*
     * The first case handles the leap year 2020. If we are past 2020, then add an extra
     * day to the epoch to account for the extra day in 2020. If we are in 2020 currently,
     * only add an extra day if we are in March or later.
     */
    if ((years_past_2020 > 0) || (month_1 > 2)) {
        the_time += SECONDS_IN_DAY;
    }

    /**
     * The second case handles the leap year 2024. If we are past 2024, then add an extra
     * day to the epoch to account for the extra day in 2024. If we are in 2024 currently,
     * only add an extra day if we are in March or later.
     */
    if ((years_past_2020 > 4) || ((years_past_2020 == 4) && (month_1 > 2))) {
        the_time += SECONDS_IN_DAY;
    }

    return the_time;
}

/**
 * @brief Converts an epoch to a real_time. Also updates the .epoch field
 * of the provided real_time_t to the given epoch.
 *
 *  @warning This conversion is only valid up to 2027 end of year. Leap years after this point are
 * not handled by the conversion.
 *
 * @param[in] epoch The epoch to convert.
 * @param[out] real_time The real time to populate.
 * @return True if a valid epoch was given. False otherwise.
 */
bool epoch_to_real_time(const epoch_t epoch, real_time_t *real_time) {
    // Set to default value just in case real_time is not properly initialized and epoch is 0,
    memcpy(real_time, &alea_time_init, sizeof(real_time_t));

    real_time->epoch = epoch;

    if (epoch < 0) {
        return false;
    }

    // Calculate seconds
    real_time->second = epoch % (MAX_SECONDS + 1);

    // Calculate minute
    real_time->minute = (epoch / SECONDS_IN_MINUTE) % (MAX_MINUTES + 1);

    // Calculate hour
    real_time->hour = (epoch / SECONDS_IN_HOUR) % (MAX_HOURS + 1);

    // Calculate year by first calculating the total number of full days that have passed
    // and then subtracting full years from this number until we find the current year
    int32_t num_days         = epoch / SECONDS_IN_DAY;
    uint32_t years_past_2020 = 0;

    uint8_t i = 0;

    for (i = 0; i < 27; i++) {
        uint16_t days_in_year;

        if (is_leap_year(years_past_2020 + START_YEAR)) {
            days_in_year = 366;
        } else {
            days_in_year = 365;
        }

        // If number of days remaining is less than days in this year, we
        // have found the current year
        if (num_days < days_in_year) {
            break;
        } else {
            num_days -= days_in_year;
            years_past_2020++;
        }
    }

    real_time->year = years_past_2020 + START_DECADE;

    // Calculate month by looping through the days in each month and checking
    // if num_days is within the boundary of this month
    const uint8_t *days_in_month;

    if (is_leap_year(years_past_2020 + START_YEAR)) {
        days_in_month = max_days_in_month_leap_year;
    } else {
        days_in_month = max_days_in_month;
    }

    i                 = 0;
    uint16_t cum_days = 0;

    for (i = 0; i < (MAX_MONTHS + 1); i++) {
        if (num_days < cum_days) {
            real_time->month = i;
            break;
        }

        cum_days += days_in_month[i];
    }

    // Calculate day, add 1 since MIN_DAYS is 1 not 0
    real_time->day = (days_in_month[i - 1] - (cum_days - num_days)) + 1;

    return true;
}

/**
 * @brief Increments the fields in a real_time_t in the same pattern as the
 * RTC chip.
 *
 * When called at 1 Hz by a task, this simulates an external RTC. When called
 * as many times as there are seconds in an epoch, this effectively converts
 * between epoch and real_time.
 *
 * @param[in] real_time The real time to increment.
 */
void increment_real_time(real_time_t *real_time) {
    real_time->second++;

    // Adjust for overflow
    if (real_time->second > MAX_SECONDS) {
        real_time->second = 0;
        real_time->minute++;
    }

    if (real_time->minute > MAX_MINUTES) {
        real_time->minute = 0;
        real_time->hour++;
    }

    if (real_time->hour > MAX_HOURS) {
        real_time->hour = 0;
        real_time->day++;
    }

    // Handle leap year
    if (is_leap_year(real_time->year)) {
        if (real_time->day > (max_days_in_month_leap_year[real_time->month - 1])) { // Months are [1, 12], array indices are [0, 11]
            real_time->day = 1;
            real_time->month++;
        }
    } else {
        if (real_time->day > (max_days_in_month[real_time->month - 1])) {
            real_time->day = 1;
            real_time->month++;
        }
    }

    if (real_time->month > MAX_MONTHS) {
        real_time->month = 1; // Go back to January
        real_time->year++;
    }

    // Update the epoch field
    set_real_time_epoch(real_time);
}

/**
 * @brief Sets the epoch field of the real_time_t.
 *
 * If the epoch cannot be calculated, this will return false and the epoch field will be set to
 * @ref no_epoch in the provided real_time.
 *
 * @param[inout] real_time The real time to use when calculating the epoch. The epoch field will be
 * updated.
 *
 * @return True if the epoch was calculated successfully and is valid. False otherwise.
 */
bool set_real_time_epoch(real_time_t *real_time) {
    real_time->epoch = real_time_to_epoch(real_time);
    return !(real_time->epoch == no_epoch);
}

/**
 * @brief Checks if the given year is leap or not.
 *
 * @param[in] year The year to check.
 * @return True if leap year, false if not.
 */
bool is_leap_year(uint16_t year) {
    if (((year % 4) != 0) || (((year % 100) == 0) && ((year % 400) != 0))) {
        return false;
    } else {
        return true;
    }
}

/**
 * @brief Checks if the given real_time_t is valid or not.
 *
 * @param[in] real_time The real_time_t to check.
 * @return True if the real_time is valid, false if it's not.
 */
bool is_real_time_valid(const real_time_t *real_time) {
    if (!is_year_valid(real_time->year)) {
        return false;
    }

    if (!is_month_valid(real_time->month)) {
        return false;
    }

    if (!is_day_valid(real_time->year, real_time->month, real_time->day)) {
        return false;
    }

    if (!is_hour_valid(real_time->hour)) {
        return false;
    }

    if (!is_minute_valid(real_time->minute)) {
        return false;
    }

    if (!is_second_valid(real_time->second)) {
        return false;
    }

    return true;
}

/**
 * @brief Checks if the provided year is valid or not.
 *
 * @param year: Year to check
 * @return True if the year is valid, false if it's not
 */
bool is_year_valid(const uint32_t year) {
    // Year should always be initialized to 20, and the max supported year is 27
    if ((year < MIN_YEARS) || (year > MAX_YEARS)) {
        return false;
    }

    return true;
}

/**
 * @brief Checks if the provided month is valid or not.
 *
 * @param month: Month to check
 * @return True if the month is valid, false if it's not
 */
bool is_month_valid(const uint32_t month) {
    // Month range is 1 - 12
    if ((month < MIN_MONTHS) || (month > MAX_MONTHS)) {
        return false;
    }

    return true;
}

/**
 * @brief Checks if the provided day is valid or not.
 *
 * @param year: Current year, must be already validated
 * @param month: Current month, must be already validated
 * @param day: Day to check
 * @return True if the day is valid, false if it's not
 */
bool is_day_valid(const uint32_t year, const uint32_t month, const uint32_t day) {
    // Day range is 1 - 31
    if ((day < MIN_DAYS) || (day > MAX_DAYS)) {
        return false;
    }

    if (is_leap_year(year + START_CENTURY)) {
        if (day > max_days_in_month_leap_year[month - 1]) {
            return false;
        }
    } else {
        if (day > max_days_in_month[month - 1]) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Checks if the provided hour is valid or not.
 *
 * @param hour: Hour to check
 * @return True if the hour is valid, false if it's not
 */
bool is_hour_valid(const uint32_t hour) {
    // We use 24-hour time, so hour is 0 - 23. It's a uint so it cannot be less than 0.
    if (hour > MAX_HOURS) {
        return false;
    }

    return true;
}

/**
 * @brief Checks if the provided minute is valid or not.
 *
 * @param minute: Minute to check
 * @return True if the minute is valid, false if it's not
 */
bool is_minute_valid(const uint32_t minute) {
    // Minute range is 0 - 59
    if (minute > MAX_MINUTES) {
        return false;
    }

    return true;
}

/**
 * @brief Checks if the provided second is valid or not.
 *
 * @param second: Second to check
 * @return True if the second is valid, false if it's not
 */
bool is_second_valid(const uint32_t second) {
    // Second range is 0 - 59
    if (second > MAX_SECONDS) {
        return false;
    }

    return true;
}

/**
 * @brief Determines if two epochs are with a certain number of seconds of each other.
 *
 * @param[in] time_a The first time to compare.
 * @param[in] time_b The time to compare against.
 * @param[in] within_sec How many seconds difference to allow. Zero means times must be equal.
 * Negatives are invalid and will return false.
 *
 * @return True if the times are within the desired range. False if times are not within the desired
 * range, within_sec is negative.
 */
bool times_are_within(const epoch_t time_a, const epoch_t time_b, epoch_t within_sec) {
    // Return immediately if within_sec is invalid.
    if (within_sec < 0) {
        return false;
    }

    // Compare times if they are both valid
    if ((time_a >= 0) && (time_b >= 0)) {
        return (abs(time_a - time_b) <= within_sec);
    } else {
        return false; // One or the other time is invalid
    }
}

/**
 * @brief Returns the current date as a formatted string in the form DDMonYYYY
 * @param[in] curr_time The time to use. The real time, not the epoch, will be used.
 * @param[out] buf A character buffer where the generated string will be placed. Must be at least 11 bytes
 *
 * @return Date as a string
 */
void time_to_ymd_string(const real_time_t *curr_time, char *buf) {
    snprintf(buf, YMD_STRING_LEN, "%02d%s%04d", curr_time->day, month_to_string[curr_time->month - 1], curr_time->year + START_CENTURY);
}

/**
 * @brief Returns the number of seconds between the epochs in two real_time_t values.
 *
 * @warning If either time is invalid, the delta will be incorrect. @ref is_real_time_valid
 * should be called on the inputs before calling this function.
 *
 * @param[in] to a real_time_t value
 * @param[in] from another real_time_t value
 * @return to - from
 */
int32_t delta(real_time_t *to, real_time_t *from) {
    epoch_t to_epoch   = real_time_to_epoch(to);
    epoch_t from_epoch = real_time_to_epoch(from);
    return to_epoch - from_epoch;
}
