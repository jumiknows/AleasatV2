/**
 * @file rtc_cmds.h
 * @brief Commands that deal with RTC configuration.
 * @date Feb 25, 2020
 * @author Richard A
 */

#include "rtc_cmds.h"
#include "obc_time.h"
#include "obc_rtc.h"
#include "obc_error.h"
#include "obc_misra.h"
#include "logger.h"
#include "obc_utils.h"
#include "obc_glibc.h"
#include <stdlib.h>
#include <string.h>

#define RTC_ARG_MAX_LEN 8

// Private function prototypes
rtc_err_t extract_time(char* arguments[7], real_time_t* time);
bool extract_rtc(const char* argument, active_rtc_t* rtc, bool* set_backup);

/**
 * @brief Updates the time on the desired RTCs after parsing the command arguments.
 *
 * @warning It is possible to communicate with inactive RTCs using the "all" command
 * argument. This should be used with caution.
 *
 * See @ref extract_rtc() for the accepted rtc format.
 * See @ref extract_time() for the accepted time format.
 *
 * Command args: rtc | year | month | day | hour | minute | second
 *
 * @param[in] cmd: The command structure to parse.
 */
void command_update_rtc_time(char** arguments) {
    // Determine which RTC should be updated
    active_rtc_t rtc_to_use = BACKUP;
    bool use_backup         = true;
    bool ok                 = extract_rtc(arguments[0], &rtc_to_use, &use_backup);

    // If that went well, proceed to try to set the time
    if (ok) {
        real_time_t set_time = orca_time_init;
        rtc_err_t err        = extract_time(arguments, &set_time);
        if (err != RTC_NO_ERR) {
            log_str_no_time(ERROR, RTC_LOG, true, "Invalid Time. RTC err: %d", err); // Indicates an error with the params
            return;
        }

        err = rtc_set_current_time(&set_time, rtc_to_use, use_backup);
        if (err == RTC_NO_ERR) {
            log_str(INFO, RTC_LOG, true, "Updated time on %s", get_time_source_name(rtc_to_use));
        } else {
            if ((err == RTC_REAL_TIME_INVALID) || (err == RTC_EPOCH_INVALID)) {
                log_str_no_time(ERROR, RTC_LOG, true, "Invalid Time. RTC err: %d",
                                err); // Indicates an error with the params
            } else {
                log_str_no_time(ERROR, RTC_LOG, true, "RTC comm issue. RTC err: %d",
                                err); // Otherwise, it's an RTC comms issue
            }
        }
    } else {
        log_str(ERROR, RTC_LOG, true, "Invalid time params."); // Invalid RTC param was passed in.
    }
}

/**
 * @brief Command to update the active RTC.
 *
 * Valid command arguments:
 * 	- "all"
 * 	- "a" or "b"
 * 	- "a_b" for both (equivalent to "all")
 * 	- "bk" for backup only
 *
 * @param[in] cmd The command structure to parse.
 */
void command_update_active_rtc(char** arguments) {
    active_rtc_t rtc_to_use = BACKUP;
    bool use_backup         = true;
    bool ok                 = extract_rtc(arguments[0], &rtc_to_use, &use_backup);

    if (rtc_to_use == get_active_rtc()) {
        // Desired RTC matches already active, so don't bother adjusting anything.
        log_str(INFO, RTC_LOG, true, "%s already active.", get_time_source_name(get_active_rtc()));
    } else {
        // Active RTC is different from what we want to set, so set it.
        if (ok) {
            rtc_err_t rtc_err = switch_active_rtc(rtc_to_use);

            // Log success or errors
            if (rtc_err == RTC_NO_ERR) {
                log_str(INFO, RTC_LOG, true, "Updated active to %s", get_time_source_name(get_active_rtc()));
            } else {
                log_str_no_time(ERROR, RTC_LOG, true, "Active RTC set err: %i", rtc_err);
            }
        } else {
            log_str_no_time(ERROR, RTC_LOG, true, "Invalid desired RTC param.");
        }
    }
}

/**
 * @brief Extracts the desired RTC from a string argument.
 * 	- The format is delimited by '_'
 * 	- RTC A and B are indicated by "a" or "b"
 * 	- The backup epoch is indicated by "bk"
 * 	- "act" can be used to set on currently-active RTCs only. Backup will only be set if it is the
 * active RTC, but "bk" can be appended too to force backup to be updated.
 * 	- Alternatively, "all" can be used to set time on all RTCs (including backup), whether active or
 * inactive. This is potentially dangerous.
 *
 * 	a/b/act should be usually be used in conjunction with bk.
 *
 * 	@param[in] cmd: The command structure. Argument 0 will be parsed by this function but the
 * command will not be modified.
 * 	@param[out] rtc: The RTCs retrieved from the command, irrespective of the "bk" flag.
 * 	@param[out] set_backup: Whether or not to update the backup RTC.
 * 	@returns True if there was no unexpected data in the argument string. False if an unexpected
 * value was passed in.
 */
bool extract_rtc(const char* argument, active_rtc_t* rtc, bool* set_backup) {
    // Set default outputs
    *rtc        = BACKUP;
    *set_backup = false;

    const char delim[2] = "_";
    char *token, *saveptr = NULL;

    // Strtok modifies the original string by inserting \0 where delimiters exist,
    // so operate on a copy of the first argument.
    char buf[RTC_ARG_MAX_LEN] = {'\0'};
    strcpy(buf, argument);

    // Tokenize the string
    token   = orca_strtok_r((char*)&buf, delim, &saveptr);
    bool ok = true;

    // Pull out the individual pieces, setting outputs as we go
    while (token != NULL) {
        if (strcmp(token, "a") == 0) {
            *rtc |= RTC_A;
        } else if (strcmp(token, "b") == 0) {
            *rtc |= RTC_B;
        } else if (strcmp(token, "bk") == 0) {
            *set_backup = true;
        } else if (strcmp(token, "all") == 0) {
            *set_backup = true;
            *rtc        = BOTH;
        } else if (strcmp(token, "act") == 0) {
            *rtc = get_active_rtc();
        } else {
            // We got an unexpected value in the command argument.
            ok = false;
        }

        token = orca_strtok_r(NULL, delim, &saveptr);
    }

    return ok;
}

/**
 * @brief Extracts the time from a set of string args and turns it into a
 * real_time_t if the time is valid.
 *
 * The command argument list is parsed with the following format expectation:
 *  args[0] = ignored by this function
 * 	args[1] = year
 * 	args[2] = month
 * 	args[3] = day
 * 	args[4] = hour
 * 	args[5] = minute
 * 	args[6] = second
 *
 * @param[in] args The command argument list.
 * @param[out] time The time output.
 * @return RTC_REAL_TIME_INVALID if time is not valid, RTC_NO_ERR otherwise
 */
rtc_err_t extract_time(char* arguments[7], real_time_t* time) {
    // Waiving MISRA check "atoi should not be used" Richard A, March 8, 2020
    OBC_MISRA_CHECK_OFF
    uint32_t val = atoi(arguments[1]);
    if (!is_year_valid(val)) {
        return RTC_REAL_TIME_INVALID;
    }
    time->year = val;

    val = atoi(arguments[2]);
    if (!is_month_valid(val)) {
        return RTC_REAL_TIME_INVALID;
    }
    time->month = val;

    val = atoi(arguments[3]);
    if (!is_day_valid(time->year, time->month, val)) {
        return RTC_REAL_TIME_INVALID;
    }
    time->day = val;

    val = atoi(arguments[4]);
    if (!is_hour_valid(val)) {
        return RTC_REAL_TIME_INVALID;
    }
    time->hour = val;

    val = atoi(arguments[5]);
    if (!is_minute_valid(val)) {
        return RTC_REAL_TIME_INVALID;
    }
    time->minute = val;

    val = atoi(arguments[6]);
    if (!is_second_valid(val)) {
        return RTC_REAL_TIME_INVALID;
    }
    time->second = val;
    OBC_MISRA_CHECK_ON

    return RTC_NO_ERR;
}
