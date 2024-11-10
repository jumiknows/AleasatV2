/**
 * @file orca_telem_logger.h
 * @brief Telemetry logging infrastructure.
 * @author Richard A
 * @date October 31, 2019
 *
 * Functions for logging telemetry. This includes initializing the telemetry logging infrastructure
 * and sumitting requests to the telemetry logger. See orca_telem_logger.c for details.
 */

#ifndef ORCA_TELEM_LOGGER_H_
#define ORCA_TELEM_LOGGER_H_

#include "orca_telemetry_defs.h"
#include "obc_error.h"

// Startup Functions
void telem_create_infra(void);
void telem_start_task(void);

// General API
void log_telem(data_bin_t bin);

#endif /* ORCA_TELEM_LOGGER_H_ */
