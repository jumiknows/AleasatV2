/**
 * @file dual_rtc_pca2129.c
 * @brief Handlers for dual RTCs.
 * @author Richard Arthurs
 * @date February 9, 2020
 *
 * See @ref dual_rtc_pca2129.h for more information.
 */

#include "obc_error.h"
#include "backup_epoch.h"
#include "rtc_pca2129.h"
#include "dual_rtc_pca2129.h"
#include "rtc_state.h"
#include "rtos.h"
#include "obc_rtc.h"

// Private function prototypes
static void get_time_backup(real_time_t* real_time);
static void get_time_regs(rtc_err_t* err_a, rtc_err_t* err_b, real_time_t* time_a, real_time_t* time_b);
static void get_time_timestamp(rtc_err_t* err_a, rtc_err_t* err_b, real_time_t* time_a, real_time_t* time_b);
static active_rtc_t determine_valid_time_source(rtc_err_t err_a, rtc_err_t err_b, real_time_t* time_a, real_time_t* time_b);
static bool is_incrementing(rtc_t* rtc, real_time_t* time);

/**
 * @brief Initializes both RTCs and sets their initial time.
 * Errors encountered during initialization are logged.
 *
 * @pre @ref init_rtc_state() has been called.
 */
void init_both_rtc(void) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        set_state_active_rtc(BOTH);

        rtc_err_t err_a = pca2129_init(&rtc_a);
        rtc_err_t err_b = pca2129_init(&rtc_b);

        /* Set the time to January 1, 2020 12:00:00 (12 PM)
         *
         * TODO: if this fails, there will be an invalid time which may
         * cause trouble when we try to log the failure.
         *
         * Perhaps the init status should be tracked in the rtc_state with
         * fields such as rtc_a_init_success.
         */
        real_time_t init_time = orca_time_init;
        set_dual_time(&init_time, RTC_A);
        if (rtc_a.most_recent_low_level_err != RTC_NO_ERR) {
            log_str(ERROR, rtc_a.log_id, true, "RTC %s ERR: %d", rtc_a.name, rtc_a.most_recent_low_level_err);
        }

        set_dual_time(&init_time, RTC_B);
        if (rtc_b.most_recent_low_level_err != RTC_NO_ERR) {
            log_str(ERROR, rtc_b.log_id, true, "RTC %s ERR: %d", rtc_b.name, rtc_b.most_recent_low_level_err);
        }

        /* Now that RTCs are initialized, errors can be logged. Logging requires
         * both RTCs to be initialized, as log messages include a timestamp.
         */
        if (err_a != RTC_NO_ERR) {
            log_str(ERROR, rtc_a.log_id, true, "RTC A ERR: %d", err_a);
        } else {
            log_str(INFO, rtc_a.log_id, false, "RTC A init OK");
        }

        if (err_b != RTC_NO_ERR) {
            log_str(ERROR, rtc_b.log_id, true, "RTC B ERR: %d", err_b);
        } else {
            log_str(INFO, rtc_b.log_id, false, "RTC B init OK");
        }
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        while (1) {
        }
    }
}

/**
 * @brief Gets the timestamp or the time reg values from both RTCs and sets the time output.
 *
 * @param[out] time The time value to use. Reliable if RTC_NO_ERR is returned.
 * @return RTC_NO_ERR or an error code.
 */
rtc_err_t get_dual_time(real_time_t* time) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        // Both RTCs start as DISABLED and are updated by the low-level driver with success or
        // MIBSPI error if appropriate.
        rtc_err_t err_a    = RTC_DISABLED;
        rtc_err_t err_b    = RTC_DISABLED;
        real_time_t time_a = orca_time_init;
        real_time_t time_b = orca_time_init;

        // Get timestamp or time based on the configuration
        if (using_timestamp()) {
            get_time_timestamp(&err_a, &err_b, &time_a, &time_b);
        } else {
            get_time_regs(&err_a, &err_b, &time_a, &time_b);
        }

        // Check for errors and compare times to determine which time source to use.
        active_rtc_t time_to_use = determine_valid_time_source(err_a, err_b, &time_a, &time_b);
        set_state_used_rtc(time_to_use);

        /* TODO: log errors if they occurred. If the time is not incrementing, perhaps
         * determine_valid_time_source() should modify the error state. Or maybe MIBSPI
         * and non-incrementing should both be logged since if there is a MIBSPI error,
         * it is likely that the time is not incrementing as well.
         */

        // Copy the time from the desired RTC into the output
        switch (time_to_use) {
            case RTC_A:
                memcpy(time, &time_a, sizeof(real_time_t));
                break;
            case RTC_B:
                memcpy(time, &time_b, sizeof(real_time_t));
                break;
            case BOTH:
            case BACKUP:
                /* BOTH is an invalid state at this point and we should never reach it. This is
                 * because determine_valid_time_source will return RTC_A if both RTCs are operating
                 * properly, since we can't accept the results from two RTCs at the same time. If we
                 * somehow get BOTH, use the backup because something has gone very wrong.
                 */
                get_time_backup(time);

                /* If the backup epoch is not the active time source, increment the override counter
                 * to indicate that something went wrong.
                 */
                if (get_active_rtc() != BACKUP) {
                    rtc_overridden_with_backup();
                }

                break;
            default:
                // We should never end up here, but if we do, use the backup epoch.
                get_time_backup(time);
                rtc_overridden_with_backup();
                // TODO: log some sort of critical error
                break;
        }
        xSemaphoreGiveRecursive(rtc_mutex);
        return RTC_NO_ERR;
    } else {
        return RTC_MUTEX_GRAB_ERR;
    }
}

/**
 * @brief Set the time on RTCs. Only RTC_A and RTC_B are considered. The backup epoch should
 * be set separately (but probably at the same time) as this function is called.
 *
 * @pre The @ref time has been validated using @ref is_real_time_valid().
 *
 * @param[in] time The real time to set. Only the real time fields will be used, not the epoch.
 * @param[in] rtcs The RTCs to change the time on. This _should_ equal active RTCs in most cases,
 * but does not _need_ to equal the active RTCs. This function will update any requested RTCs,
 * regardless of whether or not they are active.
 * @return RTC_NO_ERR or the lowest numerical error value returned from either RTC.
 */
rtc_err_t set_dual_time(real_time_t* time, active_rtc_t rtcs) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        rtc_err_t err_a = RTC_NO_ERR;
        rtc_err_t err_b = RTC_NO_ERR;

        if ((rtcs & RTC_A) == RTC_A) {
            err_a = pca2129_set_current_time(&rtc_a, time);
        }

        if ((rtcs & RTC_B) == RTC_B) {
            err_b = pca2129_set_current_time(&rtc_b, time);
        }

        // If there is a mismatch between the errors, return the lowest (most severe) error code.
        if (err_a != err_b) {
            xSemaphoreGiveRecursive(rtc_mutex);
            return (err_a < err_b) ? err_a : err_b;
        }
        xSemaphoreGiveRecursive(rtc_mutex);
        return err_a; // If errors matched, return err_a
    } else {
        return RTC_MUTEX_GRAB_ERR;
    }
}

// Private functions --------------------

/**
 * @brief Gets the time from the backup epoch.
 *
 * @param[in] real_time The real time to populate with the timestamp from the backup epoch counter.
 */
static void get_time_backup(real_time_t* real_time) {
    epoch_t backup = get_backup_epoch();
    epoch_to_real_time(backup, real_time);
}

/**
 * @brief Gets the time from both RTCs using the time registers.
 *
 * @param[out] err_a The RTC errors registered on RTC A.
 * @param[out] err_b The RTC errors registered on RTC B.
 * @param[out] time_a The time from RTC A.
 * @param[out] time_b The time from RTC B.
 */
static void get_time_regs(rtc_err_t* err_a, rtc_err_t* err_b, real_time_t* time_a, real_time_t* time_b) {
    // Read from enabled RTCs only
    if ((get_active_rtc() & RTC_A) == RTC_A) {
        *err_a = pca2129_get_current_time(&rtc_a, time_a);
    }

    if ((get_active_rtc() & RTC_B) == RTC_B) {
        *err_b = pca2129_get_current_time(&rtc_b, time_b);
    }
}

/**
 * @brief Gets the time from both RTCs using the timestamp feature.
 *
 * @param[out] err_a The RTC errors registered on RTC A.
 * @param[out] err_b The RTC errors registered on RTC B.
 * @param[out] time_a The time from RTC A.
 * @param[out] time_b The time from RTC B.
 */
static void get_time_timestamp(rtc_err_t* err_a, rtc_err_t* err_b, real_time_t* time_a, real_time_t* time_b) {
    // RA: with immediate hold and release, it's low for 961 ns consistently
    pca2129_timestamp_set(PCA2129_TIMESTAMP_HOLD);    // Lock the timestamp in both RTCs
    pca2129_timestamp_set(PCA2129_TIMESTAMP_RELEASE); // Unlock the timestamp in both RTCs

    real_time_t ts_a = orca_time_init;
    real_time_t ts_b = orca_time_init;

    // Read from enabled RTCs only
    if ((get_active_rtc() & RTC_A) == RTC_A) {
        *err_a = pca2129_get_timestamp(&rtc_a, &ts_a);
        memcpy(time_a, &ts_a, sizeof(real_time_t));
    }

    if ((get_active_rtc() & RTC_B) == RTC_B) {
        *err_b = pca2129_get_timestamp(&rtc_b, &ts_b);
        memcpy(time_b, &ts_b, sizeof(real_time_t));
    }
}

/**
 * @brief Checks the errors and times from both RTCs to determine which timestamp should be used.
 *
 * Side effects:
 * 	- Invalidates the epoch on RTCs that experienced an error from the RTC driver, such as a MIBSPI
 * error.
 * 	- Sets the prev_rtc_epoch field in both RTCs.
 *
 * @param[in] err_a The RTC error to associate with time A.
 * @param[in] err_b The RTC error to associate with time B.
 * @param[in] time_a The time from RTC A to compare.
 * @param[in] time_b The time from RTC B to compare.
 *
 * @return RTC_A, RTC_B, or BACKUP. The time source to use to report the time.
 */
static active_rtc_t determine_valid_time_source(rtc_err_t err_a, rtc_err_t err_b, real_time_t* time_a, real_time_t* time_b) {
    /* Determine which RTCs are healthy. An RTC that is valid to use for a time reading is one that
     * both has no MIBSPI errors from the low-level driver, and one that is correctly incrementing
     * its time.
     *
     * Check both RTCs and assemble the flags to determine which RTCs are valid to use for the time,
     * since if both RTCs are operating properly we just accept the value from RTC A.
     */
    uint8_t a_ok            = (is_incrementing(&rtc_a, time_a) == true) && (err_a == RTC_NO_ERR);
    uint8_t b_ok            = (is_incrementing(&rtc_b, time_b) == true) && (err_b == RTC_NO_ERR);
    active_rtc_t valid_rtcs = (active_rtc_t)(a_ok | (uint8_t)(b_ok << 1));

    active_rtc_t time_to_use = BACKUP; // The eventual return value of this function: the time
                                       // source to use when reporting the timestamp.

    /* Ensure the epochs are invalidated on any RTCs that are not used. If both RTCs are okay,
     * check that RTC A and B agree, and if so, use RTC A.
     */
    switch (valid_rtcs) {
        case BACKUP:
            time_to_use = BACKUP;
            break;

        case RTC_A:
            time_b->epoch = no_epoch; // Invalidate RTC B epoch since only RTC A was good
            time_to_use   = RTC_A;
            break;

        case RTC_B:
            time_a->epoch = no_epoch; // Invalidate RTC A epoch since only RTC B was good
            time_to_use   = RTC_B;
            break;

        case BOTH:
            if (times_are_within(time_a->epoch, time_b->epoch, get_tolerance())) {
                // Both times are within tolerance, so use RTC A.
                time_to_use = RTC_A;
            } else {
                // Times are not within tolerance second :( use the backup.
                register_rtc_mismatch(abs(time_a->epoch - time_b->epoch));
                time_to_use = BACKUP;
            }
            break;

        default:
            time_to_use = BACKUP;
            break;
    }

    return time_to_use;
}

/**
 * @brief Indicates whether the RTC is incrementing properly.
 *
 * An RTC is considered to be incrementing if the epoch passed in is greater than or equal to
 * the previous epoch saved in the RTC. The previous epoch is always set in @ref
 * determine_valid_time_source().
 *
 * Side effect: this function also updates the previous epoch in the RTC structure.
 *
 * @param rtc[inout] The RTC to examine.
 * @param time[in] The real time to compare to. The epoch field will be examined.
 * @return True if the time is incrementing, False if there is an incrementation error.
 */
static bool is_incrementing(rtc_t* rtc, real_time_t* time) {
    // Return early if either epoch is invalid
    if ((rtc->prev_rtc_epoch <= no_epoch) || (time->epoch <= no_epoch)) {
        return false;
    }

    // Valid incrementing, update the previous time for this RTC
    if (time->epoch >= rtc->prev_rtc_epoch) {
        rtc->prev_rtc_epoch = time->epoch;
        return true;
    }

    return false; // RTC is not incrementing
}
