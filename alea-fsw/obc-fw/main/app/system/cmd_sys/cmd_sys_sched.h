/**
 * @file cmd_sys_sched.h
 * @brief Command System for scheduled commands
 */

#ifndef CMD_SYS_SCHED_H_
#define CMD_SYS_SCHED_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Command System
#include "cmd_sys.h"

// RTC Scheduler
#include "rtc_scheduler.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Since scheduled commands have to be stored in the RTC scheduler in a
 * memory buffer, there is an upper limit to how large their data payload (args)
 * can be, which is defined here.
 */
#define CMD_SYS_SCHED_MAX_DATA_SIZE RTC_SCHEDULER_MAX_DATA_SIZE

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void cmd_sys_sched_pre_init(void);
void cmd_sys_sched_post_init(void);

cmd_sys_err_t cmd_sys_sched_push_cmd(const cmd_sys_msg_header_t *header, uint8_t *header_and_data, uint32_t header_and_data_len);

#endif // CMD_SYS_SCHED_H_
