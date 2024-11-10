/**
 * @file oem7x_parse.c
 * @brief OEM7x GPS module - message parsing library
 * @author Andres Martinez
 * @date April 6, 2020
 *
 */

#include "oem7x_parse.h"
#include "obc_misra.h"
#include "obc_utils.h"

#include <string.h>
#include <stdlib.h>

// Helper union to facilitate iteration over the BESTXYZ position and velocity fields
typedef union {
    gps_ecef_t;
    struct {
        double position[6];
        double velocity[6];
    };
} gps_xyz_u;

/**
 * @brief Parse ECEF position and velocity from LOG BESTXYZ (ASCII format)
 *
 * @pre Message is null-terminated
 * @param msg[in]: ASCII message buffer (modified by function)
 * @param ec[out]: Parsed ECEF position & velocity
 *
 * @return Result status - E_SUCCESS if message was parsed successfully, error code otherwise
 */
gps_parse_err_t parse_gps_bestxyz_ascii(char* msg, gps_ecef_t* ec) {
    char* conv_ptr = NULL;
    char* tok      = NULL;

    // Find command response header
    if ((!msg) || (*msg != '<')) {
        return GPS_PARSE_NO_CMD_RESP;
    }

    // Check that command responsed returned OK code
    if (strncmp(msg, "<OK", 3) != 0) {
        return GPS_PARSE_CMD_RESP_ERROR;
    }

    // Find SYNC character
    tok = strchr(msg, '#');
    if (!tok) {
        return GPS_PARSE_NO_SYNC;
    }

    // Checksum verification (TBD)

    // Check message type
    tok = strtok(tok, ",");
    if (strncmp(tok + 1, "BESTXYZA", 9) != 0) {
        return GPS_PARSE_MSG_TYPE;
    }
    strtok(NULL, ";");

    // Position solution status
    tok = strtok(NULL, ",");
    if (!tok) {
        return GPS_PARSE_MATCH_COUNT;
    }
    strncpy(ec->p_sol_status, tok, sizeof(ec->p_sol_status));

    // Skip field
    strtok(NULL, ",");

    // Position data
    gps_xyz_u* eu = (gps_xyz_u*)ec;
    uint8_t i;
    for (i = 0; i < 6; ++i) {
        tok = strtok(NULL, ",");
        if (!tok) {
            return GPS_PARSE_MATCH_COUNT;
        }
        eu->position[i] = strtod(tok, &conv_ptr);
        if (tok == conv_ptr) {
            return GPS_PARSE_MATCH_COUNT;
        }
    }

    // Velocity solution status
    tok = strtok(NULL, ",");
    if (!tok) {
        return GPS_PARSE_MATCH_COUNT;
    }
    strncpy(ec->v_sol_status, tok, sizeof(ec->v_sol_status));

    // Skip field
    strtok(NULL, ",");

    // Velocity data
    for (i = 0; i < 6; ++i) {
        tok = strtok(NULL, ",");
        if (!tok) {
            return GPS_PARSE_MATCH_COUNT;
        }
        eu->velocity[i] = strtod(tok, &conv_ptr);
        if (tok == conv_ptr) {
            return GPS_PARSE_MATCH_COUNT;
        }
    }

    // Skip fields
    for (i = 0; i < 3; ++i) {
        strtok(NULL, ",");
    }

    // Solution age
    tok = strtok(NULL, ",");
    if (!tok) {
        return GPS_PARSE_MATCH_COUNT;
    }
    ec->sol_age_s = strtof(tok, &conv_ptr);
    if (tok == conv_ptr) {
        return GPS_PARSE_MATCH_COUNT;
    }

    return GPS_PARSE_SUCCESS;
}

/**
 * @brief Parse GPS time from LOG TIMESYNC (ASCII format)
 *
 * @pre Message is null-terminated
 * @param msg[in]: ASCII message buffer (modified by function)
 * @param ts[out]: Parsed GPS time
 *
 * @return Result status - E_SUCCESS if message was parsed successfully, error code otherwise
 */
gps_parse_err_t parse_gps_timesync_ascii(char* msg, gps_time_t* ts) {
    char* conv_ptr = NULL;
    char* tok      = NULL;

    // Find command response header
    if ((!msg) || (*msg != '<')) {
        return GPS_PARSE_NO_CMD_RESP;
    }

    // Check that command responsed returned OK code
    if (strncmp(msg, "<OK", 3) != 0) {
        return GPS_PARSE_CMD_RESP_ERROR;
    }

    // Find SYNC character
    tok = strchr(msg, '#');
    if (!tok) {
        return GPS_PARSE_NO_SYNC;
    }

    // Checksum verification (TBD)

    // Check message type
    tok = strtok(tok, ",");
    if (strncmp(tok + 1, "TIMESYNCA", 10) != 0) {
        return GPS_PARSE_MSG_TYPE;
    }
    strtok(NULL, ";");

    // ref_week
    tok = strtok(NULL, ",");
    if (!tok) {
        return GPS_PARSE_MATCH_COUNT;
    }
    ts->ref_week = strtol(tok, &conv_ptr, 10);
    if ((ts->ref_week == 0) && (tok == conv_ptr)) {
        return GPS_PARSE_MATCH_COUNT;
    }

    // offset_ms
    tok = strtok(NULL, ",");
    if (!tok) {
        return GPS_PARSE_MATCH_COUNT;
    }
    ts->offset_ms = strtol(tok, &conv_ptr, 10);
    if ((ts->offset_ms == 0) && (tok == conv_ptr)) {
        return GPS_PARSE_MATCH_COUNT;
    }

    // t_sol_status
    tok = strtok(NULL, "*");
    if (!tok) {
        return GPS_PARSE_MATCH_COUNT;
    }
    strncpy(ts->t_sol_status, tok, sizeof(ts->t_sol_status));

    return GPS_PARSE_SUCCESS;
}
