/**
 * @file dual_rtc_pca2129.c
 * @brief Handlers for dual RTCs.
 * @author Richard Arthurs
 * @date February 9, 2020
 *
 * This module is meant to abstract away the details required to deal with multiple RTC chips.
 * Dealing with two RTCs generally involves duplicating actions on both and ensuring that nominally,
 * the result from RTC A is returned to the user via the top-level @ref obc_rtc.h API.
 *
 * The active RTC is normally RTC A, which is compared with RTC B during time reads. If it is known
 * that either RTC is faulty, the active RTC can be reconfigured from the ground. This concept
 * extends to overriding all external hardware RTC counts with the backup epoch too.
 *
 * When an RTC is not enabled, no MIBSPI communications are made with it.
 *
 * This API is designed such that as far as is possible, the time returned will always be valid,
 * with the backup epoch being used in the worst case. RTC error codes are still returned, as
 * callers may care that the backup epoch is being used, for example.
 */

#ifndef DUAL_RTC_PCA2129_H_
#define DUAL_RTC_PCA2129_H_

#include "rtc_state.h"

// Init and maintenance
void init_both_rtc(void);

rtc_err_t get_dual_time(real_time_t* time);
rtc_err_t set_dual_time(real_time_t* time, active_rtc_t rtcs);

#endif /* DUAL_RTC_PCA2129_H_ */
