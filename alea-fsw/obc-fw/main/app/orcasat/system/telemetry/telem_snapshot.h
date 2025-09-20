/**
 * @file telem_snapshot.h
 * @brief Telemetry snapshot declarations.
 * @author Richard A
 * @date October 31, 2019
 *
 * The telemetry snapshot structure stores the most recent telemetry readings from across the
 * satellite. The snapshot can be downlinked on its own and examined to quickly get an idea of the
 * health of ORCASat.
 *
 * Data bins are a set of telemetry readings collected at a pre-defined but configurable rate.
 *
 * The structure of this file is kept simple so that it _could_ be generated automatically from the
 * data budget.
 */

#ifndef TELEM_SNAPSHOT_H_
#define TELEM_SNAPSHOT_H_

#include "orca_telemetry_defs.h"

/**
 * @brief Telemetry snapshot.
 *
 * This structure is composed of all the data bin wrapper structures, which include
 * the telemetry readings of each bin, and a timestamp for the whole bin.
 */
typedef struct {
    obc_fast_telem_wrapper_t obc_fast;
    obc_slow_telem_wrapper_t obc_slow;
    eps_normal_telem_wrapper_t eps_normal;
    eps_fast_telem_wrapper_t eps_fast;
    eps_slow_telem_wrapper_t eps_slow;
    eps_condn_telem_wrapper_t eps_condn;
} telem_snapshot_t;

extern telem_snapshot_t snapshot;

/**
 * @brief Definitions for accessing snapshot data bin fields in a concise way.
 *
 * For example, to access the .obc_current_ma member of @ref obc_fast_telem,
 * use OBC_FAST.obc_current_ma.
 */
#define OBC_FAST   snapshot.obc_fast.telem
#define OBC_SLOW   snapshot.obc_slow.telem
#define EPS_NORMAL snapshot.eps_normal.telem
#define EPS_FAST   snapshot.eps_fast.telem
#define EPS_SLOW   snapshot.eps_slow.telem
#define EPS_CONDN  snapshot.eps_condn.telem

/**
 * @brief Definitions for accessing telemetry snapshot timestamp fields in a concise way.
 */
#define OBC_FAST_TS   snapshot.obc_fast.timestamp
#define OBC_SLOW_TS   snapshot.obc_slow.timestamp
#define EPS_NORMAL_TS snapshot.eps_normal.timestamp
#define EPS_FAST_TS   snapshot.eps_fast.timestamp
#define EPS_SLOW_TS   snapshot.eps_slow.timestamp
#define EPS_CONDN_TS  snapshot.eps_condn.timestamp

#endif /* TELEM_SNAPSHOT_H_ */
