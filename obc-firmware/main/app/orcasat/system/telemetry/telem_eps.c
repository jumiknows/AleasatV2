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
#include "obc_rtc.h"
#include "obc_hardwaredefs.h"

// Private functions
static void eps_telem_check_slow(void);

static uint8_t battery_bus_enabled;

/**
 * @brief EPS telemetry collection function (normal rate).
 *
 * Logs telemetry for the EPS in the normal data bin.
 */
void eps_normal_telem_collect(void) {
    EPS_NORMAL.sense_v_bat = 0xFFFF;
    EPS_NORMAL.sense_i_bat = 0xFFFF;

    EPS_NORMAL.sense_v_bcr = 0xFFFF;
    EPS_NORMAL.sense_i_bcr = 0xFFFF;

    EPS_NORMAL.sense_i_bus_3v3 = 0xFFFF;
    EPS_NORMAL.sense_i_bus_5v5 = 0xFFFF;

    EPS_NORMAL.fs_bat_chg_1 = 0xFFFF & (0b1 << 3);
    EPS_NORMAL.fs_bat_chg_2 = 0xFFFF & (0b1 << 4);

    EPS_NORMAL.out_1 = 0xFFFF & (0b1 << 7);
    EPS_NORMAL.out_2 = 0xFFFF & (0b1 << 8);
    EPS_NORMAL.out_3 = 0xFFFF & (0b1 << 9);
    EPS_NORMAL.out_4 = 0xFFFF & (0b1 << 10);
    EPS_NORMAL.out_5 = 0xFFFF & (0b1 << 11);
    EPS_NORMAL.out_6 = 0xFFFF & (0b1 << 12);

    EPS_NORMAL.pwr_cyc     = 0x0000;
    EPS_NORMAL.v_under_v   = 0x0000;
    EPS_NORMAL.v_short     = 0x0000;
    EPS_NORMAL.v_over_temp = 0x0000;
    EPS_NORMAL.charge_cyc  = 0x0000;

    EPS_NORMAL.def_1 = 0xFFFF;
    EPS_NORMAL.def_2 = 0xFFFF;

    EPS_NORMAL.shutdn_bat_chg = 0xFFFF & (0b1 << 2);

    // Record the timestamp
    EPS_NORMAL_TS = get_epoch_time();

    // Issue the telemetry write request
    log_telem(EPS_NORMAL_TELEM);
}

/**
 * @brief EPS telemetry collection function (fast rate).
 *
 * Logs telemetry for the EPS in the fast data bin.
 */
void eps_fast_telem_collect(void) {
    EPS_FAST.sense_v_x     = 0xFFFF;
    EPS_FAST.sense_i_x_n_a = 0xFFFF;
    EPS_FAST.sense_i_x_p_a = 0xFFFF;

    EPS_FAST.sense_v_y     = 0xFFFF;
    EPS_FAST.sense_i_y_n_a = 0xFFFF;
    EPS_FAST.sense_i_y_p_a = 0xFFFF;

    EPS_FAST.sense_v_z     = 0xFFFF;
    EPS_FAST.sense_i_z_n_a = 0xFFFF;
    EPS_FAST.sense_i_z_p_a = 0xFFFF;

    EPS_FAST.cond_in    = 0xFFFF;
    EPS_FAST.cond_out_1 = 0xFFFF;
    EPS_FAST.cond_out_2 = 0xFFFF;

    // Record the timestamp
    EPS_FAST_TS = get_epoch_time();

    // Issue the telemetry write request
    log_telem(EPS_FAST_TELEM);
}

/**
 * @brief EPS telemetry collection function (slow rate).
 *
 * Logs telemetry for the EPS in the slow data bin.
 */
void eps_slow_telem_collect(void) {
    EPS_SLOW.sense_temp_mcu = 0xFFFF;

    EPS_SLOW.sense_temp_bat_1 = 0xFFFF;
    EPS_SLOW.sense_temp_bat_2 = 0xFFFF;
    EPS_SLOW.sense_temp_bat_3 = 0xFFFF;
    EPS_SLOW.sense_temp_bat_4 = 0xFFFF;

    EPS_SLOW.max_temp_1 = 0xFFFF;
    EPS_SLOW.max_temp_2 = 0xFFFF;
    EPS_SLOW.max_temp_3 = 0xFFFF;
    EPS_SLOW.max_temp_4 = 0xFFFF;

    EPS_SLOW.min_temp_1 = 0xFFFF;
    EPS_SLOW.min_temp_2 = 0xFFFF;
    EPS_SLOW.min_temp_3 = 0xFFFF;
    EPS_SLOW.min_temp_4 = 0xFFFF;

    EPS_SLOW.sense_temp_ext_1 = 0xFFFF;
    EPS_SLOW.sense_temp_ext_2 = 0xFFFF;
    EPS_SLOW.sense_temp_ext_3 = 0xFFFF;

    EPS_SLOW.bat_bcr_en = 0xFFFF & (0b1 << 2);

    EPS_SLOW.bus_3v3_en = 0xFFFF & (0b1 << 3);
    EPS_SLOW.bus_5v_en  = 0xFFFF & (0b1 << 4);

    EPS_SLOW.heat_1 = (uint32_t)0xFFFF & (0b1 << 13);
    EPS_SLOW.heat_2 = (uint32_t)0xFFFF & (0b1 << 14);
    EPS_SLOW.heat_3 = (uint32_t)0xFFFF & (0b1 << 15);

    // Record the timestamp
    EPS_SLOW_TS = get_epoch_time();

    // Check that everything is good
    eps_telem_check_slow();

    // Issue the telemetry write request
    log_telem(EPS_SLOW_TELEM);
}

/**
 * @brief EPS telemetry collection function (slow rate).
 *
 * Logs telemetry for the EPS in the condn (i.e. conditional) data bin.
 */
void eps_condn_telem_collect(void) {
    // Record the timestamp
    EPS_CONDN_TS = get_epoch_time();

    if (EPS_CONDN_TS == 0)
        EPS_CONDN.sw_ver = 0xFFFF;

    EPS_CONDN.swswelf_lock = 0xFFFF & 0b1;

    if (battery_bus_enabled == BUS_ENABLED)
        EPS_CONDN.bat_bus_en = 0xFFFF & (0b1 << 1);

    EPS_CONDN.lup_3v3 = 0xFFFF & 0b1;
    EPS_CONDN.lup_5v  = 0xFFFF & (0b1 << 1);

    // Issue the telemetry write request
    log_telem(EPS_CONDN_TELEM);
}

//-------- Private Functions -----------

/**
 * @brief Check EPS slow telemetry bin.
 *
 * This checker function examines the EPS slow section of the telemetry snapshot and logs errors if
 * telemetry values are out of range.
 */
static void eps_telem_check_slow(void) {
    if ((EPS_SLOW.sense_temp_bat_1 < 25) && (EPS_SLOW.sense_temp_bat_2 < 25) && (EPS_SLOW.sense_temp_bat_3 < 25) && (EPS_SLOW.sense_temp_bat_4 < 25)) {
        battery_bus_enabled = BUS_ENABLED;
    } else {
        battery_bus_enabled = BUS_DISABLED;
        log_str(ERROR, LOG_TELEM_INFRA, "EPS bin %d out of range", EPS_SLOW.sense_temp_bat_1);
    }
}
