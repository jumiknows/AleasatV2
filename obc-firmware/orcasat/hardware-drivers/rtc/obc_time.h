/**
 * @file obc_time.h
 * @brief Functions for converting and dealing with time.
 * @author Andrada Zoltan, Richard Arthurs
 * @date January 26, 2020
 *
 * The functions and defs in this module are intended to be RTC-agnostic, although
 * they are sized and operate under the assumption of the PCA2129 RTC, which should
 * be fairly broadly applicable.
 *
 * ORCASat operates with an epoch start date/time of January 1, 2020 at 12:00:00 (12 PM).
 * This time corresponds to an epoch of 0.
 *
 * Assumptions across this module:
 * 	- Month 1 is January. Month 12 is December.
 * 	- Start year is 2020, but the RTC counters and structures can't hold the value 2020.
 * 	  They instead count from 0 (which we take to mean year 2000), so would be initialized
 * 	  with a value of 20 in most cases.
 * 	- Minutes/seconds range: 0 - 59
 * 	- Days range: 1 - 31, with leap years being handled as a special case.
 * 	- 24-hour time, with valid hours being 0 - 23.
 *
 * 	See the MAX_ and MIN_ defines for the limits.
 */

#ifndef OBC_TIME_H_
#define OBC_TIME_H_

#include "sys_common.h"

/**
 * @brief Start year/decade for ORCASat time reference.
 */
#define START_YEAR    2020
#define START_DECADE  20
#define START_CENTURY 2000

/*
 * @brief The lowest and highest values a time range may take (inclusive).
 *
 * These are driven by how the hardware RTC represents the values, and how
 * many bits are available in the real_time_t bitfield for representing the
 * range. The more restrictive value is taken.
 */
#define MIN_SECONDS 0
#define MAX_SECONDS 59
#define MIN_MINUTES 0
#define MAX_MINUTES 59
#define MIN_HOURS   0
#define MAX_HOURS   23
#define MIN_DAYS    1
#define MAX_DAYS    31
#define MIN_MONTHS  1
#define MAX_MONTHS  12
#define MIN_YEARS   START_DECADE
#define MAX_YEARS   27

/**
 * @brief The number of days in a month for leap years and non-leap years.
 *
 * Index 0 is January, index 11 is December.
 */
extern const uint8_t max_days_in_month[12];
extern const uint8_t max_days_in_month_leap_year[12];

/**
 * @brief ORCASat epoch time type.
 *
 * This is defined as an int32_t (signed) to maintain compatibility with standard time.h functions
 * which use -1 to indicate an error. Modern systems actually use int64_t, but we don't need that
 * many bits to represent the time ranges that are reasonable for us. More info:
 * https://nuxi.nl/blog/2016/06/29/cloudlibc-timezones.html
 *
 * The maximum value of an int32_t is 2,147,483,647. When converted to seconds, that's about 68
 * years.
 */
typedef int32_t epoch_t;

/**
 * @brief ORCASat real time.
 *
 * This structure contains the time counts as represented in the RTC, but in decimal as opposed
 * to BCD.
 *
 * The day of week is excluded.
 *
 * Notes are for PCA2129 RTC chip.
 */
typedef struct {             // Range for PCA2129 or this struct, whichever is more restrictive.
    unsigned int second : 6; // 0 - 59
    unsigned int minute : 6; // 0 - 59
    unsigned int hour : 5;   // 0 - 23, in 24-hour mode (default), 0 - 11 in 12-hour mode
    unsigned int day : 6;    // 1 - 31, February gets 29 days if the year is exactly divisible by 4.
    unsigned int month : 4;  // 1 - 12
    unsigned int year : 6;   // 0 - 63, 0 = year 2000 for us. Note: PCA2129 range is 0-99.
    epoch_t epoch;           // The (optional) epoch value.
} real_time_t;

/**
 * @brief 12:00:00 PM January 20, 2020.
 */
extern const real_time_t orca_time_init;

/**
 * @brief Invalid or lack of epoch time. This can be used to indicate that the epoch component of a
 * real_time_t has not been calculated yet, or could not be calculated from the base real time.
 */
extern const epoch_t no_epoch;

/**
 * @brief The same as @ref no_epoch, but truly const and suitable for initializing structures. If,
 * for example, epoch_t fields of the RTC structure are initialized with @ref no_epoch, the error
 * "expression must have a constant value" will occur.
 * https://stackoverflow.com/questions/11702629/c-error-expression-must-have-a-constant-value
 *
 * Prefer no_epoch at runtime, but use NO_EPOCH for struct initialization.
 */
#define NO_EPOCH -1

/**
 * @brief Length in chars of the string returned by the time_to_ymd_string function
 * 
 * @ref time_to_ymd_string
 */
#define YMD_STRING_LEN 11

/* Public API -------------- */

// Time conversion utilities
epoch_t real_time_to_epoch(const real_time_t* real_time);
bool epoch_to_real_time(const epoch_t epoch, real_time_t* real_time);
void time_to_ymd_string(const real_time_t* curr_time, char* buf);
int32_t delta(real_time_t* to, real_time_t* from);

// General Validation
bool is_leap_year(uint16_t year);
bool is_real_time_valid(const real_time_t* real_time);
bool times_are_within(const epoch_t time_a, const epoch_t time_b, epoch_t within_sec);
bool is_year_valid(const uint32_t year);
bool is_month_valid(const uint32_t month);
bool is_day_valid(const uint32_t year, const uint32_t month, const uint32_t day);
bool is_hour_valid(const uint32_t hour);
bool is_minute_valid(const uint32_t minute);
bool is_second_valid(const uint32_t second);

// Backend
void increment_real_time(real_time_t* real_time);
bool set_real_time_epoch(real_time_t* real_time);

#endif /* ORCASAT_HARDWARE_DRIVERS_RTC_OBC_TIME_H_ */
