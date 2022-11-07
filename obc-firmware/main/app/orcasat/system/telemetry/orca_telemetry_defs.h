/**
 * @file orca_telemetry_defs.h
 * @brief Structures that define a per-bin set of telemetry values.
 * @author Richard A
 * @date October 31, 2019
 *
 * In the future, we can probably generate all of these structures from the data budget.
 *
 * Each telemetry data bin is represented by two structures:
 *     - data_bin_name_t is the raw representation of the bin's contents. Individual telemetry data
 * points make up these structures.
 *     - data_bin_wrapper_t contains the data_bin_name_t and a timestamp.
 */

#ifndef ORCASAT_SYSTEM_TELEMETRY_ORCA_TELEMETRY_DEFS_H_
#define ORCASAT_SYSTEM_TELEMETRY_ORCA_TELEMETRY_DEFS_H_

#include "sys_common.h"
#include "obc_rtc.h"

#define BUS_DISABLED 0
#define BUS_ENABLED  1

/**
 * @brief Enum indicating the data bin.
 */
typedef enum { OBC_SLOW_TELEM = 0, OBC_FAST_TELEM = 1, EPS_SLOW_TELEM = 2, EPS_NORMAL_TELEM = 3, EPS_FAST_TELEM = 4, EPS_CONDN_TELEM = 5, NUM_TELEM_BINS = 6 } data_bin_t;

/**
 * @brief OBC fast rate telemetry.
 */
typedef struct {
    int16_t obc_temperature_c;
    uint16_t obc_current_ma;
} obc_fast_telem_t;

/**
 * @brief OBC fast rate telemetry wrapper.
 *
 * Each telemetry bin has a wrapper to contain timestamp and the telemetry.
 * This is the data structure that is written to flash.
 */
typedef struct {
    epoch_t timestamp;
    obc_fast_telem_t telem;
} obc_fast_telem_wrapper_t;

/**
 * @brief OBC slow rate telemetry.
 */
typedef struct {
    uint32_t obc_fs_free_bytes;
    uint32_t minheap;
    uint16_t ramoccur_1;
    uint16_t ramoccur_2;
    uint32_t obc_rtc_health;
    uint32_t obc_flag_check;
} obc_slow_telem_t;

/**
 * @brief OBC slow rate telemetry wrapper.
 */
typedef struct {
    epoch_t timestamp;
    obc_slow_telem_t telem;
} obc_slow_telem_wrapper_t;

/**
 * @brief EPS normal rate telemetry.
 */
typedef struct {
    uint16_t sense_v_bat;
    uint16_t sense_i_bat;

    uint16_t sense_v_bcr;
    uint16_t sense_i_bcr;

    uint16_t sense_i_bus_3v3;
    uint16_t sense_i_bus_5v5;

    uint32_t fs_bat_chg_1;
    uint32_t fs_bat_chg_2;

    uint32_t out_1;
    uint32_t out_2;
    uint32_t out_3;
    uint32_t out_4;
    uint32_t out_5;
    uint32_t out_6;

    uint16_t pwr_cyc;

    uint16_t v_under_v;   // count # times occurred.
    uint16_t v_short;     // count # times occurred.
    uint16_t v_over_temp; // count # times occurred.
    uint16_t charge_cyc;  // count # times occurred.

    uint16_t def_1;
    uint16_t def_2;

    uint32_t shutdn_bat_chg;
} eps_normal_telem_t;

/**
 * @brief EPS normal rate telemetry wrapper.
 */
typedef struct {
    epoch_t timestamp;
    eps_normal_telem_t telem;
} eps_normal_telem_wrapper_t;

/**
 * @brief EPS fast rate telemetry.
 */
typedef struct {
    uint16_t sense_v_x;
    uint16_t sense_i_x_n_a;
    uint16_t sense_i_x_p_a;

    uint16_t sense_v_y;
    uint16_t sense_i_y_n_a;
    uint16_t sense_i_y_p_a;

    uint16_t sense_v_z;
    uint16_t sense_i_z_n_a;
    uint16_t sense_i_z_p_a;

    uint16_t cond_in;
    uint16_t cond_out_1;
    uint16_t cond_out_2;
} eps_fast_telem_t;

/**
 * @brief EPS fast rate telemetry wrapper.
 */
typedef struct {
    epoch_t timestamp;
    eps_fast_telem_t telem;
} eps_fast_telem_wrapper_t;

/**
 * @brief EPS slow rate telemetry.
 */
typedef struct {
    int16_t sense_temp_mcu;

    int16_t sense_temp_bat_1;
    int16_t sense_temp_bat_2;
    int16_t sense_temp_bat_3;
    int16_t sense_temp_bat_4;

    int16_t max_temp_1;
    int16_t max_temp_2;
    int16_t max_temp_3;
    int16_t max_temp_4;

    int16_t min_temp_1;
    int16_t min_temp_2;
    int16_t min_temp_3;
    int16_t min_temp_4;

    int16_t sense_temp_ext_1;
    int16_t sense_temp_ext_2;
    int16_t sense_temp_ext_3;

    uint32_t bat_bcr_en;

    uint32_t bus_3v3_en;
    uint32_t bus_5v_en;

    uint32_t heat_1;
    uint32_t heat_2;
    uint32_t heat_3;
} eps_slow_telem_t;

/**
 * @brief EPS slow rate telemetry wrappper.
 */
typedef struct {
    epoch_t timestamp;
    eps_slow_telem_t telem;
} eps_slow_telem_wrapper_t;

/**
 * @brief EPS conditional telemetry.
 */
typedef struct {
    uint16_t sw_ver;       // initialize once at beginning.
    uint32_t swswelf_lock; // collect when rbf pin removed.

    uint32_t bat_bus_en; // collect if temp < 45.

    uint32_t lup_3v3; // periodic
    uint32_t lup_5v;  // periodic
} eps_condn_telem_t;

typedef struct {
    epoch_t timestamp;
    eps_condn_telem_t telem;
} eps_condn_telem_wrapper_t;

#endif /* ORCASAT_SYSTEM_TELEMETRY_ORCA_TELEMETRY_DEFS_H_ */
