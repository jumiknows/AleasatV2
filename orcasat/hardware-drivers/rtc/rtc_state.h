/**
 * @file rtc_state.h
 * @brief Contains setters and getters for RTC state.
 *
 * The RTC state management and tracking is done in this module so it is very clear what the
 * RTC configuration options are, and so internal stats can be generated. In the future, many
 * of these settings will be backed by NVCT and/or be configurable by command.
 *
 * Note that state configuration (setters) must be called through the RTC mutex to prevent possible
 * logic errors in code that makes decisions with RTC state. See notes above the setters for safe
 * places where they can safely be called.
 *
 * @author Richard A
 * @date February 16, 2020
 */

#ifndef RTC_STATE_H_
#define RTC_STATE_H_

#include "sys_common.h"

/**
 * @brief Describes which RTC is currently active.
 *
 * If both RTCs are active, RTC_A is normally chosen. Otherwise, one or the other
 * of the hardware RTCs can be manually enabled, or the backup epoch can be used.
 *
 * Note: The values in this enum are designed such that they can be ANDed together to determine
 * the status of the RTC. This also makes it easy to determine which RTC is active from a
 * log message that just contains the enum value. Similarly, states of either RTC can be ORed
 * together and shifted to construct the enum value. If changing the enumeration, ensure that any
 * math and casts are still compatible.
 *
 * @warning If updating this string, also update the @ref time_source_names array.
 *
 * For example, if the status is BOTH: 0b00000011 and we want to determine if RTC_A: 0b00000001
 * should be queried, 0b00000011 & 0b00000001 is 0b00000001, or the RTC_A value itself.
 */
typedef enum { BACKUP = 0, RTC_A = 1, RTC_B = 2, BOTH = 3 } active_rtc_t;

void init_rtc_state_backup(void);
void init_rtc_state_hw(void);

/**
 * @brief Setters. Setters take the RTC mutex, so are safe to call from any thread. However,
 * in most cases, they should be called through the top-level obc_rtc.h API.
 */
void set_state_active_rtc(active_rtc_t rtc); // Active RTC indicated which RTCs will be queried for time requests.
void set_state_used_rtc(active_rtc_t rtc);   // Used RTC indicates which RTC was actually used to
                                             // produce the most recent timestamp.

/**
 * @brief Getters. These are safe to call from anywhere.
 */
active_rtc_t get_active_rtc(void); // Active RTC indicates which RTCs will be queried for time requests.
bool using_timestamp(void);
bool is_backup(void);
uint8_t get_tolerance(void);
void set_ts(bool use_ts);

/**
 * @brief Failure indicators. These should only be called from within code wrapped by the RTC mutex
 * used in @ref obc_rtc.c.
 */
void rtc_overridden_with_backup(void);
void register_rtc_mismatch(uint32_t mismatch_magnitude);

/**
 * @brief Informational printing.
 */
void print_rtc_info(void);
void print_rtc_stats(void);
const char* get_time_source_name(active_rtc_t rtc);

#endif /* RTC_STATE_H_ */
