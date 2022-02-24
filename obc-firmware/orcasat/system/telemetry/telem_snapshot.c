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
telem_snapshot_t snapshot = {.obc_fast = {.timestamp = 0, .telem = {.obc_temperature_c = 0, .obc_current_ma = 0}},
                             .obc_slow = {.timestamp = 0, .telem = {.obc_fs_free_bytes = 0, .minheap = 0, .ramoccur_1 = 0, .ramoccur_2 = 0, .obc_rtc_health = 0, .obc_flag_check = 0}},
                             .eps_slow = {.timestamp = 0, .telem = {.sense_v_bat = 0, .sense_i_bat = 0}}};
