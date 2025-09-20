/**
 * @file obc_rtc.c
 * @brief Top-level RTC API implementation
*/

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_rtc.h"
#include "obc_time.h"
#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"
#include "logger.h"

// RTC
#include "pca2129_rtc.h"
#include "backup_epoch.h"
#include "rtc_mock.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief RTC mutex grab timeout.
 */
#define RTC_MUTEX_TIMEOUT_MS 1500

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct rtc_alarm_isr_info {
    rtc_alarm_cb_t isr;
    void *args;
} rtc_alarm_isr_info_t;

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief Definition and storage for the RTC mutex.
 *
 * A recursive mutex is used to allow a single task to take the mutex multiple times. This is
 * required because internal RTC code calls the logger on errors, which needs a timestamp, which
 * requires the RTC.
 */
static SemaphoreHandle_t rtc_mutex = NULL;
static StaticSemaphore_t rtc_mutex_buf;

#if FEATURE_HW_RTC
static rtc_alarm_isr_info_t rtc_alarm_isr_info = {.isr = NULL, .args = NULL};
#endif

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Creates the RTC mutex.
 */
void rtc_pre_init(void) {
    rtc_mutex = xSemaphoreCreateRecursiveMutexStatic(&rtc_mutex_buf);
}

/**
 * @brief Initializes the RTC.
 *
 * This starts the RTC mock, or initializes the hardware RTC, depending on whether
 * hardware RTC feature is enabled.
 */
void rtc_init(void) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {

#if FEATURE_HW_RTC

        if (pca2129_rtc_init() == RTC_CONFIG_SUCCESS) {
            LOG_RTC__RECONFIGURED_IN_STARTUP();
        }

#else
        rtc_init_mock();
#endif
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        while (1) {
        };
    }
}

/**
 * @brief Initializes and starts the TMS570's RTI counter.
 */
void rtc_init_backup(void) {
    backup_epoch_init();
}

/**
 * @brief Interface to call alarm callback
*/
#if FEATURE_HW_RTC
void rtc_alarm_isr(void) {
    pca2129_clear_alarm();

    if (rtc_alarm_isr_info.isr) {
        (rtc_alarm_isr_info.isr)(rtc_alarm_isr_info.args);
    }

    return;
}
#endif

/**
 * @brief Reads the time from RTC in ALEASAT epoch format.
 *
 * If an error occurs in communicating with the RTC, it is logged. If the time
 * cannot be converted to epoch, or cannot be read from the RTC, RTC_EPOCH_ERR
 * will be returned instead of an epoch.
 *
 * @return RTC_EPOCH_ERR if the time cannot be read or converted. Otherwise, the epoch.
 */
epoch_t rtc_get_epoch_time(void) {
    real_time_t curr_time = alea_time_init;

    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        rtc_err_t err         = RTC_SUCCESS;

        err = rtc_get_current_time(&curr_time);

        if (err != RTC_SUCCESS) {
            LOG_RTC__CANNOT_READ_TIME();
        }

        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        LOG_RTC__CANNOT_READ_TIME();
        curr_time.epoch = no_epoch;
    }

    if (curr_time.epoch == no_epoch) {
        return RTC_EPOCH_ERR;
    }

    return curr_time.epoch;
}

/**
 * @brief Gets the current real time from the RTC.
 *
 * @param[out] The time, valid if RTC_SUCCESS is returned;
 * @return RTC_SUCCESS if successful, error code otherwise.
 */
rtc_err_t rtc_get_current_time(real_time_t *curr_time) {
    rtc_err_t err = RTC_SUCCESS;

    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
#if FEATURE_HW_RTC
        err = pca2129_get_current_time(curr_time);
        curr_time->epoch = real_time_to_epoch(curr_time);
#else
        err = rtc_get_current_time_mock(curr_time);
#endif

        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        err = RTC_MUTEX_GRAB_ERR;
    }

    return err;
}

/**
 * @brief Set the current real time on desired RTCs.
 *
 * @pre The real time must have been validated prior to passing to this function.
 *
 * @param[in] curr_time:  time to set to. The real_time component will be used, not the epoch.
 * @return RTC_SUCCESS if successful, error code otherwise.
 */
rtc_err_t rtc_set_current_time(real_time_t *curr_time) {
    rtc_err_t err = RTC_SUCCESS;

    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
#if FEATURE_HW_RTC
        err = pca2129_set_current_time(curr_time);
#else
        err = rtc_set_current_time_mock(curr_time);
#endif
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        err = RTC_MUTEX_GRAB_ERR;
    }

    return err;
}

/**
 * rtc_set_absolute_alarm
 *
 * @brief Sets an alarm to activate at a pre-defined time.
 *
 * @param[in] timestamp Epoch timestamp when the alarm should trigger
 * @param[in] cb        Callback function to invoke at the alarm time
 * @param[in] cb_arg    Argument to pass to the callback function
 *
 * @return RTC_SUCCESS if successful, error code otherwise.
 */
rtc_err_t rtc_set_absolute_alarm(uint32_t timestamp, rtc_alarm_cb_t cb, void *cb_arg) {
    rtc_err_t err = RTC_SUCCESS;
#if FEATURE_HW_RTC
    real_time_t alarm_time;

    if (!epoch_to_real_time((epoch_t) timestamp, &alarm_time) || !is_real_time_valid(&alarm_time)) {
        return RTC_ALARM_INVALID;
    }

    rtc_alarm_isr_info.isr = cb;
    rtc_alarm_isr_info.args = cb_arg;
    err = pca2129_set_absolute_alarm(alarm_time.second, alarm_time.minute, alarm_time.hour, alarm_time.day);
#else
    err = rtc_set_alarm_mock(timestamp, cb, cb_arg);
#endif
    return err;
}


/**
 * rtc_capture_timestamp
 *
 * @brief Saves current time as timestamp in the RTC and fills @ref ts with the captured timestamp.
 *
 * @param[out] ts The captured timestamp, valid if return value is RTC_SUCCESS.
 *
 * If the hardware RTC is not enabled, then we simply return the current time from the software mock RTC.
 *
 * @return RTC_SUCCESS if successful, error code otherwise.
 */
rtc_err_t rtc_capture_timestamp(real_time_t *ts) {
    rtc_err_t err = RTC_SUCCESS;

    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
#if FEATURE_HW_RTC
        err = pca2129_capture_timestamp();

        if (err != RTC_SUCCESS) {
            xSemaphoreGiveRecursive(rtc_mutex);
            return err;
        }

        err = pca2129_read_timestamp(ts);

        if (err != RTC_SUCCESS) {
            xSemaphoreGiveRecursive(rtc_mutex);
            return err;
        }

        ts->epoch = real_time_to_epoch(ts);
#else
        err = rtc_get_current_time_mock(ts);
#endif
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        err = RTC_MUTEX_GRAB_ERR;
    }

    return err;
}
