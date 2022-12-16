/** @file obc_prompt_cmd_actions.c
 *  @brief OBC prompt command function implementations.
 */

#ifndef OBC_PROMPT_CMD_ACTIONS_H_
#define OBC_PROMPT_CMD_ACTIONS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include <stdint.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// System commands
void cmd_ack(uint32_t arg_len, void* arg);
void cmd_ping(uint32_t arg_len, void* arg);
void cmd_sys_time(uint32_t arg_len, void* arg);
void cmd_cpu_usage(uint32_t arg_len, void* arg);
void cmd_get_heap(uint32_t arg_len, void* arg);
void cmd_get_minheap(uint32_t arg_len, void* arg);
void cmd_get_board_type(uint32_t arg_len, void* arg);
void cmd_watchdog_reset(uint32_t arg_len, void* arg);
void cmd_get_sequence_num(uint32_t arg_len, void* arg);
void cmd_last_response(uint32_t arg_len, void* arg);
void cmd_temp_reset(uint32_t arg_len, void* arg);
void cmd_expander_reset(uint32_t arg_len, void* arg);
void cmd_i2c_reset(uint32_t arg_len, void* arg);

// Time-related commands
void cmd_get_time(uint32_t arg_len, void* arg);
void cmd_get_epoch(uint32_t arg_len, void* arg);
void cmd_get_epoch_and_time(uint32_t arg_len, void* arg);
void cmd_get_backup_epoch(uint32_t arg_len, void* arg);

// RTC
void cmd_set_time(uint32_t arg_len, void* arg);
void cmd_set_rtc(uint32_t arg_len, void* arg);
void cmd_rtc_ts(uint32_t arg_len, void* arg);

// Testing commands
void cmd_exec_fs_test(uint32_t arg_len, void* arg);
void cmd_count(uint32_t arg_len, void* arg);
void cmd_get_count(uint32_t arg_len, void* arg);
void cmd_run_hang_task(uint32_t arg_len, void* arg);
void cmd_echo_to_log(uint32_t arg_len, void* arg);

// Power configuration commands
void cmd_LP_on(uint32_t arg_len, void* arg);
void cmd_LP_off(uint32_t arg_len, void* arg);

// RTOS commands
void cmd_rtos_suspend(uint32_t arg_len, void* arg);
void cmd_rtos_resume(uint32_t arg_len, void* arg);
void cmd_rtos_period(uint32_t arg_len, void* arg);
void cmd_rtos_state(uint32_t arg_len, void* arg);

// Filesystem commands
void cmd_fs_mkdir(uint32_t arg_len, void* arg);
void cmd_fs_rm(uint32_t arg_len, void* arg);
void cmd_fs_create(uint32_t arg_len, void* arg);
void cmd_fs_file_size(uint32_t arg_len, void* arg);
void cmd_fs_size(uint32_t arg_len, void* arg);

// Misc. OBC telemetry
void cmd_get_temperature(uint32_t arg_len, void* arg);

// Periodic Actions
void cmd_get_period(uint32_t arg_len, void* arg);
void cmd_set_period(uint32_t arg_len, void* arg);

// Settings commands
void cmd_get_setting(uint32_t arg_len, void* arg);
void cmd_set_setting(uint32_t arg_len, void* arg);
void cmd_provision_nvct(uint32_t arg_len, void* arg);
void cmd_erase_nvct(uint32_t arg_len, void* arg);

#endif /* OBC_PROMPT_CMD_ACTIONS_H_ */
