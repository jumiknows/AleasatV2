/*
 * rtc_mock.h
 *
 *  Created on: Apr 19, 2019
 *      Author: Richard
 */

#ifndef RTC_MOCK_H_
#define RTC_MOCK_H_

#include "sys_common.h"
#include "obc_rtc.h"

void rtc_init_mock(void);
rtc_err_t rtc_get_current_time_mock(real_time_t* curr_time);
rtc_err_t rtc_set_current_time_mock(const real_time_t* curr_time);
rtc_err_t rtc_get_startup_status_mock(void);

#endif /* RTC_MOCK_H_ */
