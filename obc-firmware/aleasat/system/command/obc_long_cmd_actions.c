/** @file obc_long_cmd_actions.c
 *  @brief OBC long command function implementations.
 *
 *  TODO(ALEA-574): Store long commands and have a command to get them back.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_cmd.h"
#include "obc_long_cmd_actions.h"
#include "obc_task_info.h"
#include "obc_gps.h"
#include "obc_temperature.h"
#include "obc_utils.h"
#include "obc_cmd.h"
#include "obc_rtos.h"
#include "logger.h"
#include "rtc_state.h"
#include "scheduler.h"

// Standard Library
#include <string.h>

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

// ------------------------ RTOS LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Prints information about all RTOS tasks.
 */
void cmd_rtos_tasks(uint32_t arg_len, void* arg) {
    print_tasks();
}

/** @brief Prints info about the OBC RTOS layer. */
void cmd_rtos_info(uint32_t arg_len, void* arg) {
    print_rtos_status();
}

// ------------------------ RTC LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Prints RTC config information.
 */
void cmd_rtc_info(uint32_t arg_len, void* arg) {
    if (arg_len == 0) {
        print_rtc_info();
    }
}

/**
 * @brief Prints RTC stats.
 */
void cmd_rtc_stats(uint32_t arg_len, void* arg) {
    if (arg_len == 0) {
        print_rtc_stats();
    }
}

// ------------------------ SCHEDULE LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Prints the schedule.
 */
void cmd_print_sched(uint32_t arg_len, void* arg) {
    print_schedule();
}

// ------------------------ GPS LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Request the current GPS module time data
 * This command accepts a first argument as the timeout value for accurate acquisition
 *
 * Non-zero value will poll the GPS receiver for n-seconds until an accurate result is returned
 * Zero-value or missing argument will get the immediate result from the GPS receiver
 */
void cmd_gps_time(uint32_t arg_len, void* arg) {
    gps_time_t ts = {0};
    gps_req_err_t err;
    char* args[1] = {NULL};

    uint8_t num_args = obc_cmd_read_str_arguments(arg, 1, args);

    if (num_args == 1) {
        const int16_t acq_timeout = cseq_to_num((char*)arg);
        if (acq_timeout == -1) {
            log_str(ERROR, LOG_GPS_GENERAL, true, "Invalid timeout: %s", (char*)arg);
            return;
        }

        err                        = gps_req_time(acq_timeout, &ts);
    } else {
        err = gps_req_time(0, &ts);
    }

    if (err == GPS_SUCCESS) {
        log_str(INFO, LOG_GPS_GENERAL, true, "GPS TIME RECV");
        log_str(INFO, LOG_GPS_GENERAL, true, "Ref. week: %d", ts.ref_week);
        log_str(INFO, LOG_GPS_GENERAL, true, "Offset (ms): %d", ts.offset_ms);
        log_str(INFO, LOG_GPS_GENERAL, true, "Sol. status: %s", ts.t_sol_status);
    }
}

/**
 * @brief Request the current GPS module position and velocity data
 * This command accepts a first argument as the timeout value for accurate acquisition
 *
 * Non-zero value will poll the GPS receiver for n-seconds until an accurate result is returned
 * Zero-value or missing argument will get the immediate result from the GPS receiver
 */
void cmd_gps_xyz(uint32_t arg_len, void* arg) {
    gps_ecef_t ec = {0};
    gps_req_err_t err;
    char* args[1] = {NULL};

    uint8_t num_args = obc_cmd_read_str_arguments(arg, 1, args);

    if (num_args == 1) {
        const int16_t acq_timeout = cseq_to_num((char*)arg);
        if(acq_timeout == -1) {
            log_str(ERROR, LOG_GPS_GENERAL, true, "Invalid timeout: %s", (char*)arg);
            return;
        }

        err                        = gps_req_xyz(acq_timeout, &ec);
    } else {
        err = gps_req_xyz(0, &ec);
    }

    if (err == GPS_SUCCESS) {
        log_str(INFO, LOG_GPS_GENERAL, true, "GPS XYZ RECV");

        log_str(INFO, LOG_GPS_GENERAL, true, "Position data (m):");
        log_str(INFO, LOG_GPS_GENERAL, true, "px: %lf", ec.px_m);
        log_str(INFO, LOG_GPS_GENERAL, true, "py: %lf", ec.py_m);
        log_str(INFO, LOG_GPS_GENERAL, true, "pz: %lf", ec.pz_m);
        log_str(INFO, LOG_GPS_GENERAL, true, "px_sd: %lf", ec.px_sd_m);
        log_str(INFO, LOG_GPS_GENERAL, true, "py_sd: %lf", ec.py_sd_m);
        log_str(INFO, LOG_GPS_GENERAL, true, "pz_sd: %lf", ec.pz_sd_m);
        log_str(INFO, LOG_GPS_GENERAL, true, "Sol. status: %s", ec.p_sol_status);

        log_str(INFO, LOG_GPS_GENERAL, true, "Velocity data (m/s):");
        log_str(INFO, LOG_GPS_GENERAL, true, "vx: %lf", ec.vx_mps);
        log_str(INFO, LOG_GPS_GENERAL, true, "vy: %lf", ec.vy_mps);
        log_str(INFO, LOG_GPS_GENERAL, true, "vz: %lf", ec.vz_mps);
        log_str(INFO, LOG_GPS_GENERAL, true, "vx_sd: %lf", ec.vx_sd_mps);
        log_str(INFO, LOG_GPS_GENERAL, true, "vy_sd: %lf", ec.vy_sd_mps);
        log_str(INFO, LOG_GPS_GENERAL, true, "vz_sd: %lf", ec.vz_sd_mps);
        log_str(INFO, LOG_GPS_GENERAL, true, "Sol. status: %s", ec.v_sol_status);

        log_str(INFO, LOG_GPS_GENERAL, true, "Sol. age: %f", ec.sol_age_s);
    }
}

/**
 * @brief Send a string to the OEM7X GPS module and receive the response string.
 * Intended to be used for sending arbitrary user-specified commands to the module (i.e. status request, reset)
 *
 * The message sent to the GPS module will consist of all words after the gps_cmd (plus a \r\n terminator)
 * Example: <gps_cmd LOG RXSTATUSA ONCE> will send "LOG RXSTATUSA ONCE\r\n" to the module
 * GPS command strings are restricted to a maximum length of 62 - (arg_count - 1)
 *
 * Missing arguments will return an error
 */
void cmd_gps_cmd(uint32_t arg_len, void* arg) {
    char* arguments[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    uint8_t num_args    = obc_cmd_read_str_arguments(arg, 10, arguments);
    if (num_args == 0) {
        log_str(DEBUG, LOG_GPS_GENERAL, false, "Usage: gps_cmd [WORD_0 ..]");
        return;
    }

    // Form the arguments into a valid OEM7X command string
    char req_msg[64]                  = {0};
    char resp_msg[GPS_RX_BUFFER_SIZE] = {0};
    for (uint8_t i = 0; i < num_args; ++i) {
        if (i != 0) {
            strcat(req_msg, " ");
        }
        strcat(req_msg, arguments[i]);
    }

    // Send the command and receive the response string
    log_str(INFO, LOG_GPS_GENERAL, true, "Sending: %s", req_msg);
    const gps_req_err_t err = gps_cmd(req_msg, resp_msg);
    if (err == GPS_SUCCESS) {
        // TBD: Need to log the entire response string for downlinking, which is sometimes more than 31 bytes..
        log_str(INFO, LOG_GPS_GENERAL, true, "Resp: %s", resp_msg);
    } else {
        log_str(ERROR, LOG_GPS_GENERAL, true, "GPS req. err: %d", err);
    }
}

// ------------------------ TESTING LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Executes the temperature sensor self-test routine.
 */
void cmd_test_temperature(uint32_t arg_len, void* arg) {
    self_test_temperature();
}

void cmd_test_log_str(uint32_t arg_len, void* arg) {
    char test_payload[MAX_PAYLOAD_SIZE] = "";

    log_str(DEBUG, LOG_PRINT_GENERAL, false, "A");

    for (uint8_t i = 0; i < MAX_PAYLOAD_SIZE; i++) {
        strcat(test_payload, "A");
    }

    log_str(DEBUG, LOG_PRINT_GENERAL, false, test_payload);
}
