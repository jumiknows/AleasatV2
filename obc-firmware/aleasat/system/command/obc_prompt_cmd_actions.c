/** @file obc_normal_cmd_actions.c
 *  @brief OBC prompt command function implementations.
 *
 * Prompt commands must only ever return a single logger message, and the
 * formatting of the return message should be done in this file so it is
 * easy to audit.
 *
 * Errors logged as side effects of a command are logged to the filesystem
 * automatically, but command implementations must log a single error message
 * indicating the overall result here.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_prompt_cmd_actions.h"
#include "obc_featuredefs.h"
#include "obc_watchdog.h"
#include "obc_time.h"
#include "logger.h"
#include "backup_epoch.h"
#include "rtc_cmds.h"
#include "rtc_state.h"
#include "hang.h"
#include "filesystem.h"
#include "low_power.h"
#include "obc_utils.h"
#include "obc_temperature.h"
#include "tms_i2c.h"
#include "telem_obc.h"
#include "obc_settings.h"
#include "obc_hardwaredefs.h"
#if !defined(PLATFORM_ALEA_V1)
#include "telem_eps.h"
#endif
#include "obc_cmd.h"
#include "obc_rtos.h"
#include "obc_task_info.h"
#include "obc_gpio.h"
#include "nvct.h"

// Standard Library
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

// Third Part
#include "printf.h"

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Stores the most recently called logging information,
 * which can be queried with the last_response command.
 */
struct Last_Logged {
    log_level_t log_lvl;
    log_identifier_t log_id;
    char string[MAX_PAYLOAD_SIZE + 1]; // + 1 to contain the null terminator that is ignored by the logger but is inserted by string methods.
};

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief A counter that is incremented by the "count" command.
 * This is a general purpose utility that is useful for testing
 * and demonstration purposes.
 */
static uint32_t demo_counter = 0;

struct Last_Logged last_logged = {.log_lvl = INFO, .log_id = PRINT_GENERAL, .string = "first"};

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void prompt_cmd_response(log_level_t lvl, log_identifier_t id, bool write_to_file, char* format, ...);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

// ------------------------ SYSTEM PROMPT COMMAND IMPLEMENTATION -----------------------

/**
 * @brief Acknowledges with a single logger message.
 */
void cmd_ack(uint32_t arg_len, void* arg) {
    prompt_cmd_response(INFO, ACK_CMD, true, "Ack!");
}

/**
 * @brief Ping command for Houston to detect the OBC.
 * The message returned by this command is not logged in the filesystem.
 */
void cmd_ping(uint32_t arg_len, void* arg) {
    // NOTE: tests assume that this command does not log to the filesystem.
    prompt_cmd_response(INFO, PING_CMD, false, "Ping");
}

/**
 * @brief Prints out the board type.
 */
void cmd_get_board_type(uint32_t arg_len, void* arg) {
    prompt_cmd_response(INFO, BOARD_ID, true, BOARD_TYPE_MSG);
}

/**
 * @brief Resets the OBC using the watchdog.
 */
void cmd_watchdog_reset(uint32_t arg_len, void* arg) {
    watchdog_reset();
}

/**
 * @brief Gets the next valid sequence number from the OBC.
 *
 * The number returned is suitable for using with the next command
 * to the OBC, it is not the sequence number of this command.
 */
void cmd_get_sequence_num(uint32_t arg_len, void* arg) {
    prompt_cmd_response(INFO, SEQ_NUM_CMD, false, "%d", obc_cmd_next_seq_num());
}

/**
 * @brief Gets the response of the previous prompt command.
 *
 * Does not update the @ref last_response itself. Executing this
 * command multiple times will always return the response from
 * the last prompt command that wasn't this one.
 */
void cmd_last_response(uint32_t arg_len, void* arg) {
    log_str(last_logged.log_lvl, last_logged.log_id, false, "%s", last_logged.string);
}

/**
 * @brief Reset temperature sensor
 */
void cmd_temp_reset(uint32_t arg_len, void* arg) {
    reset_temp();
    prompt_cmd_response(INFO, PRINT_GENERAL, false, "Reset temp sensor");
}

/**
 * @brief Reset GPIO expander
 */
void cmd_expander_reset(uint32_t arg_len, void* arg) {
    gpio_expander_reset();
    prompt_cmd_response(INFO, PRINT_GENERAL, false, "Reset GPIO expander");
}

/**
 * @brief Reset I2C bus
 */
void cmd_i2c_reset(uint32_t arg_len, void* arg) {
    const i2c_err_t err = tms_i2c_reset(0, 500); // 0 retires, 500 ms mutex timeout
    if (err == I2C_SUCCESS) {
        prompt_cmd_response(INFO, I2C_LOG, false, "I2C bus reset");
    } else {
        prompt_cmd_response(INFO, I2C_LOG, false, "I2C reset failed: %d", err);
    }
}

// ------------------------ TIME PROMPT COMMAND IMPLEMENTATION -----------------------

/**
 * @brief The time in format yy-mm-dd hh:mm:ss
 */
void cmd_get_time(uint32_t arg_len, void* arg) {
    real_time_t curr_time = {0};
    rtc_err_t err         = rtc_get_current_time(&curr_time);
    if (err == RTC_NO_ERR) {
        prompt_cmd_response(INFO, GET_EPOCH_CMD, true, "%d-%d-%d %d:%d:%d", curr_time.year, curr_time.month, curr_time.day, curr_time.hour, curr_time.minute, curr_time.second);
    } else {
        prompt_cmd_response(ERROR, GET_EPOCH_CMD, true, "Cannot read time.");
    }
}

/**
 * @brief The time in epoch format.
 * Seconds since 12:00 AM on January 1, 2020.
 */
void cmd_get_epoch(uint32_t arg_len, void* arg) {
    epoch_t epoch = rtc_get_epoch_time();
    if (epoch != -1) {
        prompt_cmd_response(INFO, GET_EPOCH_CMD, true, "Epoch: %d", epoch);
    } else {
        prompt_cmd_response(ERROR, GET_EPOCH_CMD, true, "Epoch invalid.");
    }
}

/**
 * @brief Log current epoch and real time.
 * Format is: epoch yy-mm-dd hh:mm:ss
 */
void cmd_get_epoch_and_time(uint32_t arg_len, void* arg) {
    real_time_t curr_time = {0};
    rtc_err_t err         = rtc_get_current_time(&curr_time);
    if (err == RTC_NO_ERR) {
        prompt_cmd_response(INFO, GET_EPOCH_CMD, true, "%d %d-%d-%d %d:%d:%d", curr_time.epoch, curr_time.year, curr_time.month, curr_time.day, curr_time.hour, curr_time.minute, curr_time.second);
    } else {
        prompt_cmd_response(ERROR, GET_EPOCH_CMD, true, "Cannot read time.");
    }
}

/**
 * @brief The backup epoch.
 */
void cmd_get_backup_epoch(uint32_t arg_len, void* arg) {
    epoch_t backup_epoch = get_backup_epoch();
    prompt_cmd_response(INFO, GET_BACKUP_EPOCH_CMD, true, "Backup Epoch: %d", backup_epoch);
}

// ------------------------ RTC PROMPT COMMAND IMPLEMENTATION -----------------------

/**
 * @brief Set the time on RTCs.
 */
void cmd_set_time(uint32_t arg_len, void* arg) {
    char* arguments[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    uint8_t num_args   = obc_cmd_read_str_arguments(arg, 7, arguments);
    if (num_args == 7) {
        command_update_rtc_time(arguments);
    } else {
        prompt_cmd_response(ERROR, RTC_LOG, true, "Too few args. Need rtc y m d h m s.");
    }
}

/**
 * @brief Update which RTCs are active (used for timekeeping).
 */
void cmd_set_rtc(uint32_t arg_len, void* arg) {
    char* arguments[1] = {NULL};
    uint8_t num_args   = obc_cmd_read_str_arguments(arg, 1, arguments);
    if (num_args == 1) {
        command_update_active_rtc(arg);
    } else {
        prompt_cmd_response(ERROR, RTC_LOG, true, "Too few args. Need rtc.");
    }
}

/**
 * @brief Sets the RTC into timestamp mode.
 *
 * This command accepts a first argument of 1 or 0 to enable/disable timestamp mode.
 *
 * rtc_ts 1 will enable timestamp mode
 * rtc_ts 0 will disable timestamp mode
 */
void cmd_rtc_ts(uint32_t arg_len, void* arg) {
    char* arguments[1] = {NULL};
    uint8_t num_args   = obc_cmd_read_str_arguments(arg, 1, arguments);
    if (num_args == 1) {
        int8_t ts = (int8_t) cseq_to_num((char*)arg);
        if (ts == -1) {
            log_str(ERROR, RTC_LOG, true, "Invalid ts arg %s", (char*)arg);
            return;
        }

        if ((ts == 1) || (ts == 0)) {
            rtc_set_ts(ts);
            ts ? prompt_cmd_response(INFO, RTC_LOG, true, "RTC mode: ts") : prompt_cmd_response(INFO, RTC_LOG, false, "RTC mode: reg");
        } else {
            prompt_cmd_response(ERROR, RTC_LOG, true, "Invalid ts arg %s", (const char*)arg);
        }
    } else {
        prompt_cmd_response(ERROR, RTC_LOG, true, "Usage: rtc_ts 1");
    }
}

// ------------------------ TESTING PROMPT COMMAND IMPLEMENTATION -----------------------

/**
 * @brief Initiates file-system test.
 */
void cmd_exec_fs_test(uint32_t arg_len, void* arg) {
    // TODO
}

/** @brief Count command, increments a counter. This can be
 * used to determine how many times this command was executed,
 * for testing and demonstration purposes.
 */
void cmd_count(uint32_t arg_len, void* arg) {
    demo_counter++;
}

/** @brief Count command, increments a counter. This can be
 * used to determine how many times this command was executed,
 * for testing and demonstration purposes.
 */
void cmd_get_count(uint32_t arg_len, void* arg) {
    prompt_cmd_response(INFO, PRINT_GENERAL, false, "Count: %d", demo_counter);
}

/**
 * @brief Change the task status of the hang task, causes task to hang.
 * Should be caught by the watchdog.
 */
void cmd_run_hang_task(uint32_t arg_len, void* arg) {
    if (set_hang_task_status_alive()) {
        prompt_cmd_response(INFO, HANG_TASK_LOG, false, "Hanging a test task.");
    } else {
        prompt_cmd_response(ERROR, HANG_TASK_LOG, true, "Error when hanging test task.");
    }
}

/**
 * @brief Prints whatever data is provided as an argument.
 */
void cmd_echo_to_log(uint32_t arg_len, void* arg) {
    char* arguments[1] = {NULL};
    uint8_t num_args   = obc_cmd_read_str_arguments(arg, 1, arguments);
    if (num_args == 1) {
        prompt_cmd_response(INFO, ECHO_CMD, true, arguments[0]);
    } else {
        prompt_cmd_response(ERROR, ECHO_CMD, true, "Can only echo single word.");
    }
}

// ------------------------ POWER MODE PROMPT COMMAND IMPLEMENTATION -----------------------

/**
 * @brief Enables low power mode.
 */
void cmd_LP_on(uint32_t arg_len, void* arg) {
    prompt_cmd_response(INFO, PRINT_GENERAL, true, "Enabled low power!");
    idle_sleep_on();
}

/**
 * @brief Disables low power mode.
 */
void cmd_LP_off(uint32_t arg_len, void* arg) {
    prompt_cmd_response(INFO, PRINT_GENERAL, true, "Disabled low power!");
    idle_sleep_off();
}

// ------------------------ RTOS PROMPT COMMAND IMPLEMENTATION -----------------------

/**
 * @brief Suspends a task by name
 * @param[in] arg_len:	The length of the argument buffer in bytes
 * @param[in] arg:		Character array of 20 bytes for each ASCII argument delimited by a space.
 * Should contain the task name
 *
 * */
void cmd_rtos_suspend(uint32_t arg_len, void* arg) {
    if (suspend_task((char*)arg, true) != OBC_RTOS_OK) {
        prompt_cmd_response(ERROR, OBC_TASK, false, "Invalid task name");
    }
}
/**
 * @brief Resumes a task by name
 * @param[in] arg_len:	The length of the argument buffer in bytes
 * @param[in] arg:		Character array of 20 bytes for each ASCII argument delimited by a space.
 * Should contain the task name
 *
 * */
void cmd_rtos_resume(uint32_t arg_len, void* arg) {
    if (suspend_task((char*)arg, false) != OBC_RTOS_OK) {
        prompt_cmd_response(ERROR, OBC_TASK, false, "Invalid task name");
    }
}

/**
 * @brief Check or modify the period of a task by name.
 * To check period: rtos_period blinky
 * To modify period: rtos_period blinky 500
 * @param arg_len:	The length of the argument buffer in bytes
 * @param arg:		Character array of 20 bytes for each ASCII argument delimited by a space
 *
 * */
void cmd_rtos_period(uint32_t arg_len, void* arg) {
    char* arguments[3] = {NULL, NULL, NULL};
    uint8_t num_args   = obc_cmd_read_str_arguments(arg, 3, arguments);

    if (num_args == 0) {
        prompt_cmd_response(ERROR, OBC_TASK, true, "Task name not found");
        return;
    }
    uint8_t id      = 0;
    char* task_name = arguments[0];
    if (get_task_id_from_name(task_name, &id) != OBC_RTOS_OK) {
        prompt_cmd_response(ERROR, OBC_TASK, true, "Task ID %d not found", id);
        return;
    }
    task_period_t period_ms = 0;
    if (num_args == 1) { // only have task name in cmd. return current period
        prompt_cmd_response(INFO, OBC_TASK, true, "Task %s per %d", task_name, period_ms);
        return;
    } else if (num_args != 2) {
        prompt_cmd_response(ERROR, OBC_TASK, true, "1 or 2 arguments required");
        return;
    }
    char* task_period = arguments[1];
    if (task_period == NULL) {
        prompt_cmd_response(ERROR, OBC_TASK, true, "Task period not found");
        return;
    }

    period_ms = cseq_to_num(task_period);
    if (period_ms == -1) {
    	log_str(ERROR, OBC_TASK, true, "Invalid period %s", (char*)arg);
        return;
    }

    if (period_ms <= 0) {
        prompt_cmd_response(ERROR, OBC_TASK, true, "Task period not valid");
        return;
    }
    // cseq will not handle negative integers or non integers correctly
    // check that period is greater than zero since it cannot
    // be zero, that is reserved for aperiodic tasks
    // changing task period of aperiodic tasks should return an err
    if (set_task_period(id, period_ms) != OBC_RTOS_OK) {
        prompt_cmd_response(ERROR, OBC_TASK, true, "Task is not periodic");
    }
}

/**
 * @brief Prints the state of a task by name
 * @param[in] arg_len:	The length of the argument buffer in bytes
 * @param[in] arg:		Character array of 20 bytes for each ASCII argument delimited by a space.
 * Should contain the task name
 */
void cmd_rtos_state(uint32_t arg_len, void* arg) {
    if (print_task_state((char*)arg) != OBC_RTOS_OK) {
        prompt_cmd_response(ERROR, OBC_TASK, false, "Invalid task name");
    }
}

// ------------------------ FILESYSTEM PROMPT COMMAND IMPLEMENTATION -----------------------

/**
 * @brief Create a new directory
 *            Arg 0 - directory path
 */
void cmd_fs_mkdir(uint32_t arg_len, void* arg) {
    if (arg_len == 0) {
        prompt_cmd_response(DEBUG, FS_LOG, false, "Usage: mkdir [PATH]");
    } else {
        fs_err_t err = fs_mkdir((const char*)arg);

        if (err != FS_OK) {
            prompt_cmd_response(ERROR, FS_LOG, true, "Filesystem error: %d", err);
        } else {
            prompt_cmd_response(INFO, FS_LOG, true, "Directory successfully created.");
        }
    }
}

/**
 * @brief Remove a file or directory
 *            Arg 0 - file / directory path
 */
void cmd_fs_rm(uint32_t arg_len, void* arg) {
    if (arg_len == 0) {
        prompt_cmd_response(DEBUG, FS_LOG, false, "Usage: rm [PATH]");
    } else {
        fs_err_t err = fs_delete((const char*)arg);

        if (err != FS_OK) {
            prompt_cmd_response(ERROR, FS_LOG, true, "Filesystem error: %d", err);
        } else {
            prompt_cmd_response(INFO, FS_LOG, true, "Item successfully removed.");
        }
    }
}

/**
 * @brief Create a new file
 *            Arg 0 - file path
 */
void cmd_fs_create(uint32_t arg_len, void* arg) {
    if (arg_len == 0) {
        prompt_cmd_response(DEBUG, FS_LOG, false, "Usage: mkfile [PATH]");
    } else {
        fs_err_t err = fs_create((const char*)arg);

        if (err != FS_OK) {
            prompt_cmd_response(ERROR, FS_LOG, true, "Filesystem error: %d", err);
        } else {
            prompt_cmd_response(INFO, FS_LOG, true, "File successfully created.");
        }
    }
}

/**
 * @brief Get size of an existing file
 *            Arg 0 - file path
 */
void cmd_fs_file_size(uint32_t arg_len, void* arg) {
    if (arg_len == 0) {
        prompt_cmd_response(DEBUG, FS_LOG, false, "Usage: get_file_size [PATH]");
    } else {
        uint32_t size = 0;
        fs_err_t err  = fs_file_size((const char*)arg, &size);

        if (err != FS_OK) {
            prompt_cmd_response(ERROR, FS_LOG, true, "Filesystem error: %d", err);
        } else {
            prompt_cmd_response(INFO, FS_LOG, true, "File Size: %d", size);
        }
    }
}

/**
 * @brief Get current size of the filesystem in bytes
 */
void cmd_fs_size(uint32_t arg_len, void* arg) {
    prompt_cmd_response(INFO, FS_LOG, true, "FS Size: %d", fs_size());
}

// ------------------------ MISC TELEMETRY PROMPT COMMAND IMPLEMENTATION -----------------------

/**
 * @brief Gets the onboard temperature of the OBC.
 */
void cmd_get_temperature(uint32_t arg_len, void* arg) {
    int16_t temp = 0;
    if (read_temp(&temp) == TEMP_SUCCESS) {
        prompt_cmd_response(INFO, TEMP_LOG, true, "Temperature: %d C", temp);
    } else {
        prompt_cmd_response(ERROR, TEMP_LOG, true, "Temperature read error.");
    }
}

// ---------------------------- Periodic Action -----------------------

void cmd_get_period(uint32_t arg_len, void* arg) {
    char* args[1]    = {NULL};
    uint8_t num_args = obc_cmd_read_str_arguments(arg, 1, args);
    if (num_args == 0) {
        int8_t action_id      = (int8_t) cseq_to_num((char*)arg);
        if (action_id == -1) {
            log_str(ERROR, RTC_LOG, true, "Invalid action id: %s", (char*)arg);
            return;
       	}

        uint32_t period            = 0;
        scheduler_err_t result = scheduler_get_period(action_id, &period);
        if (result == SCHEDULER_SUCCESS) {
            prompt_cmd_response(INFO, PRINT_GENERAL, false, "Task Period %d", period);
        } else {
            prompt_cmd_response(INFO, PRINT_GENERAL, false, "Scheduler error: %d", result);
        }
    } else {
        prompt_cmd_response(ERROR, RTC_LOG, true, "Usage: get_period [action ID]");
    }
}

void cmd_set_period(uint32_t arg_len, void* arg) {
    char* args[2]    = {NULL, NULL};
    uint8_t num_args = obc_cmd_read_str_arguments(arg, 2, args);
    if (num_args == 2) {
    	int8_t action_id = cseq_to_num(args[0]);
        if (action_id == -1) {
            log_str(ERROR, RTC_LOG, true, "Invalid action id: %s", (char*)arg);
            return;
        }

        int32_t period = cseq_to_num(args[1]);
        if (period == -1) {
            log_str(ERROR, RTC_LOG, true, "Invalid period: %s", (char*)arg);
            return;
        }

        scheduler_err_t result = scheduler_set_period(action_id, period);
        if (result != SCHEDULER_SUCCESS) {
            prompt_cmd_response(INFO, PRINT_GENERAL, false, "Scheduler error: %d", result);
        }
        prompt_cmd_response(INFO, PRINT_GENERAL, false, "period changed %d, %d", action_id, period);
    } else {
        prompt_cmd_response(ERROR, RTC_LOG, true, "Usage: set_period [action ID] [new interval]");
    }
}

// ---------------------------- NVCT and Settings -----------------------

/**
 * @brief Erases the NVCT at the given table index.
 *
 * Expected usage: nvct_erase [table index]
 */
void cmd_erase_nvct(uint32_t arg_len, void* arg) {
    char* args[1]    = {NULL};
    uint8_t num_args = obc_cmd_read_str_arguments(arg, 1, args);
    if (num_args == 1) {
    	int32_t table_index = cseq_to_num(args[0]);
        if (table_index == -1) {
            log_str(ERROR, SETTINGS_LOG, true, "Invalid table index: %s", (char*)arg);
            return;
        }

        nvct_err_enum_t err  = erase_nvct_table(table_index);
        switch (err) {
            case NVCT_SUCCESS:
                prompt_cmd_response(INFO, SETTINGS_LOG, true, "NVCT %d erased", table_index);
                break;
            case NVCT_TABLE_INDEX_ERROR:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "Table %d invalid", table_index);
                break;
            case NVCT_FAILED_TO_WRITE_TO_MRAM_ERROR:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "MRAM error");
                break;
            default:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "Unknown error: %d", err);
                break;
        }
    } else {
        prompt_cmd_response(ERROR, SETTINGS_LOG, false, "Usage: erase_nvct [table index]");
    }
}

/**
 * @brief Provisions an NVCT with a snapshot of the OBC's settings.
 *
 * Expected usage: nvct_provision [table index]
 */
void cmd_provision_nvct(uint32_t arg_len, void* arg) {
    char* args[1]    = {NULL};
    uint8_t num_args = obc_cmd_read_str_arguments(arg, 1, args);
    if (num_args == 1) {
        int32_t table_index = cseq_to_num((char*)arg);
        if (table_index == -1) {
            log_str(ERROR, SETTINGS_LOG, true, "Invalid table index: %s", (char*)arg);
            return;
        }

        setting_err_t err = provision_new_settings_table(table_index);
        switch (err) {
            case SETTING_OK:
                prompt_cmd_response(INFO, SETTINGS_LOG, true, "Provision OK %d", table_index);
                break;
            case SETTING_NVCT_UPDATE_ERR:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "NVCT failed");
                break;
            case INVALID_SETTING_NAME:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "No fw_version!");
                break;
            default:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "Unknown error: %d", err);
                break;
        }
    } else {
        prompt_cmd_response(ERROR, SETTINGS_LOG, false, "Usage: erase_nvct [table index]");
    }
}

/**
 * @brief Gets the value of a setting.
 *
 * Expected usage: setting_get [setting name]
 */
void cmd_get_setting(uint32_t arg_len, void* arg) {
    char* args[1]    = {NULL};
    uint8_t num_args = obc_cmd_read_str_arguments(arg, 1, args);
    if (num_args == 1) {
        uint32_t val             = 0;
        setting_nvct_t nvct_info = NOT_NVCT;
        setting_err_t err        = get_uint32_from_command((const char*)arg, &val, &nvct_info);

        switch (err) {
            case SETTING_OK:
                prompt_cmd_response(INFO, SETTINGS_LOG, false, "%s %d (%d)", (const char*)arg, val, nvct_info);
                break;
            case INVALID_SETTING_NAME:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "%s not found", (const char*)arg);
                break;
            default:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "Unknown error: %d (%d)", err, nvct_info);
                break;
        }
    } else {
        prompt_cmd_response(ERROR, SETTINGS_LOG, false, "Usage: get_setting [name]");
    }
}

/**
 * @brief Sets the value of a setting.
 *
 * Expected usage: setting_get [setting name] [setting value]
 */
void cmd_set_setting(uint32_t arg_len, void* arg) {
    char* args[1]    = {NULL};
    uint8_t num_args = obc_cmd_read_str_arguments(arg, 2, args);
    if (num_args == 2) {
        const char* setting = args[0];
        int32_t val        = cseq_to_num(args[1]);
        if (val == -1) {
            log_str(ERROR, SETTINGS_LOG, true, "Invalid val: %s", (char*)arg);
            return;
        }

        setting_err_t err   = set_uint32_from_command(setting, val);

        switch (err) {
            case SETTING_OK:
                prompt_cmd_response(INFO, SETTINGS_LOG, false, "%s %d OK", (const char*)arg, val);
                break;
            case INVALID_SETTING_NAME:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "%s not found", (const char*)arg);
                break;
            case INVALID_SETTING_VALUE:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "%s invalid val", (const char*)arg);
                break;
            case SETTING_CALLBACK_FAILED:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "%s callback failed", (const char*)arg);
                break;
            case SETTING_IMMUTABLE:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "%s immutable", (const char*)arg);
                break;
            case SETTING_NVCT_UPDATE_ERR:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "%s NVCT err", (const char*)arg);
                break;
            default:
                prompt_cmd_response(ERROR, SETTINGS_LOG, true, "Unknown error: %d", err);
                break;
        }
    } else {
        prompt_cmd_response(ERROR, SETTINGS_LOG, false, "set_setting [name] [value]");
    }
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

// ------------------------ LAST RESPONSE PROMPT COMMAND -----------------------
/**
 * @brief Logs the response to a prompt command. The details of the
 * response are saved so that they can be queried with the last_response
 * command.
 *
 * @param lvl The logging system level.
 * @param id The log ID.
 * @param write_to_file Whether or not to log the data to a file.
 * @param format Printf-style format string.
 */
static void prompt_cmd_response(log_level_t lvl, log_identifier_t id, bool write_to_file, char* format, ...) {
    last_logged.log_lvl = lvl;
    last_logged.log_id  = id;
    va_list va;
    OBC_MISRA_CHECK_OFF
    va_start(va, format);
    OBC_MISRA_CHECK_ON
    orca_vsnprintf(last_logged.string, MAX_PAYLOAD_SIZE + 1, format, va);
    va_end(va);
    log_str(lvl, id, write_to_file, "%s", last_logged.string);
}
