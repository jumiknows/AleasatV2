/**
 * @file rtc_cmds.h
 * @brief Commands that deal with RTC configuration.
 * @date Feb 25, 2020
 * @author Richard A
 */

#ifndef RTC_CMDS_H_
#define RTC_CMDS_H_

/**
 * @brief Command to update the time on various sets of RTCs.
 */
void command_update_rtc_time(char** arguments);

/**
 * @brief Command to update the active RTCs (the ones that will be communicated with).
 */
void command_update_active_rtc(char** arguments);

#endif /* RTC_CMDS_H_ */
