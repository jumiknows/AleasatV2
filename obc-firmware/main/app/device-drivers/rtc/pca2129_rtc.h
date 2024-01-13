/**
 * @file pca2129_rtc.h
 * @brief Defines for the PCA2129 RTC driver
*/

#ifndef PCA2129_RTC_H_
#define PCA2129_RTC_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "tms_mibspi.h"
#include "obc_time.h"
#include "obc_rtc.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

rtc_err_t pca2129_rtc_init(void);

rtc_err_t pca2129_get_current_time(real_time_t* curr_time);
rtc_err_t pca2129_set_current_time(const real_time_t* curr_time);
rtc_err_t pca2129_set_absolute_alarm(uint8_t second, uint8_t minute, uint8_t hour, uint8_t day);
void pca2129_clear_alarm(void);
rtc_err_t pca2129_capture_timestamp(void);
rtc_err_t pca2129_read_timestamp(real_time_t* ts);

rtc_err_t pca2129_debug_read_single_register(const uint8_t reg, uint8_t *val);
rtc_err_t pca2129_debug_write_single_register(const uint8_t reg, const uint8_t *val);


#endif /* PCA2129_RTC_H_ */
