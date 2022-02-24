/**
 * @file oem7x_parse.h
 * @brief OEM7x GPS module - message parsing library
 * @author Andres Martinez
 * @date April 6, 2020
 */

#ifndef OEM7X_PARSE_H
#define OEM7X_PARSE_H

#include "sys_common.h"
#include "obc_error.h"

/**
 * @brief Wrapper for GPS ECEF position and velocity
 */
typedef struct {
    // Position vector and standard deviation
    double px_m;
    double py_m;
    double pz_m;
    double px_sd_m;
    double py_sd_m;
    double pz_sd_m;
    // Velocity vector and standard deviation
    double vx_mps;
    double vy_mps;
    double vz_mps;
    double vx_sd_mps;
    double vy_sd_mps;
    double vz_sd_mps;

    float sol_age_s; // Solution age (seconds)

    char p_sol_status[24]; // Position solution status
    char v_sol_status[24]; // Velocity solution status
} gps_ecef_t;

/**
 * @brief Wrapper for GPS time
 */
typedef struct {
    uint32_t ref_week;     // Reference week
    uint32_t offset_ms;    // Milliseconds into the reference week
    char t_sol_status[24]; // Time solution status
} gps_time_t;

gps_parse_err_t parse_gps_bestxyz_ascii(char* msg, gps_ecef_t* ec);
gps_parse_err_t parse_gps_timesync_ascii(char* msg, gps_time_t* ts);

#endif
