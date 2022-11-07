/** @file obc_normal_cmd_actions.h
 *  @brief OBC Command function implementations.
 *
 * Critical commands are executed immediately without using the scheduler,
 * and inside an RTOS critical section.
 *
 * They are intended for emergency functionality that should be executed
 * in the case of problems with other pieces of the system that may
 * prevent regular scheduled commands from executing.
 *
 * Critical commands should use as little infrastructure as possible.
 * This means no logging, RTC, filesystem, etc.
 */

#ifndef OBC_CRITICAL_CMD_ACTIONS_H_
#define OBC_CRITICAL_CMD_ACTIONS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include "sys_common.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// Commands which have the potential to be critical (can also be scheduled)
void cmd_nrst(uint32_t arg_len, void* arg);
#ifndef PLATFORM_ALEA_V1 // TODO use COMMS_WD signal for heartbeat (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/60)
void cmd_hb_suspend(uint32_t arg_len, void* arg);
void cmd_hb_start(uint32_t arg_len, void* arg);
#endif

#endif /* OBC_CRITICAL_CMD_ACTIONS_H_ */
