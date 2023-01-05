/**
 * @file rtc_mock.h
 * @brief Software mock of a real-time clock
 */

#ifndef RTC_MOCK_H_
#define RTC_MOCK_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "sys_common.h"
#include "obc_rtc.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void rtc_init_mock(void);
rtc_err_t rtc_get_current_time_mock(real_time_t* curr_time);
rtc_err_t rtc_set_current_time_mock(const real_time_t* curr_time);
rtc_err_t rtc_get_startup_status_mock(void);
rtc_err_t rtc_set_alarm_mock(uint32_t timestamp, rtc_alarm_cb_t cb, void *cb_arg);

#endif /* RTC_MOCK_H_ */
