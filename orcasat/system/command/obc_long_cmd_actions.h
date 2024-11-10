/** @file obc_long_cmd_actions.h
 *  @brief OBC long command function implementations.
 *  @author ORCASat C&DH team
 *  @date 2020-07-06
 */

#ifndef OBC_LONG_CMD_ACTIONS_H_
#define OBC_LONG_CMD_ACTIONS_H_

#include "sys_common.h"

// RTOS commands
void cmd_rtos_tasks(uint32_t arg_len, void* arg);
void cmd_rtos_info(uint32_t arg_len, void* arg);

// Filesystem commands
void cmd_fs_ls(uint32_t arg_len, void* arg);

// CFDP
void cmd_downlink_put_request(uint32_t arg_len, void* arg);

// RTC
void cmd_rtc_stats(uint32_t arg_len, void* arg);
void cmd_rtc_info(uint32_t arg_len, void* arg);

// Scheduler
void cmd_print_sched(uint32_t arg_len, void* arg);

// GPS
void cmd_gps_time(uint32_t arg_len, void* arg);
void cmd_gps_xyz(uint32_t arg_len, void* arg);
void cmd_gps_cmd(uint32_t arg_len, void* arg);

// ADCS commands
void cmd_adcs_tlm(uint32_t arg_len, void* arg);
void cmd_adcs_tlms(uint32_t arg_len, void* arg);
void cmd_adcs_ack(uint32_t arg_len, void* arg);
void cmd_adcs_comms(uint32_t arg_len, void* arg);

// Testing
void cmd_test_temperature(uint32_t arg_len, void* arg);
void cmd_cfdp_pdu_demo(uint32_t arg_len, void* arg);

#endif /* OBC_LONG_CMD_ACTIONS_H_ */
