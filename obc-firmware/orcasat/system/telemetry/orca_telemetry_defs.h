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

/**
 * @brief Enum indicating the data bin.
 */
typedef enum { OBC_SLOW_TELEM = 0, OBC_FAST_TELEM, EPS_SLOW_TELEM, NUM_TELEM_BINS } data_bin_t;

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
 * @brief EPS slow rate telemetry.
 */
typedef struct {
    uint16_t sense_v_bat;
    uint16_t sense_i_bat;
} eps_slow_telem_t;

/**
 * @brief EPS slow rate telemetry wrappper.
 */
typedef struct {
    epoch_t timestamp;
    eps_slow_telem_t telem;
} eps_slow_telem_wrapper_t;

#endif /* ORCASAT_SYSTEM_TELEMETRY_ORCA_TELEMETRY_DEFS_H_ */
