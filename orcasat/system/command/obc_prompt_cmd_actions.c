/** @file obc_normal_cmd_actions.c
 *  @brief OBC prompt command function implementations.
 *  @author ORCASat C&DH team
 *  @date 2020-07-06
 *
 * Prompt commands must only ever return a single logger message, and the
 * formatting of the return message should be done in this file so it is
 * easy to audit.
 *
 * Errors logged as side effects of a command are logged to the filesystem
 * automatically, but command implementations must log a single error message
 * indicating the overall result here.
 */

#include "obc_prompt_cmd_actions.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"
#include "obc_featuredefs.h"
#include "obc_watchdog.h"
#include "obc_time.h"
#include "backup_epoch.h"
#include "rtc_cmds.h"
#include "rtc_state.h"
#include "unit_tests.h"
#include "hang.h"
#include "filesystem.h"
#include "low_power.h"
#include "obc_utils.h"
#include "obc_temperature.h"
#include "telem_obc.h"
#if !defined(PLATFORM_ALEA_V1)
#include "telem_eps.h"
#endif
#include "obc_cmd.h"
#include "obc_rtos.h"
#include "obc_task_info.h"
#include "i2c_freertos.h"
#include "printf.h"
#include "obc_gpio.h"
#include "nvct.h"
#include "obc_settings.h"
#include "obc_hardwaredefs.h"

/**
 * @brief A counter that is incremented by the "count" command.
 * This is a general purpose utility that is useful for testing
 * and demonstration purposes.
 */
static uint32_t demo_counter = 0;

/**
 * @brief Stores the most recently called logging information,
 * which can be queried with the last_response command.
 */
struct Last_Logged {
    log_level_t log_lvl;
    log_identifier_t log_id;
    char string[MAX_PAYLOAD_SIZE + 1]; // + 1 to contain the null terminator that is ignored by the logger but is inserted by string methods.
};
struct Last_Logged last_logged = {.log_lvl = INFO, .log_id = PRINT_GENERAL, .string = "first"};

// Private Functions
static void prompt_cmd_response(log_level_t lvl, log_identifier_t id, bool write_to_file, char* format, ...);

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
 * @brief Current amount of free RTOS heap.
 */
void cmd_get_heap(uint32_t arg_len, void* arg) {
    size_t heap_size = xPortGetFreeHeapSize();
    prompt_cmd_response(INFO, GET_HEAP_CMD, true, "Heap: %d bytes", heap_size);
}

/**
 * @brief Minimum amount of free RTOS heap since boot.
 */
void cmd_get_minheap(uint32_t arg_len, void* arg) {
    size_t heap_size = xPortGetMinimumEverFreeHeapSize();
    prompt_cmd_response(INFO, GET_MINHEAP_CMD, true, "Minheap: %d bytes", heap_size);
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
    prompt_cmd_response(INFO, SEQ_NUM_CMD, false, "%d", next_seq_num());
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
    if (num_args(arg_len) == 7) {
        command_update_rtc_time(arg);
    } else {
        prompt_cmd_response(ERROR, RTC_LOG, true, "Too few args. Need rtc y m d h m s.");
    }
}

/**
 * @brief Update which RTCs are active (used for timekeeping).
 */
void cmd_set_rtc(uint32_t arg_len, void* arg) {
    if (num_args(arg_len) == 1) {
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
    if (num_args(arg_len) == 1) {
        uint8_t ts = cseq_to_num((char*)arg);
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
    test_filesystem();
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
    if (num_args(arg_len) == 1) {
        char buf[ARGUMENT_SIZE + 1] = {'\0'};
        memcpy(buf, arg, arg_len);
        prompt_cmd_response(INFO, ECHO_CMD, true, (char*)buf);
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
    const char delim[] = " ";
    char* task_name    = strtok((char*)arg, delim);
    if (task_name != NULL) {
        uint8_t id = 0;
        if (get_task_id_from_name(task_name, &id) == OBC_RTOS_OK) {
            task_period_t period_ms = 0;
            if (num_args(arg_len) == 1) { // only have task name in cmd. return current period
                if (get_task_period_from_id(id, &period_ms) == OBC_RTOS_OK) {
                    prompt_cmd_response(INFO, OBC_TASK, true, "Task %s per %d", task_name, period_ms);
                }
                // invalid ID logs error in get_task_period_from_id()
            } else if (num_args(arg_len) == 2) { // cmd has task name and period arg
                char* task_period = (char*)arg + ARGUMENT_SIZE;
                if (task_period != NULL) {
                    period_ms = cseq_to_num(task_period);                    // cseq will not handle negative integers
                                                                             // or non integers correctly
                    if (period_ms > 0) {                                     // check that period is greater than zero since it cannot
                                                                             // be zero, that is reserved for aperiodic tasks
                        if (set_task_period(id, period_ms) != OBC_RTOS_OK) { // changing task period of aperiodic tasks should return
                                                                             // an err
                            prompt_cmd_response(ERROR, OBC_TASK, true, "Task is not periodic");
                        }
                    } else {
                        prompt_cmd_response(ERROR, OBC_TASK, true, "Task period not valid");
                    }
                } else {
                    prompt_cmd_response(ERROR, OBC_TASK, true, "Task period not found");
                }
            } else {
                prompt_cmd_response(ERROR, OBC_TASK, true, "1 or 2 arguments required");
            }
        } else {
            prompt_cmd_response(ERROR, OBC_TASK, true, "Task ID %d not found", id);
        }
    } else {
        prompt_cmd_response(ERROR, OBC_TASK, true, "Task name not found");
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

// ------------------------ TELEMETRY COLLECTION PROMPT COMMAND IMPLEMENTATION -----------------------

/**
 * @brief Log all OBC internal slow-rate telemetry.
 */
void cmd_telem_obc_slow(uint32_t arg_len, void* arg) {
    if (num_args(arg_len) == 0) {
        obc_slow_telem_collect();
    }
}

/**
 * @brief Log all OBC internal fast-rate telemetry.
 */
void cmd_telem_obc_fast(uint32_t arg_len, void* arg) {
    if (num_args(arg_len) == 0) {
        obc_fast_telem_collect();
    }
}

#if !defined(PLATFORM_ALEA_V1)
/**
 * @brief Log all EPS slow-rate telemetry.
 */
void cmd_telem_eps_slow(uint32_t arg_len, void* arg) {
    if (num_args(arg_len) == 0) {
        eps_slow_telem_collect();
    }
}
#endif // !defined(PLATFORM_ALEA_V1)

/**
 * @brief Scans the I2C bus and prints the devices found on the bus
 *            Arg 0 - 'v' for verbose mode
 */
void cmd_i2c_scan(uint32_t arg_len, void* arg) {
    uint8_t i;
    i2c_err_t status;
    bool verbose = false;
    i2c_command_t command = {0, DEBUG_CMD_RECEIVE_QUEUE_ID, 0, SCAN, 0, NULL};
    if(strcmp((const char*)arg, "v") == 0) {
        verbose = true;
    }
    log_str(INFO, I2C_LOG, false, "starting i2cscan\n");
    // 0x00 is a reserved address, start at 0x01
    for(i=1; i < 127; i++) {
        command.destination = i;
        send_command_to_i2c_worker(&command, I2C_WORKER_DEFAULT_PRIORITY);
        xQueueReceive(debug_cmd_rx_q, (void*)&status, portMAX_DELAY);
        if(verbose) {
            log_str(INFO, I2C_LOG, false, "Scanned %02X, result: %d\n", i, status);
        }
        if(status == I2C_SUCCESS) {
            log_str(INFO, I2C_LOG, false, "Device detected at %02X\n", i);
        }
    }
    log_str(INFO, I2C_LOG, false, "finished i2cscan\n");
}

/**
 * @brief Read a value from an i2c device
 *            Arg 0 - 'XX' address of device to read from as two character hex
 *            Arg 1 - 'XX' address of register to read from as two character hex
 *
 */
void cmd_i2c_read(uint32_t arg_len, void* arg) {
    uint8_t dev_addr, reg_addr;
    uint8_t buffer[1] = {0};
    i2c_err_t status = I2C_QUEUE_FULL;
    i2c_command_t command = {0, DEBUG_CMD_RECEIVE_QUEUE_ID, 0, READ_DATA, 1, buffer};
    cmd_argument_t* args = (cmd_argument_t*)arg;
    if (num_args(arg_len) != 2) {
        log_str(DEBUG, I2C_LOG, false, "Usage: i2c_read ADDR REG\n");
        return;
    }
    dev_addr = strtoul((const char*)args[0], NULL, 16);
    reg_addr = strtoul((const char*)args[1], NULL, 16);
    command.destination = dev_addr;
    command.cmd = reg_addr;
	log_str(DEBUG, I2C_LOG, false, "i2c_read %x %x\n", dev_addr, reg_addr);
	send_command_to_i2c_worker(&command, I2C_WORKER_DEFAULT_PRIORITY);
	xQueueReceive(debug_cmd_rx_q, (void*)&status, portMAX_DELAY);
	if(status == I2C_SUCCESS) {
		log_str(DEBUG, I2C_LOG, false, "Resp: %x\n", buffer[0]);
	} else {
		log_str(DEBUG, I2C_LOG, false, "Err: %d\n", status);
	}
}

/**
 * @brief Write a value from an i2c device
 *            Arg 0 - 'XX' address of device to write to as two character hex
 *            Arg 1 - 'XX' address of register to write to as two character hex
 *            Arg 2 - 'XX' value to write as two character hex
 *
 */
void cmd_i2c_write(uint32_t arg_len, void* arg) {
    uint8_t dev_addr, reg_addr;
    uint8_t buffer[1] = {0};
    i2c_err_t status = I2C_QUEUE_FULL;
    i2c_command_t command = {0, DEBUG_CMD_RECEIVE_QUEUE_ID, 0, WRITE_DATA, 1, buffer};
    cmd_argument_t* args = (cmd_argument_t*)arg;
    if (num_args(arg_len) != 3) {
        log_str(DEBUG, I2C_LOG, false, "Usage: i2c_write ADDR REG VAL\n");
        return;
    }
    dev_addr = strtoul((const char*)args[0], NULL, 16);
    reg_addr = strtoul((const char*)args[1], NULL, 16);
    buffer[0] = strtoul((const char*)args[2], NULL, 16);
    command.destination = dev_addr;
    command.cmd = reg_addr;
	log_str(DEBUG, I2C_LOG, false, "i2c_write %X %X %X\n", dev_addr, reg_addr, buffer[0]);
	send_command_to_i2c_worker(&command, I2C_WORKER_DEFAULT_PRIORITY);
	xQueueReceive(debug_cmd_rx_q, (void*)&status, portMAX_DELAY);
	if(status == I2C_SUCCESS) {
		log_str(DEBUG, I2C_LOG, false, "Success\n");
	} else {
		log_str(DEBUG, I2C_LOG, false, "Err: %d\n", status);
	}
}

// ---------------------------- Periodic Action -----------------------

void cmd_get_period(uint32_t arg_len, void* arg) {
    if (num_args(arg_len) == 1) {
        uint8_t action_id      = cseq_to_num((char*)arg);
        uint32_t period        = 0;
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
    if (num_args(arg_len) == 2) {
        uint8_t action_id      = cseq_to_num((char*)arg);
        uint32_t period        = cseq_to_num((char*)(arg) + ARGUMENT_SIZE);
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
    if (num_args(arg_len) == 1) {
        uint32_t table_index = cseq_to_num((char*)arg);
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
    if (num_args(arg_len) == 1) {
        uint32_t table_index = cseq_to_num((char*)arg);
        setting_err_t err    = provision_new_settings_table(table_index);
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
    if (num_args(arg_len) == 1) {
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
    if (num_args(arg_len) == 2) {
        uint32_t val      = cseq_to_num((char*)(arg) + ARGUMENT_SIZE);
        setting_err_t err = set_uint32_from_command((char*)(arg), val);

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
