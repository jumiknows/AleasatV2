/**
 * @file obc_rtc.h
 * @brief Top-level RTC API
*/

#ifndef OBC_RTC_H_
#define OBC_RTC_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_time.h"

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Return codes for RTC functions.
 */
typedef enum rtc_errors {

    /**
     * @brief Returned when the HW RTC is re-configured during an
     * OBC startup.
     */
    RTC_CONFIG_SUCCESS = 1,

    /**
     * @brief Returned upon success of most RTC functions.
     */
    RTC_SUCCESS = 0,

    /**
     * @brief RTC cannot acquire a mutex.
     */
    RTC_MUTEX_GRAB_ERR = -1,

    /**
     * @brief RTC communications experienced a MIBSPI error.
     */
    RTC_MIBSPI_ERR = -2,

    /**
     * @brief RTC could not set a GPIO pin.
     */
    RTC_GPIO_WRITE_ERR = -3,

    /**
     * @brief A byte representing a bit has a value that is not 0x00 or 0x01.
     */
    RTC_BIT_WRITE_ERR = -4,

    /**
     * @brief Could not read time, or could not convert time read to epoch.
     */
    RTC_EPOCH_ERR = -5,

    /**
     * @brief Indicates that the scheduled alarm time is invalid.
     */
    RTC_ALARM_INVALID = -6

} rtc_err_t;

/**
 * @brief Function pointer type for registering RTC alarm callbacks.
*/
typedef void (*rtc_alarm_cb_t)(void *arg);

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// Initialization steps
void rtc_pre_init(void);
void rtc_init(void);
void rtc_init_backup(void);

// General application API
epoch_t rtc_get_epoch_time(void);
rtc_err_t rtc_get_current_time(real_time_t *curr_time);

// RTC configuration
rtc_err_t rtc_set_current_time(real_time_t *curr_time);

// RTC alarm
rtc_err_t rtc_set_absolute_alarm(uint32_t timestamp, rtc_alarm_cb_t cb, void *cb_arg);
void rtc_alarm_isr(void);

// RTC timestamp
rtc_err_t rtc_capture_timestamp(real_time_t *ts);

#endif /* OBC_RTC_H_ */
