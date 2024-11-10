/**
 * @file rtc_cmds.h
 * @brief Commands that deal with RTC configuration.
 * @date Feb 25, 2020
 * @author Richard A
 */

#ifndef RTC_CMDS_H_
#define RTC_CMDS_H_
#include "obc_cmd.h"

/**
 * @brief Command to update the time on various sets of RTCs.
 */
void command_update_rtc_time(const cmd_argument_t* cmd);

/**
 * @brief Command to update the active RTCs (the ones that will be communicated with).
 */
void command_update_active_rtc(const cmd_argument_t* cmd);

/**
 * For testing only, bring out extract_rtc() so that we can run unit tests on it.
 */
#ifdef CEEDLING_TEST
#include "rtc_state.h"
bool extract_rtc(const cmd_argument_t* cmd, active_rtc_t* rtc, bool* set_backup);
#endif

#endif /* RTC_CMDS_H_ */
