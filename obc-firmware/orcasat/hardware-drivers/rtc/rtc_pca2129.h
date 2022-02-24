/*
 * rtc_pca2129.h
 *
 * Created on: Jan 19, 2019
 *      Author: Julie Belleville
 *
 * Edited on: May 19, 2019
 * 		Author: Andrada Zoltan
 * 		Changes: - changed API to use real time and not epoch
 * 		         - added support to specify which RTC to use
 */

#ifndef RTC_PCA2129_H_
#define RTC_PCA2129_H_

#include "FreeRTOS.h"
#include "rtos_semphr.h"
#include "gio.h"
#include "obc_rtc.h"
#include "logger.h"

/**
 * @brief Settings for the timestamp feature.
 *
 * Timestamp is recorded when the pin goes low.
 */
#define PCA2129_TIMESTAMP_HOLD    0U
#define PCA2129_TIMESTAMP_RELEASE 1U

// RTC struct
typedef struct {
    /**
     * @brief The previous epoch read from this RTC. Used to determine that the RTC is still
     * incrementing time.
     */
    epoch_t prev_rtc_epoch;
    /**
     * @brief The time recorded at startup of the RTC.
     */
    real_time_t start_time;
    /**
     * @brief Pointer to the chip select GIO used for this RTC.
     */
    gioPORT_t* cs_port;
    /**
     * @brief Pin number for the chip select used for this RTC. Output (active low) from TMS570.
     */
    uint32_t cs_pin;
    /**
     * @brief Pointer to the alarm GIO used for this RTC.
     */
    gioPORT_t* alarm_port;
    /**
     * @bried The alarm pin number used for this RTC. Input to TMS570.
     */
    uint32_t alarm_pin;
    /**
     * @brief The name of the RTC, used for logging messages.
     */
    const char* name;
    /**
     * @brief The log ID to use when logging most messages for this RTC.
     */
    log_identifier_t log_id;
    /**
     * @brief The most recent error logged by this RTC in this module.
     */
    rtc_err_t most_recent_low_level_err;

} rtc_t;

/**
 * @brief RTC interrupt sources. These sources are queried from the interrupt
 * status register when RTC interrupts are raised.
 */
typedef enum rtc_int_sources { UNKNOWN_TRIGGER_INT = 1, ALARM_INT = 2, SECOND_INT = 3, TIMESTAMP_GROUND_INT = 4, TIMESTAMP_EDGE_INT = 5, SWITCH_OVER_BATTERY_INT = 6, BATTERY_LOW_INT = 7 } rtc_irq_t;

/**
 * @brief RTC A and B structures. These maintain the state of each RTC.
 */
extern rtc_t rtc_a;
extern rtc_t rtc_b;

// Initialization and time
rtc_err_t pca2129_init(rtc_t* rtc);
rtc_err_t pca2129_get_current_time(rtc_t* rtc, real_time_t* curr_time);
rtc_err_t pca2129_set_current_time(rtc_t* rtc, real_time_t* curr_time);
rtc_err_t pca2129_get_timestamp(rtc_t* rtc, real_time_t* curr_time);
void pca2129_timestamp_set(uint32_t val);

// Alarm
rtc_err_t pca2129_setup_relative_alarm(rtc_t* rtc, uint32_t seconds_to_alarm);
rtc_err_t pca2129_setup_absolute_alarm(rtc_t* rtc, real_time_t alarm_time);
rtc_err_t pca2129_enable_alarm(rtc_t* rtc);

// Misc.
rtc_irq_t pca2129_handle_interrupts(rtc_t* rtc);
rtc_err_t pca2129_get_startup_status(rtc_t* rtc);

#endif /* RTC_PCA2129_H_ */
