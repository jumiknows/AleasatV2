/**
 * @file telem_eps.h
 * @brief Telemetry collection functions for the EPS.
 * @author Richard A
 * @date October 31, 2019
 *
 * EPS telemetry functions.
 */

#ifndef ORCASAT_SYSTEM_TELEMETRY_TELEM_EPS_H_
#define ORCASAT_SYSTEM_TELEMETRY_TELEM_EPS_H_

#include "obc_error.h"

void eps_normal_telem_collect(void);
void eps_fast_telem_collect(void);
void eps_slow_telem_collect(void);
void eps_condn_telem_collect(void);

#endif /* ORCASAT_SYSTEM_TELEMETRY_TELEM_EPS_H_ */
