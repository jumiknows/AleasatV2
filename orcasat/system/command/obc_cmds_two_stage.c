/** @file obc_cmds_two_stage.c
 *  @brief Two-stage commands
 *  @author Lana
 *  @date 23-January-2020
 */

#include <stdbool.h>
#include <stdlib.h>

#include "log_ids.h"
#include "logger.h"
#include "obc_rtc.h"
#include "obc_cmds_two_stage.h"

#define TWO_STAGE_TIMEOUT_MINS (0.1) //(10)
#define TWO_STAGE_TIMEOUT_SECS (TWO_STAGE_TIMEOUT_MINS) * (60)

typedef struct CMD_ARM_DATA {
    bool is_armed;
    real_time_t arm_time;
} CMD_ARM_DATA_t;

static bool check_arm(CMD_ARM_DATA_t* arm_data);
static bool is_arm_valid(real_time_t* time_start, real_time_t* time_end);

/** @brief Arm for the test two-stage command */
CMD_ARM_DATA_t cmd_two_stage_arm_data;
void cmd_two_stage_arm(uint32_t arg_len, void* arg) {
    cmd_two_stage_arm_data.is_armed = true;
    uint8_t errcode                 = get_current_time(&cmd_two_stage_arm_data.arm_time);
    if (errcode != RTC_NO_ERR) {
        log_str(ERROR, CMD_GENERAL, true, "CommandSys failed to read time");
    }
}

/** @brief Fire for the test two-stage command */
void cmd_two_stage_fire(uint32_t arg_len, void* arg) {
    int16_t err = check_arm(&cmd_two_stage_arm_data);
    if (err != true) {
        log_str(ERROR, CMD_GENERAL, true, "FAILED two-stage fire test\n");
        return;
    }
    log_str(INFO, CMD_GENERAL, true, "Two-stage test: fired!");
}

/**
 * @brief Checks whether there a valid arm is available for the corresponding command
 *
 * @param [out] arm_data: pointer to the arm data
 * @return 0 if arm is valid, -1 otherwise
 */
static bool check_arm(CMD_ARM_DATA_t* arm_data) {
    bool is_armed = arm_data->is_armed;
    if (!is_armed) {
        log_str(ERROR, CMD_GENERAL, true, "No ARM");
        return false;
    } else {
        real_time_t time_fire = {NULL};
        uint8_t errcode       = get_current_time(&time_fire);
        if (errcode != RTC_NO_ERR) {
            log_str(ERROR, CMD_GENERAL, true, "CommandSys failed to read time");
            return false;
        }
        if (!is_arm_valid(&(arm_data->arm_time), &time_fire)) {
            log_str(ERROR, CMD_GENERAL, true, "ARM expired");
            arm_data->is_armed = false;
            return false;
        }
        arm_data->is_armed = false;
        return true;
    }
}

/**
 * @brief Checks whether the two time entries are within the two-stage arm timeout
 *
 * @param time_start: pointer to start time
 * @param time_end: pointer to end time
 * @return true if ARM is within time limit, false otherwise
 */
static bool is_arm_valid(real_time_t* time_start, real_time_t* time_end) {
    // TODO: Replace this with a proper epoch calculator when it is implemented
    uint32_t time_start_sec   = time_start->second + (60 * (time_start->minute + (60 * (time_start->hour + (24 * time_start->day)))));
    uint32_t time_end_sec     = time_end->second + (60 * (time_end->minute + (60 * (time_end->hour + (24 * time_end->day)))));
    uint32_t time_elapsed_sec = time_end_sec - time_start_sec;
    if (time_elapsed_sec < (uint32_t)(TWO_STAGE_TIMEOUT_SECS)) {
        return true;
    } else {
        return false;
    }
}
