/** @file obc_cmd.c
 *  @brief OBC command processing.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_cmd.h"

// OBC
#include "logger.h"
#include "scheduler.h"
#include "obc_uart.h"
#include "obc_utils.h"
#include "obc_cmd.h"
#include "obc_time.h"
#include "obc_prompt_cmd_actions.h"
#include "obc_long_cmd_actions.h"
#include "obc_critical_cmd_actions.h"
#include "obc_glibc.h"

// Testing
#ifdef SYSTEM_TESTS
#include "test_commands.h"
#endif /* SYSTEM_TESTS */

// Standard Library
#include <string.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief A command. Maps a name (opcode) string to an action (function).
 */
typedef struct {
    const char* name;
    scheduler_action_t func;
} cmd_entry;

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief The next valid sequence number that will be accepted.
 *
 * The sequence number starts at 1 at boot, and commands with
 * sequence number 0 are always accepted. Each time a command is
 * received, the next valid sequence number increases. Out of
 * sequence commands will not be executed or scheduled.
 */
uint8_t next_valid_seq_num = 1;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static obc_cmd_err_t parse(cmd_t* cmd, char* input_cmd_str, char** opcode_out);
static obc_cmd_err_t invoke(cmd_t* cmd, action_id_t* action_id, bool immediate, scheduler_submission_source_t source);
static obc_cmd_err_t find_command(char* name, scheduler_action_t* action);
static bool check_seq_num(int32_t sequence_num);
static void increment_seq_num(void);

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief The command table. Maps command opcode strings to a function (action).
 *
 * During testing, a reduced command table is used so that we don't need to mock
 * absolutely every command function.
 */
const cmd_entry command_table[] = {
    /* Random OBC stats */
    {"get_heap", &cmd_get_heap},
    {"get_minheap", &cmd_get_minheap},

    /* RTC Commands */
    {"rtc_stats", &cmd_rtc_stats},
    {"rtc_info", &cmd_rtc_info},
    {"rtc_ts", &cmd_rtc_ts},
    {"set_rtc", &cmd_set_rtc},

    /* RTOS Commands */
    {"rtos_tasks", &cmd_rtos_tasks},
    {"rtos_info", &cmd_rtos_info},
    {"rtos_period", &cmd_rtos_period},
    {"rtos_suspend", &cmd_rtos_suspend},
    {"rtos_resume", &cmd_rtos_resume},
    {"rtos_state", &cmd_rtos_state},

    /* Filesystem commands */
    {"exec_fs_test", &cmd_exec_fs_test},
    {"ls", &cmd_fs_ls},
    {"mkdir", &cmd_fs_mkdir},
    {"rm", &cmd_fs_rm},
    {"mkfile", &cmd_fs_create},
    {"get_file_size", &cmd_fs_file_size},
    {"get_fs_size", &cmd_fs_size},

    /* System Commands */
    {"get_time", &cmd_get_time},
    {"get_epoch", &cmd_get_epoch},
    {"get_epoch_and_time", &cmd_get_epoch_and_time},
    {"get_backup_epoch", &cmd_get_backup_epoch},
    {"set_time", &cmd_set_time},
    {"lpon", &cmd_LP_on},
    {"lpoff", &cmd_LP_off},
    {"nrst", &cmd_nrst},

    /* Scheduler Commands */
    {"get_seq_num", &cmd_get_sequence_num},
    {"get_period", &cmd_get_period},
    {"set_period", &cmd_set_period},
    {"last_response", &cmd_last_response},
    {"print_sched", &cmd_print_sched},

    /* Telemetry Commands */
    {"obc_slow", &cmd_telem_obc_slow},
    {"obc_fast", &cmd_telem_obc_fast},
#if !defined(PLATFORM_ALEA_V1)
    {"eps_slow", &cmd_telem_eps_slow},
#endif // !defined(PLATFORM_ALEA_V1)

    /* GPS Commands */
    {"gps_time", &cmd_gps_time},
    {"gps_xyz", &cmd_gps_xyz},
    {"gps_cmd", &cmd_gps_cmd},

    /* MISC Commands */
    {"echo", &cmd_echo_to_log},
    {"get_board_type", &cmd_get_board_type},
    {"ack", &cmd_ack},
    {"ping", &cmd_ping},
    {"count", &cmd_count},
    {"get_count", &cmd_get_count},
    {"temptest", &cmd_test_temperature},
    {"temp", &cmd_get_temperature},
    {"wdreset", &cmd_watchdog_reset},
    {"run_hang_task", &cmd_run_hang_task},

/* Commands for devices on an ALEA PCB */
#ifndef PLATFORM_ALEA_V1 // TODO use COMMS_WD signal for heartbeat (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/60)
    {"hb_suspend", &cmd_hb_suspend},
    {"hb_start", &cmd_hb_start},
    {"reset_temp", &cmd_temp_reset},
    {"reset_expander", &cmd_expander_reset},
    {"i2c_reset", &cmd_i2c_reset},
    {"nvct_erase", &cmd_erase_nvct},
    {"nvct_provision", &cmd_provision_nvct},
    {"setting_get", &cmd_get_setting},
    {"setting_set", &cmd_set_setting},
#endif

/* Tests */
#ifdef SYSTEM_TESTS
    {"test_ack", &cmd_test_ack},
    {"test_comms_raw", &cmd_test_comms_raw},
    {"test_comms_tx_only", &cmd_test_comms_tx_only},
    {"test_comms_tx_rx", &cmd_test_comms_tx_rx},
    {"test_comms_stress1", &cmd_test_comms_stress1},
    {"test_comms_flash_app", &cmd_test_comms_flash_app},
    {"test_comms_reboot", &cmd_test_comms_reboot},
    {"test_magnetorquer", &cmd_test_mag},
    {"test_magnetorquer_all", &cmd_test_mag_all},
    {"test_magnetorquer_init", &cmd_test_mag_init},
    {"test_imu", &cmd_imu_test},
    {"test_can_gpio", &cmd_test_can_gpio},
    {"test_panel_gyro", &cmd_test_panel_gyro},
    {"test_log_str", &cmd_test_log_str},
#endif // SYSTEM_TESTS
};

/**
 * @brief The critical command table. Maps command opcode strings to a function (action).
 *
 * Critical commands are executed immediately once received, inside an RTOS critical section
 * so that no interrupts can affect how they run. They cannot be scheduled for a point in
 * the future.
 *
 * The critical command table is not included in Ceedling tests.
 */
// Critical command table: critical commands are executed immediately, without the scheduler.
const cmd_entry critical_cmd_table[] = {
    {"nrst_c", &cmd_nrst},
#ifndef PLATFORM_ALEA_V1 // TODO use COMMS_WD signal for heartbeat (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/60)
    {"hb_suspend_c", &cmd_hb_suspend},
    {"hb_start_c", &cmd_hb_start},
#endif
};
const uint32_t num_crit_cmds = sizeof(critical_cmd_table) / sizeof(cmd_entry);

/**
 * @brief The number of regular (non-critical) commands known to the command system.
 */
const uint32_t num_cmds = sizeof(command_table) / sizeof(cmd_entry);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Returns the next valid sequence number that will be accepted.
 * @return The next valid sequence number.
 */
uint8_t obc_cmd_next_seq_num(void) {
    return next_valid_seq_num;
}

/**
 * @brief Looks up an action (function pointer) in the command table and returns
 * the opcode string if the action is a known command. This is used to map actions
 * to the string opcodes outside of the command system.
 *
 * @param[in] action The action (function pointer) to retrieve the opcode string for.
 * @param[out] opcode Points to the command opcode string if the command exists, or "NONE"
 * otherwise.
 * @retval OBC_COMMAND_FOUND: opcode will be valid.
 * @retval OBC_CMD_NO_MATCHING_CMD_NAME: action could not be found, opcode will be "NONE".
 */
obc_cmd_err_t obc_cmd_opcode_from_action(scheduler_action_t action, const char** opcode) {
    static const char* no_matching_cmd = "NONE"; // String that opcode will point to if there is no action
    obc_cmd_err_t status               = OBC_CMD_NO_MATCHING_CMD_NAME;
    *opcode                            = no_matching_cmd;

    // Loop over all actions in the command table and set the output opcode
    // parameter if the command matches.
    uint8_t i = 0;
    for (i = 0; i < num_cmds; i++) {
        if (command_table[i].func == action) {
            *opcode = command_table[i].name;
            status  = OBC_COMMAND_FOUND;
        }
    }
    return status;
}

/**
 * @brief parses and schedules a command.
 *
 * @param[in] buff: a char sequence representing a command. Must be null terminated.
 * @param[in] source: INTERNAL or EXTERNAL. If coming from the radio or UART, it should be EXTERNAL.
 *
 * @returns OBC_IMMEDIATE_SUCCESS if command is successfully scheduled to run immediately
 * @returns OBC_SCHEDULED_SUCCESS if command is successfully scheduled
 * @returns OBC_CRITICAL_SUCCESS if critical command is successfully invoked
 * @returns an error code if there was a problem parsing or scheduling the command
 */
obc_cmd_err_t obc_cmd_parse_and_invoke(char* buff, scheduler_submission_source_t source) {
    // Copy in the command because this function modifies it
    // Is it bad design? Absolutely
    char cmd_str[MAX_CMD_SIZE] = {'\0'};
    char* parsed_opcode        = NULL;
    strncpy(cmd_str, buff, MAX_CMD_SIZE);

    cmd_t cmd             = {'\0'};
    obc_cmd_err_t err     = parse(&cmd, cmd_str, &parsed_opcode);
    action_id_t action_id = 0;
    increment_seq_num();

    // If the command is a critical command, run it right away and then return.
    // TODO(Julian): get rid of critical commands
    if (err == OBC_COMMAND_CRITICAL) {
        cmd.action(0, NULL);
        return OBC_CRITICAL_SUCCESS;
    }

    // If the command is a valid immediate or scheduled command, invoke it so it gets scheduled or
    // executed.
    // TODO: [when we add code generation] we should validate the command before invoking it
    if ((err == OBC_SCHEDULED_SUCCESS) || (err == OBC_IMMEDIATE_SUCCESS)) {
        bool immediate           = (err == OBC_IMMEDIATE_SUCCESS);
        obc_cmd_err_t invoke_err = invoke(&cmd, &action_id, immediate, source);

        // Only reply with success message for externally-scheduled commands. We want the success
        // messages to print as fast as possible so that they appear before any messages from the
        // executed command, so use @ref log_str_no_time.
        // Log errors for all commands.

        switch (invoke_err) {
            case OBC_IMMEDIATE_SUCCESS:
                if ((strcmp(parsed_opcode, "nrst") != 0) && (source == EXTERNAL)) { // no logging for nrst command because the response is often cut off by the reset
                    log_str_no_time(INFO, CMD_GENERAL, false, "> %s immediate", parsed_opcode);
                }
                break;
            case OBC_SCHEDULED_SUCCESS:
                if (source == EXTERNAL) {
                    log_str_no_time(INFO, CMD_GENERAL, false, "> %s scheduled %d", parsed_opcode, action_id);
                }
                break;
            case OBC_CMD_FAILED_TO_SCHEDULE:
                log_str(ERROR, CMD_GENERAL, false, "> %s sched err", parsed_opcode);
                break;
            default:
                // Should never get here.
                log_str(ERROR, CMD_GENERAL, false, "> %s unknown err", parsed_opcode);
                break;
        }
        return invoke_err;
    } else {
        // There were issues parsing the command, so it was not invoked.
        switch (err) {
            case OBC_CMD_SYNTAX_ERROR:
                log_str(ERROR, CMD_GENERAL, true, "> %s invalid time fmt", parsed_opcode);
                break;
            case OBC_CMD_NO_MATCHING_CMD_NAME:
                log_str(ERROR, CMD_GENERAL, true, "> %s invalid cmd name", parsed_opcode);
                break;
            case OBC_CMD_NO_INTERVAL:
                log_str(ERROR, CMD_GENERAL, true, "> %s no interval", parsed_opcode);
                break;
            case OBC_CMD_TIME_ERR:
                log_str(ERROR, CMD_GENERAL, true, "> %s invalid epoch", parsed_opcode);
                break;
            case OBC_SEQ_NUM_INVALID:
                log_str(ERROR, CMD_GENERAL, true, "> %s invalid seq num", parsed_opcode);
                break;
            default:
                // Should never get here.
                log_str(ERROR, CMD_GENERAL, true, "> %s parse err %d", parsed_opcode, err);
                break;
        }
        return err;
    }
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief delegate the execution of a command to the scheduler
 *
 * @param[in] cmd: a command to be executed
 * @param[out] action_id: the action-id returned from the scheduler
 * @param[in] immediate: whether or not the command should first execute immediately.
 * @param[in] source: INTERNAL or EXTERNAL, indicating whether the command came from the OBC itself
 * or not.
 *
 * @returns OBC_IMMEDIATE_SUCCESS or OBC_SCHEDULED_SUCCESS if command is successfully scheduled,
 * otherwise OBC_CMD_FAILED_TO_SCHEDULE.
 */
static obc_cmd_err_t invoke(cmd_t* cmd, action_id_t* action_id, bool immediate, scheduler_submission_source_t source) {
    uint32_t interval         = cmd->interval;
    real_time_t start_at      = cmd->start_at;
    scheduler_action_t action = cmd->action;
    uint8_t argument_len      = cmd->argument.len;
    char* argument_data       = cmd->argument.data;

    scheduler_err_t err = SCHEDULER_SUCCESS;

    if (immediate) {
        err = scheduler_schedule_action_start_now(action_id, source, interval, action, argument_len, argument_data);
    } else {
        err = scheduler_schedule_action(action_id, source, start_at, interval, action, argument_len, argument_data);
    }

    if (err != SCHEDULER_SUCCESS) {
        return OBC_CMD_FAILED_TO_SCHEDULE;
    }
    return immediate ? OBC_IMMEDIATE_SUCCESS : OBC_SCHEDULED_SUCCESS;
}

/**
 * @brief finds an action associated with a command name, either a regular
 * command action or a critical command action.
 *
 * @param[in] name: the command name (opcode)
 * @param[out] action: the action associated with the command name, populated with a function
 * pointer.
 *
 * @retval OBC_CMD_FOUND if command is found that should be executed with the scheduler.
 * @retval OBC_COMMAND_CRITICAL if a command is found that should be executed without the scheduler.
 * @retval OBC_CMD_NO_MATCHING_CMD_NAME if no commands with a matching opcode are found.
 */
static obc_cmd_err_t find_command(char* name, scheduler_action_t* action) {
    uint32_t idx = 0;

    // No critical commands were found, so check regular commands.
    for (idx = 0; idx < num_cmds; idx++) {
        if (strcmp(name, command_table[idx].name) == 0) {
            *action = command_table[idx].func;
            return OBC_COMMAND_FOUND;
        }
    }

    return OBC_CMD_NO_MATCHING_CMD_NAME;
}

/**
 * @brief parses a command string
 *
 * @param[out] cmd: holds the resulting command
 * @param[in] input_cmd_str: a char sequence representing a command
 * @param[out] opcode_out: populated with a pointer to the start of
 * the opcode if there is one. Otherwise NULL and an error will be returned.
 *
 * @returns OBC_IMMEDIATE_SUCCESS if command is successfully parsed and is immediate
 * @returns OBC_SCHEDULED_SUCCESS if command is successfully parsed and is scheduled
 * @returns OBC_CMD_NO_INTERVAL if the command does not have an interval
 * @returns OBC_CMD_SYNTAX_ERROR if the command has an issue with its format
 * @returns OBC_CMD_CRITICAL if the command is a critical command
 */
static obc_cmd_err_t parse(cmd_t* cmd, char* input_cmd_str, char** opcode_out) {
    *opcode_out        = NULL; // Set the default output
    const char delim[] = " ";
    char *token, *saveptr = NULL;
    token = orca_strtok_r(input_cmd_str, delim, &saveptr);

    // Get and do a basic check on the sequence number.
    // Further checking is done after the opcode is known.
    int32_t sequence_num = cseq_to_num(token);
    if ((token == NULL) || (sequence_num == -1)) {
        return OBC_CMD_SYNTAX_ERROR;
    }

    // Get the opcode and set the opcode output
    token = orca_strtok_r(NULL, " ", &saveptr);
    if (token == NULL) {
        return OBC_CMD_SYNTAX_ERROR;
    }
    *opcode_out = token; // Update the opcode output with a pointer to what we think is the opcode.

    // Now check the sequence number for validity.
    // This is done after checking for an opcode because
    // the invalid command sequence number error message
    // also needs the opcode string.
    if (!check_seq_num(sequence_num)) {
        return OBC_SEQ_NUM_INVALID;
    }
    // It is safe to apply the int32 to the uint8_t sequence_num field here
    // because it was just checked to be within range.
    cmd->sequence_num = (uint8_t)sequence_num;

    // Determine if the command opcode is known
    obc_cmd_err_t err = find_command(token, &cmd->action);
    if (err != OBC_COMMAND_FOUND) {
        return err; // Command is invalid or is a critical command
    }

    // Parse until the next space (immediate) or '-' (scheduled).
    token = orca_strtok_r(NULL, " -", &saveptr);
    if (token == NULL) {
        return OBC_CMD_SYNTAX_ERROR;
    }

    // If there is a * in the place for the date, it's an immediate command.
    bool immediate = false;
    if (*token == '*') {
        cmd->start_at = orca_time_init;
        immediate     = true;
    } else {
        // It's a scheduled command, so assemble the start date and time.
        int32_t val = 0;

        // Year
        val   = cseq_to_num(token);
        token = orca_strtok_r(NULL, "-", &saveptr);
        if ((!is_year_valid(val)) || (token == NULL) || (val == -1)) {
            return OBC_CMD_SYNTAX_ERROR;
        }
        cmd->start_at.year = val;

        // Month
        val   = cseq_to_num(token);
        token = orca_strtok_r(NULL, " ", &saveptr);
        if ((!is_month_valid(val)) || (token == NULL) || (val == -1)) {
            return OBC_CMD_SYNTAX_ERROR;
        }
        cmd->start_at.month = val;

        // Day
        val   = cseq_to_num(token);
        token = orca_strtok_r(NULL, ":", &saveptr);
        if ((!is_day_valid(cmd->start_at.year, cmd->start_at.month, val)) || (token == NULL) || (val == -1)) {
            return OBC_CMD_SYNTAX_ERROR;
        }
        cmd->start_at.day = val;

        // Hour
        val   = cseq_to_num(token);
        token = orca_strtok_r(NULL, ":", &saveptr);
        if ((!is_hour_valid(val)) || (token == NULL) || (val == -1)) {
            return OBC_CMD_SYNTAX_ERROR;
        }
        cmd->start_at.hour = val;

        // Minute
        val   = cseq_to_num(token);
        token = orca_strtok_r(NULL, " ", &saveptr);
        if ((!is_minute_valid(val)) || (token == NULL) || (val == -1)) {
            return OBC_CMD_SYNTAX_ERROR;
        }
        cmd->start_at.minute = val;

        // Second
        val = cseq_to_num(token);
        if ((!is_second_valid(val)) || (val == -1)) {
            return OBC_CMD_SYNTAX_ERROR;
        }
        cmd->start_at.second = val;
    }

    // Extract the interval
    token = orca_strtok_r(NULL, " ", &saveptr);
    if (token == NULL) {
        return OBC_CMD_NO_INTERVAL;
    }
    cmd->interval = cseq_to_num(token); // TODO: this works, but we should probably rethink our validation strategy

    // Extract up to NUM_ARGUMENTS arguments
    token = orca_strtok_r(NULL, ".", &saveptr);
    if (token != NULL) {
        cmd->argument.data = token;
        cmd->argument.len  = strlen(token);
    } else {
        cmd->argument.data = NULL;
        cmd->argument.len  = 0;
    }

    // Update the epoch inside the real_time_t scheduled time for the command.
    // Abort if the conversion did not succeed.
    cmd->start_at.epoch = real_time_to_epoch(&cmd->start_at);
    if (cmd->start_at.epoch == no_epoch) {
        return OBC_CMD_TIME_ERR;
    }

    return immediate ? OBC_IMMEDIATE_SUCCESS : OBC_SCHEDULED_SUCCESS;
}

/**
 * @brief Checks a sequence number for validity.
 *
 * Sequence numbers equal to 0 or the next valid sequence number are
 * valid.
 *
 * @param sequence_num The sequence number contained in the just-received command to check.
 * @return whether or not the sequence number is valid and the command should be processed further.
 */
static bool check_seq_num(int32_t sequence_num) {
    if ((sequence_num == 0) || (sequence_num == next_valid_seq_num)) {
        return true;
    }
    return false;
}

/**
 * @brief Increments the sequence number to the next value.
 *
 * Once the sequence number hits 255, it rolls back to 1 at
 * the next increment because commands define the sequence
 * number as an 8-bit value.
 */
static void increment_seq_num(void) {
    // Increment or roll back to zero.
    if (next_valid_seq_num == 255) {
        next_valid_seq_num = 1;
    } else {
        next_valid_seq_num++;
    }
}

/**
 * @brief Parses a sequence of space separated values into an array
 * @param data string to be parsed
 * @param num number of arguments to read
 * @param args where to store the results
 * @returns the number of arguments read
 */
uint8_t obc_cmd_read_str_arguments(char* data, uint8_t num, char** args) {
    if (data == NULL) {
        return 0;
    }

    uint8_t i = 0;
    char *token, *saveptr = NULL;
    token = orca_strtok_r(data, " ", &saveptr);

    while ((token != NULL) && (i < num)) {
        args[i] = token;
        token   = orca_strtok_r(NULL, " ", &saveptr);
        i += 1;
    }

    return i;
}
