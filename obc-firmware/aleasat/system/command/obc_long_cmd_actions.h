/** @file obc_long_cmd_actions.h
 *  @brief OBC long command function implementations.
 */

#ifndef OBC_LONG_CMD_ACTIONS_H_
#define OBC_LONG_CMD_ACTIONS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include <stdint.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

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

// Testing
void cmd_test_temperature(uint32_t arg_len, void* arg);
void cmd_cfdp_pdu_demo(uint32_t arg_len, void* arg);

#endif /* OBC_LONG_CMD_ACTIONS_H_ */
