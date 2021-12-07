/** @file obc_long_cmd_actions.c
 *  @brief OBC long command function implementations.
 *  @author ORCASat C&DH team
 *  @date 2020-07-06
 *
 *  TODO(ALEA-574): Store long commands and have a command to get them back.
 */

#include <string.h>
#include "obc_long_cmd_actions.h"
#include "logger.h"
#include "obc_rtos.h"
#include "filesystem.h"
#include "rtc_state.h"
#include "scheduler.h"
#include "obc_task_info.h"
#include "obc_gps.h"
#include "obc_temperature.h"
#include "obc_utils.h"
#include "obc_cmd.h"

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

// ------------------------ FILESYSTEM LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Read the contents of a directory, contents are printed in the @ref fs_ls function
 *            Arg 0 - directory path
 */
void cmd_fs_ls(uint32_t arg_len, void* arg) {
    if (arg_len == 0) {
        log_str(DEBUG, FS_LOG, false, "Usage: ls [PATH]");
    } else {
        char ls_list[MAX_DIRS][LFS_NAME_MAX] = {'\0'};

        fs_err_t err = fs_ls((const char*)arg, ls_list);

        if (err != FS_OK) {
            log_str(ERROR, FS_LOG, true, "Filesystem error: %d", err);
        } else {
            uint8_t i;
            for (i = 0; i < MAX_DIRS; i++) {
                if (strlen(ls_list[i]) != 0) {
                    log_str(DEBUG, FS_LOG, false, "%s", ls_list[i]);
                }
            }
        }
    }
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

    if (num_args(arg_len) == 1) {
        const uint16_t acq_timeout = cseq_to_num((char*)arg);
        err                        = gps_req_time(acq_timeout, &ts);
    } else {
        err = gps_req_time(0, &ts);
    }

    if (err == GPS_SUCCESS) {
        log_str(INFO, GPS_LOG, true, "GPS TIME RECV");
        log_str(INFO, GPS_LOG, true, "Ref. week: %d", ts.ref_week);
        log_str(INFO, GPS_LOG, true, "Offset (ms): %d", ts.offset_ms);
        log_str(INFO, GPS_LOG, true, "Sol. status: %s", ts.t_sol_status);
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

    if (num_args(arg_len) == 1) {
        const uint16_t acq_timeout = cseq_to_num((char*)arg);
        err                        = gps_req_xyz(acq_timeout, &ec);
    } else {
        err = gps_req_xyz(0, &ec);
    }

    if (err == GPS_SUCCESS) {
        log_str(INFO, GPS_LOG, true, "GPS XYZ RECV");

        log_str(INFO, GPS_LOG, true, "Position data (m):");
        log_str(INFO, GPS_LOG, true, "px: %lf", ec.px_m);
        log_str(INFO, GPS_LOG, true, "py: %lf", ec.py_m);
        log_str(INFO, GPS_LOG, true, "pz: %lf", ec.pz_m);
        log_str(INFO, GPS_LOG, true, "px_sd: %lf", ec.px_sd_m);
        log_str(INFO, GPS_LOG, true, "py_sd: %lf", ec.py_sd_m);
        log_str(INFO, GPS_LOG, true, "pz_sd: %lf", ec.pz_sd_m);
        log_str(INFO, GPS_LOG, true, "Sol. status: %s", ec.p_sol_status);

        log_str(INFO, GPS_LOG, true, "Velocity data (m/s):");
        log_str(INFO, GPS_LOG, true, "vx: %lf", ec.vx_mps);
        log_str(INFO, GPS_LOG, true, "vy: %lf", ec.vy_mps);
        log_str(INFO, GPS_LOG, true, "vz: %lf", ec.vz_mps);
        log_str(INFO, GPS_LOG, true, "vx_sd: %lf", ec.vx_sd_mps);
        log_str(INFO, GPS_LOG, true, "vy_sd: %lf", ec.vy_sd_mps);
        log_str(INFO, GPS_LOG, true, "vz_sd: %lf", ec.vz_sd_mps);
        log_str(INFO, GPS_LOG, true, "Sol. status: %s", ec.v_sol_status);

        log_str(INFO, GPS_LOG, true, "Sol. age: %f", ec.sol_age_s);
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
    const uint8_t arg_count = num_args(arg_len);
    if (arg_count == 0) {
        log_str(DEBUG, GPS_LOG, false, "Usage: gps_cmd [WORD_0 ..]");
        return;
    }

    // Form the arguments into a valid OEM7X command string
    char req_msg[64]                  = {0};
    char resp_msg[GPS_RX_BUFFER_SIZE] = {0};
    uint8_t i;
    for (i = 0; i < arg_count; ++i) {
        if (i != 0) {
            strcat(req_msg, " ");
        }
        strcat(req_msg, (char*)arg + (ARGUMENT_SIZE * i));
    }

    // Send the command and receive the response string
    log_str(INFO, GPS_LOG, true, "Sending: %s", req_msg);
    const gps_req_err_t err = gps_cmd(req_msg, resp_msg);
    if (err == GPS_SUCCESS) {
        // TBD: Need to log the entire response string for downlinking, which is sometimes more than 31 bytes..
        log_str(INFO, GPS_LOG, true, "Resp: %s", resp_msg);
    } else {
        log_str(ERROR, GPS_LOG, true, "GPS req. err: %d", err);
    }
}

// ------------------------ TESTING LONG COMMAND IMPLEMENTATION ------------------------

/**
 * @brief Executes the temperature sensor self-test routine.
 */
void cmd_test_temperature(uint32_t arg_len, void* arg) {
    self_test_temperature();
}
