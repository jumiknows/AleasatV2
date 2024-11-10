/**
 * @file obc_rtc.c
 * @brief Top-level functions for getting the time, setting alarms, and dealing with either
 * the RTC or the mocked RTC.
 *
 * @author Andrada Zoltan, Richard Arthurs
 * @date January 26, 2020
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
 *  	Changes: removed support for old RTC
 *  	         changed API to support real time instead of epoch
 * 
 *  Edited on: Feburary 15, 2020
 *      Author: Karan Nedungadi
 *      Changes: removed dual RTC functionality
 *               removed any support to rtc_b
 *
 *  Edited on: February 16, 2020
 *  	Author: Richard Arthurs
 *  	Changes: added phase 1.1 support for dual RTCs.
 */

#include "obc_rtc.h"
#include "obc_time.h"
#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"
#include "backup_epoch.h"
#include "rtc_mock.h"
#include "gio.h"
#include "printf.h"
#include "filesystem.h"
#include "logger.h"
#include "rtc_state.h"
#include "rtos_semphr.h"
#ifdef PLATFORM_ORCA_V1
#include "rtc_pca2129.h"
#include "dual_rtc_pca2129.h"
#endif

/**
 * @brief Definition and storage for the RTC mutex.
 */
SemaphoreHandle_t rtc_mutex = NULL;
StaticSemaphore_t rtc_mutex_buf;

real_time_t rtc_latest_time;
rtc_err_t rtc_latest_state;

void rtc_create_infra(void) {
    rtc_mutex = xSemaphoreCreateRecursiveMutexStatic(&rtc_mutex_buf);
}

void rtc_init_backup(void) {
    init_rtc_state_backup();
    backup_epoch_init();
}

/**
 * @brief Initializes the RTCs.
 *
 * This starts the RTC mock, or sets and checks the configuration for a hardware RTC, depending on
 * platform. It also creates the RTC mutex.
 *
 * Errors during initialization are logged.
 */
void rtc_init(void) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        init_rtc_state_hw();
        rtc_latest_state = RTC_NO_SAVED_TIME;

#if RTC_MOCK_EN
        rtc_init_mock();
#else
#ifdef PLATFORM_ORCA_V1
        init_both_rtc();
#endif
#endif
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        while (1) {
        };
    }
}

/**
 * @brief Reads the time from RTC in ORCASat epoch format.
 *
 * If a comms error occurs, it is logged. If the time cannot be converted to
 * epoch, or a comms error prevents an epoch from being calculated, the epoch
 * returned will be -1.
 *
 * @return -1 if the time cannot be converted. Otherwise, the epoch.
 */
epoch_t rtc_get_epoch_time(void) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        rtc_err_t err         = RTC_NO_ERR;
        real_time_t curr_time = orca_time_init;

        err = rtc_get_current_time(&curr_time);

        if (err != RTC_NO_ERR) {
            log_str(ERROR, GET_EPOCH_CMD, true, "Cannot read time. Err: %d", err);
        }

        xSemaphoreGiveRecursive(rtc_mutex);
        return curr_time.epoch;
    } else {
        /**
         * TODO: handle this error more elegantly. Same goes for other similar cases.
         * - Forever looping would eventually cause the watchdog to reset the system. If
         *   this is what we want, should we attempt to log some sort of data about why we reset?
         *
         * - We could also return the backup epoch, which should be safely readable atomically
         * regardless of whether or not another task has the mutex. However, failure to take this
         * mutex may indicate a larger problem, in which case it's probably better to reset.
         */
        while (1) {
        };
    }
}

/**
 * @brief Gets the saved time in ORCASat epoch format.
 *
 * If epoch value was previously successfully saved, saved epoch is returned.
 * Otherwise, RTC is is accessed to calculate epoch.
 *
 * @return -1 if the time cannot be converted. Otherwise, the epoch.
 */
epoch_t get_epoch_time(void) {
    if ((rtc_latest_state != RTC_NO_ERR) || (rtc_latest_time.epoch == no_epoch)) {
        return rtc_get_epoch_time();
    } else {
        return rtc_latest_time.epoch;
    }
}

/**
 * @brief Gets the current real time from the RTC.
 *
 * @param[out] The time, valid if RTC_NO_ERR is returned;
 * @return RTC_NO_ERR if successful, error code otherwise.
 */
rtc_err_t rtc_get_current_time(real_time_t* curr_time) {
    rtc_err_t err = RTC_NO_ERR;

    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
#if RTC_MOCK_EN
        err = rtc_get_current_time_mock(curr_time);
#else
#ifdef PLATFORM_ORCA_V1
        err = get_dual_time(curr_time);
#endif
#endif
        rtc_latest_state = err;
        if (err == RTC_NO_ERR) {
            rtc_latest_time = *curr_time;
        }

        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        while (1) {
        };
    }

    return err;
}

/**
 * @brief Gets the latest saved RTC time.
 *
 * @param[out] The time
 * @return RTC_NO_ERR if successful, error code otherwise.
 */
rtc_err_t get_current_time(real_time_t* curr_time) {
    if (rtc_latest_state != RTC_NO_ERR) {
        rtc_get_current_time(curr_time);
    } else {
        *curr_time = rtc_latest_time;
    }
    return rtc_latest_state;
}

/**
 * @brief Set the current real time on desired RTCs.
 *
 * @pre The real time must have been validated prior to passing to this function.
 *
 * @param[in] curr_time:  time to set to. The real_time component will be used, not the epoch.
 * @param[in] rtcs: The hardware RTCs to update. Only RTC_A, RTC_B, or BOTH are valid values here. A
 * value of BACKUP will be ignored. Whether or not to set the backup epoch is controlled with the
 * @ref update_backup parameter.
 * @param[in] update_backup: Whether or not to update the backup epoch.
 * @return RTC_NO_ERR if successful, error code otherwise.
 */
rtc_err_t rtc_set_current_time(real_time_t* curr_time, active_rtc_t rtcs, bool update_backup) {
    rtc_err_t err = RTC_NO_ERR;

    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        if (update_backup) {
            set_backup_epoch(curr_time);
        }

#if RTC_MOCK_EN
        err = rtc_set_current_time_mock(curr_time);
#else
#ifdef PLATFORM_ORCA_V1
        err = set_dual_time(curr_time, rtcs);
/**
 * TODO: rerun scheduler so that it can purge any actions that are no longer valid.
 * Actions that are invalid due to a changed time should be logged.
 */
#endif
#endif
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        err = RTC_MUTEX_GRAB_ERR;
    }
    return err;
}

/**
 * @brief Set active RTCs. This controls which RTCs are actually communicated with for time
 * requests.
 *
 * This is done while holding @ref rtc_mutex so that the active rtc cannot be changed during another
 * task's get time request. Doing so could throw off the RTC selection logic in dual_rtc_pca2129.h.
 *
 * @param rtc The RTCs to set as active.
 * @return RTC_NO_ERR or RTC_MUTEX_GRAB_ERR
 */
rtc_err_t switch_active_rtc(active_rtc_t rtc) {
    rtc_err_t err = RTC_NO_ERR;

    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        set_state_active_rtc(rtc);
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        err = RTC_MUTEX_GRAB_ERR;
    }
    return err;
}

/**
 * @brief Sets an alarm on RTC A to activate some seconds in the future.
 *
 * @param[in] seconds_to_alarm Within how many seconds should the alarm trigger
 * @return RTC_NO_ERR if successful, error code otherwise.
 */
rtc_err_t rtc_set_relative_alarm(uint32_t seconds_to_alarm) {
    rtc_err_t err = RTC_NO_ERR;

#ifdef PLATFORM_ORCA_V1
    err = pca2129_setup_relative_alarm(&rtc_a, seconds_to_alarm);
#endif
    return err;
}

/**
 * rtc_set_absolute_alarm
 *
 * @brief Sets an alarm on RTC A to activate at a pre-defined time.
 *
 * @param[in] alarm_time The real time that the alarm should trigger. Real time fields are used, not
 * epoch.
 * @return RTC_NO_ERR if successful, error code otherwise.
 */
rtc_err_t rtc_set_absolute_alarm(real_time_t alarm_time) {
    rtc_err_t err = RTC_NO_ERR;
#ifdef PLATFORM_ORCA_V1
    err = pca2129_setup_absolute_alarm(&rtc_a, alarm_time);
#endif
    return err;
}

/**
 * @brief Set the timestamp mode of the RTC.
 *
 * This needs to be done while holding the RTC mutex so that timestamp logic will not
 * change part way through an existing request to query the time.
 *
 * @param[in] use_ts Whether or not to use the timestamp mode. True = timestamp is used, False =
 * register mode is used.
 */
void rtc_set_ts(bool use_ts) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        set_ts(use_ts);
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        while (1) {
        };
    }
}
