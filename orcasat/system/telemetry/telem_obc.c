/**
 * @file telem_obc.c
 * @brief OBC internal telemetry collection.
 * @author Richard A
 * @date October 31, 2019
 *
 * Internal OBC telemetry readings are status reports from the TMS570 and from devices on the OBC
 * itself.
 *
 * The internal OBC telemetry provides an example of how telemetry readings can be handled from
 * external subsystems.
 */

#include "orca_telemetry_defs.h"
#include "orca_telem_logger.h"
#include "telem_snapshot.h"
#include "telem_obc.h"
#include "sys_common.h"
#include "reg_tcram.h"
#include "logger.h"
#include "scheduler.h"
#include "obc_temperature.h"
#include "obc_rtc.h"

// Private Function Prototypes
static void obc_telem_check_slow(void);
static void obc_telem_check_fast(void);

static uint16_t ramoccur_1_obc_telem(void);
static uint16_t ramoccur_2_obc_telem(void);
static uint32_t rtos_minheap_obc_telem(void);

/**
 * @brief OBC telemetry collection function (slow rate).
 *
 * This function serves as a general example of what a telemetry logging function should look like,
 * while also performing the internal OBC telemetry logging.
 */
void obc_slow_telem_collect(void) {
    OBC_SLOW.minheap    = rtos_minheap_obc_telem();
    OBC_SLOW.ramoccur_1 = ramoccur_1_obc_telem();
    OBC_SLOW.ramoccur_2 = ramoccur_2_obc_telem();
    // TODO: other slow telemetry

    // Record the timestamp
    OBC_SLOW_TS = get_epoch_time();

    // Check that everything is good
    obc_telem_check_slow();

    // Issue the telemetry write request
    log_telem(OBC_SLOW_TELEM);
}

/**
 * @brief OBC telemetry collection function (fast rate).
 *
 * Reads OBC fast rate telemetry values.
 */
void obc_fast_telem_collect(void) {
    // Read the temperature
    temp_err_t temp_err = read_temp(&OBC_FAST.obc_temperature_c);
    if (temp_err != TEMP_SUCCESS) {
        log_str(ERROR, TEMP_LOG, true, "Temp IO err: %d", temp_err);
    }

    // TODO: current readings

    OBC_FAST_TS = get_epoch_time();
    obc_telem_check_fast();
    log_telem(OBC_FAST_TELEM);
}

//-------- Private Functions -----------

/**
 * @brief Check OBC slow telemetry bin.
 *
 * This checker function examines the OBC slow section of the telemetry snapshot and logs errors if
 * telemetry values are out of range.
 */
static void obc_telem_check_slow(void) {
    if (OBC_SLOW.minheap < 2000) {
        log_str(INFO, SYS_LOG, true, "OBC minheap %d", OBC_SLOW.minheap);
    } else if (OBC_SLOW.minheap == 0) {
        log_str(ERROR, SYS_LOG, true, "OBC minheap %d", OBC_SLOW.minheap);
    } else {
        // Heap is fine, do nothing.
    }
}

/**
 * @brief Check OBC fast telemetry bin.
 *
 * This checker function examines the OBC fast section of the telemetry snapshot and logs errors if
 * telemetry values are out of range.
 */
static void obc_telem_check_fast(void) {
    if ((OBC_FAST.obc_temperature_c > 30) || (OBC_FAST.obc_temperature_c < 20)) {
        log_str(ERROR, TEMP_LOG, true, "OBC temp %d out of range", OBC_FAST.obc_temperature_c);
    }
}

//-------- Telemetry Collection Functions -----------

/**
 * @brief RAMOCCUR counter for corrected single-bit errors in TCRAM (0714 TRM section 6.3.1, pg.
 * 307)
 * @return The number of single-bit corrected errors in RAMOCCUR.
 */
static uint16_t ramoccur_1_obc_telem(void) {
    return tcram1REG->RAMOCCUR;
}

/**
 * @brief RAMOCCUR counter for corrected single-bit errors in TCRAM (0714 TRM section 6.3.1, pg.
 * 307)
 * @return The number of single-bit corrected errors in RAMOCCUR.
 */
static uint16_t ramoccur_2_obc_telem(void) {
    return tcram2REG->RAMOCCUR;
}

/**
 * @brief Gets the minimum heap available to the RTOS during the lifecycle of the application (time
 * since boot).
 * @return Minimum free heap (bytes).
 */
static uint32_t rtos_minheap_obc_telem(void) {
    return xPortGetMinimumEverFreeHeapSize();
}
