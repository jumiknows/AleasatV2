/**
 * @file obc_rtc.h
 * @brief Top-level functions for getting the time, setting alarms, and dealing with either
 * the RTC or the mocked RTC.
 *
 *  Created on: Jun 12, 2017
 *      Author: Steven + Richard + Victor
 *
 *  Edited on: Jan 19, 2019
 *  	Author: Julie Belleville
 *  	Changes: altered to act as top-level API
 *
 *  Edited on: May 20, 2019
 *  	Author: Andrada Zoltan
 *  	Changes: - removed support for old RTC
 *  	         - changed API to support real time instead of epoch
 *
 *  	 Depending on the platform, the top-level RTC module will select the appropriate RTC.
 *  	 On LaunchPads, the RTC is mocked with a FreeRTOS task. On OBC platforms, the two hardware
 *  	 RTCs are treated as one from the perspective of the user.
 */

#ifndef OBC_RTC_H_
#define OBC_RTC_H_

#include "sys_common.h"
#include "obc_error.h"
#include "obc_time.h"
#include "rtc_state.h"
#include "rtos.h"

/**
 * @brief Recursive mutex to protect access to the RTC.
 *
 * A recursive mutex is used to allow a single task to take the mutex multiple times. This is
 * required because internal RTC code calls the logger on errors, which needs a timestamp, which
 * requires the RTC.
 */
extern SemaphoreHandle_t rtc_mutex;

/**
 * @brief RTC mutex grab timeout.
 */
#define RTC_MUTEX_TIMEOUT_MS 1500

typedef void (*rtc_alarm_cb_t)(void *arg);

// Initialization steps
void rtc_create_infra(void);
void rtc_init_backup(void);
void rtc_init(void);

// General application API
epoch_t rtc_get_epoch_time(void);
epoch_t get_epoch_time(void);
rtc_err_t rtc_get_current_time(real_time_t* curr_time);
rtc_err_t get_current_time(real_time_t* curr_time);

// RTC configuration
rtc_err_t rtc_set_current_time(real_time_t* curr_time, active_rtc_t rtcs, bool update_backup);
rtc_err_t switch_active_rtc(active_rtc_t rtc);

rtc_err_t rtc_set_relative_alarm(uint32_t seconds_to_alarm);

rtc_err_t rtc_set_absolute_alarm(uint32_t timestamp, rtc_alarm_cb_t cb, void *cb_arg);

void rtc_set_ts(bool use_ts);
#endif /* OBC_RTC_H_ */
