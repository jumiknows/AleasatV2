/**
 * @file rtc_state.c
 * @brief Contains setters and getters for RTC state.
 * @author Richard A
 * @date February 16, 2020
 */

#include "rtc_state.h"
#include "logger.h"
#include "obc_rtc.h"

/**
 * @brief Tracks the state of the dual RTC handling.
 */
struct rtc_state {
    /**
     * @brief Indicates that RTC A/B were initialized successfully.
     */
    bool rtc_a_init_success;
    bool rtc_b_init_success;

    /**
     * @brief Indicates which RTC was actually being used for the most recent
     * timestamp request. If both RTCs are active and detected to be working,
     * this field will indicate RTC_A, as RTC A is the RTC that will be used
     * for timing in this case.
     *
     * If the backup epoch is active, or if RTC A is explicitly disabled, this
     * will read BACKUP or RTC B. It should never be BOTH, because we can't
     * actually use both RTCs to create a single timestamp.
     */
    active_rtc_t used_rtc;

    /**
     * @brief Indicates which RTCs are enabled. If an RTC is active, attempts will
     * be made to communicate with it, but its value may not be used in the timestamp.
     *
     * The RTC used to get the time is indicated by @ref used_rtc.
     */
    active_rtc_t active_rtc;

    /**
     * @brief Indicates whether the timestamp method is being used to get times
     * from the RTC.
     *
     * 0 = registers are being used.
     * 1 = timestamp being used.
     */
    bool timestamp;

    /**
     * @brief Allowed tolerance between RTC A and RTC B epoch values (seconds).
     *
     * If epochs from both RTCs are within this value, both RTCs will be considered
     * operational.
     */
    uint8_t time_tolerance_sec;

    /**
     * @brief Number of RTC A and B epoch mismatches recorded in timestamp mode.
     */
    uint32_t num_a_b_ts_mismatches;

    /**
     * @brief Number of RTC A and B epoch mismatches recorded in register mode.
     */
    uint32_t num_a_b_reg_mismatches;

    /**
     * @brief Average absolute mismatch between RTC A and B, when mismatch was above
     * the configured tolerance in timestamp mode.
     */
    float avg_mismatch_ts;

    /**
     * @brief Average absolute mismatch between RTC A and B, when mismatch was above
     * the configured tolerance in register mode.
     */
    float avg_mismatch_reg;

    /**
     * @brief Number of times the hardware RTCs were overridden by the backup epoch.
     */
    uint32_t backup_overrides;

    /**
     * NOTE: when adding fields to this structure, be sure to initialize them in
     * @ref init_rtc_state().
     */
} state;

/**
 * @brief RTC names for informational printing purposes.
 * @warning They must appear in the same order as the enum keys in @ref active_rtc_t so that
 * the enum value can be used to index this array.
 */
const char* time_source_names[4] = {"BACKUP", "A", "B", "BOTH"};

/**
 * @brief Initializes fields of the state structure.
 *
 * For proper initialization, all fields of the state struct must be initialized in this
 * function, and it must be called before any attempts are made to use the RTC driver.
 */
void init_rtc_state_backup(void) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(200))) {
        state.rtc_a_init_success = false;
        state.rtc_b_init_success = false;

        state.used_rtc               = BACKUP;
        state.active_rtc             = BACKUP;
        state.timestamp              = false;
        state.time_tolerance_sec     = 1;
        state.num_a_b_ts_mismatches  = 0;
        state.num_a_b_reg_mismatches = 0;
        state.avg_mismatch_ts        = 0;
        state.avg_mismatch_reg       = 0;
        state.backup_overrides       = 0;
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        while (1) {
        }
    }
}

void init_rtc_state_hw(void) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(200))) {
        state.rtc_a_init_success = false;
        state.rtc_b_init_success = false;

        state.used_rtc               = RTC_A; // Nominally, RTC_A will be the used RTC so start up this way.
        state.active_rtc             = BOTH;  // Both RTCs enabled by default
        state.timestamp              = false; // Set timestamp mode off, since it does strange things with set_time
        state.time_tolerance_sec     = 1;
        state.num_a_b_ts_mismatches  = 0;
        state.num_a_b_reg_mismatches = 0;
        state.avg_mismatch_ts        = 0;
        state.avg_mismatch_reg       = 0;
        state.backup_overrides       = 0;
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        while (1) {
        }
    }
}

/**
 * @brief Gets the active RTC.
 * @return The RTCs that are active and can therefore be used for time readings.
 */
active_rtc_t get_active_rtc(void) {
    return state.active_rtc;
}

/**
 * @brief Updates the RTCs that may be used for time requests.
 */
void set_state_active_rtc(active_rtc_t rtc) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        state.active_rtc = rtc;
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        while (1) {
        }
    }
}

/**
 * @brief Updates the used RTC in the state.
 */
void set_state_used_rtc(active_rtc_t rtc) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        state.used_rtc = rtc;
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        while (1) {
        }
    }
}

/**
 * @brief Change the timestamp mode.
 *
 * @param[in] use_ts Whether or not to use the timestamp mode. True = timestamp is used, False =
 * register mode is used.
 */
void set_ts(bool use_ts) {
    if (xSemaphoreTakeRecursive(rtc_mutex, pdMS_TO_TICKS(RTC_MUTEX_TIMEOUT_MS))) {
        state.timestamp = use_ts;
        xSemaphoreGiveRecursive(rtc_mutex);
    } else {
        while (1) {
        }
    }
}

/**
 * @brief Returns whether or not the timestamp feature should be used.
 * @return True if the timestamp should be used. False if time register should be used.
 */
bool using_timestamp(void) {
    return state.timestamp;
}

/**
 * @brief Determines if the active time source is the backup epoch.
 * @return True if the backup epoch is used. False if a hardware RTC is enabled.
 */
bool is_backup(void) {
    return (state.active_rtc == BACKUP);
}

/**
 * @brief Get the allowable tolerance between RTC A and B epoch values.
 * @return The tolerance (seconds).
 */
uint8_t get_tolerance(void) {
    return state.time_tolerance_sec;
}

/**
 * @brief Indicates that the hardware RTC use was overridden by the backup epoch. This
 * means we encountered some sort of RTC problem.
 */
void rtc_overridden_with_backup(void) {
    state.backup_overrides++;
}

/**
 * @brief Registers a time mismatch in the counter. Depending on the timestamp/register
 * mode of the RTC, the appropriate counter is incremented and the average mismatch
 * magnitude is updated.
 */
void register_rtc_mismatch(uint32_t mismatch_magnitude) {
    if (state.timestamp) {
        state.num_a_b_ts_mismatches++;
        state.avg_mismatch_ts = ((state.avg_mismatch_ts * (state.num_a_b_ts_mismatches - 1)) + mismatch_magnitude) / state.num_a_b_ts_mismatches;
    } else {
        state.num_a_b_reg_mismatches++;
        state.avg_mismatch_reg = ((state.avg_mismatch_reg * (state.num_a_b_reg_mismatches - 1)) + mismatch_magnitude) / state.num_a_b_reg_mismatches;
    }
}

/**
 * @brief Prints RTC information.
 */
void print_rtc_info(void) {
    log_str(INFO, LOG_RTC_GENERAL, true, "Active: %s", time_source_names[state.active_rtc]);
    log_str(INFO, LOG_RTC_GENERAL, true, "Last used: %s", time_source_names[state.used_rtc]);
    log_str(INFO, LOG_RTC_GENERAL, true, "Tolerance: %d", state.time_tolerance_sec);
    log_str(INFO, LOG_RTC_GENERAL, true, "Timestamp: %d", state.timestamp);
}

/**
 * @brief Prints RTC stats that are tracked.
 */
void print_rtc_stats(void) {
    log_str(INFO, LOG_RTC_GENERAL, true, "TS mismatch: %d", state.num_a_b_ts_mismatches);
    log_str(INFO, LOG_RTC_GENERAL, true, "TS mismatch avg: %f", state.avg_mismatch_ts);
    log_str(INFO, LOG_RTC_GENERAL, true, "REG mismatch: %d", state.num_a_b_reg_mismatches);
    log_str(INFO, LOG_RTC_GENERAL, true, "REG mismatch avg: %f", state.avg_mismatch_reg);
    log_str(INFO, LOG_RTC_GENERAL, true, "Overrides: %d", state.backup_overrides);
}

/**
 * @brief Returns a pointer to the string representation of the desired
 * RTC type.
 *
 * @param rtc The type to return a string for.
 */
const char* get_time_source_name(active_rtc_t rtc) {
    return time_source_names[rtc];
}
