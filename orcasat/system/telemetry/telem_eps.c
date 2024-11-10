/**
 * @file telem_eps.c
 * @brief Telemetry collection functions for the EPS.
 * @author Richard A
 * @date October 31, 2019
 */

#include "orca_telemetry_defs.h"
#include "orca_telem_logger.h"
#include "telem_snapshot.h"
#include "telem_eps.h"
#include "sys_common.h"
#include "logger.h"
#include "scheduler.h"
#include "obc_rtc.h"
#include "obc_hardwaredefs.h"
#include "obc_adc.h"

// Private functions
static void eps_telem_check(void);

/**
 * @brief EPS telemetry collection function (slow rate).
 *
 * Logs telemetry for the EPS in the slow data bin.
 */
void eps_slow_telem_collect(void) {
    // Update ADC to read latest values
    if (!adc_update()) {
        return; // ADC Update issue. Do not save values
    }

    // Save ADC values
    EPS_SLOW.sense_v_bat = adc_get_channel_val(EPS_SENSE_V_BAT_ADC);
    EPS_SLOW.sense_i_bat = adc_get_channel_val(EPS_SENSE_I_BAT_ADC);

    // Record the timestamp
    EPS_SLOW_TS = get_epoch_time();

    // Check that everything is good
    eps_telem_check();

    // Issue the telemetry write request
    log_telem(EPS_SLOW_TELEM);
}

/**
 * @brief Checks EPS telemetry that values are within expected ranges.
 */
static void eps_telem_check(void) {
}
