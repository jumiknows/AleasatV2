/**
 * @file telem_snapshot.c
 * @brief Telemetry snapshot.
 * @author Richard A
 * @date October 31, 2019
 */

#include "telem_snapshot.h"

/**
 * @brief Telemetry snapshot.
 *
 * Initializes all of the telemetry data points.
 */
telem_snapshot_t snapshot = {
    .obc_fast = {
        .timestamp = 0,
        .telem = {
            .obc_current_ma = 0}
    },
    .obc_slow = {
        .timestamp = (epoch_t)0,
        .telem = {
            .obc_fs_free_bytes = 0,
            .minheap = 0,
            .ramoccur_1 = 0,
            .ramoccur_2 = 0,
            .obc_rtc_health = 0,
            .obc_flag_check = 0
        }
    },
    .eps_fast = {
        .timestamp = (epoch_t)0,
        .telem = {
            .sense_v_x = 0,
            .sense_i_x_n_a = 0,
            .sense_i_x_p_a = 0,
            .sense_v_y = 0,
            .sense_i_y_n_a = 0,
            .sense_i_y_p_a = 0,
            .sense_v_z = 0,
            .sense_i_z_n_a = 0,
            .sense_i_z_p_a = 0,
            .cond_in = 0,
            .cond_out_1 = 0,
            .cond_out_2 = 0
        }
    },
    .eps_normal = {
        .timestamp = (epoch_t)0,
        .telem = {
            .sense_v_bat = 0,
            .sense_i_bat = 0,
            .sense_v_bcr = 0,
            .sense_i_bcr = 0,
            .sense_i_bus_3v3 = 0,
            .sense_i_bus_5v5 = 0,
            .fs_bat_chg_1 = 0,
            .fs_bat_chg_2 = 0,
            .out_1 = 0,
            .out_2 = 0,
            .out_3 = 0,
            .out_4 = 0,
            .out_5 = 0,
            .out_6 = 0,
            .sense_v_bat = 0,
            .sense_i_bat = 0,
            .sense_v_bcr = 0,
            .sense_i_bcr = 0,
            .sense_i_bus_3v3 = 0,
            .sense_i_bus_5v5 = 0,
            .fs_bat_chg_1 = 0,
            .fs_bat_chg_2 = 0,
            .out_1 = 0,
            .out_2 = 0,
            .out_3 = 0,
            .out_4 = 0,
            .out_5 = 0,
            .out_6 = 0,
            .pwr_cyc = 0,
            .v_under_v   = 0,
            .v_short     = 0,
            .v_over_temp = 0,
            .charge_cyc  = 0,
            .def_1 = 0,
            .def_2 = 0,
            .shutdn_bat_chg = 0
        }
    },
    .eps_slow = {
        .timestamp = (epoch_t)0,
        .telem = {
            .sense_temp_mcu = 0,
            .sense_temp_bat_1 = 0,
            .sense_temp_bat_2 = 0,
            .sense_temp_bat_3 = 0,
            .sense_temp_bat_4 = 0,
            .max_temp_1 = 0,
            .max_temp_2 = 0,
            .max_temp_3 = 0,
            .max_temp_4 = 0,
            .min_temp_1 = 0,
            .min_temp_2 = 0,
            .min_temp_3 = 0,
            .min_temp_4 = 0,
            .sense_temp_ext_1 = 0,
            .sense_temp_ext_2 = 0,
            .sense_temp_ext_3 = 0,
            .bat_bcr_en = 0,
            .bus_3v3_en = 0,
            .bus_5v_en = 0,
            .heat_1 = 0,
            .heat_2 = 0,
            .heat_3 = 0
        }
    },
    .eps_condn = {
        .timestamp = (epoch_t)0,
        .telem = {
            .sw_ver = 0,
            .swswelf_lock = 0,
            .bat_bus_en = 0,
            .lup_3v3 = 0,
            .lup_5v  = 0
        }
    }
};
