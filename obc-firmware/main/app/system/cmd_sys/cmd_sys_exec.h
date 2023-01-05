/**
 * @file cmd_sys_exec.h
 * @brief Command system executor
 */

#ifndef CMD_SYS_EXEC_H_
#define CMD_SYS_EXEC_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Command System
#include "cmd_sys.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void cmd_sys_exec_create_infra(void);
void cmd_sys_exec_start_task(void);

cmd_sys_err_t cmd_sys_exec_enqueue(const cmd_sys_cmd_t *cmd, cmd_sys_callback_t callback, uint32_t timeout_ticks);

#endif // CMD_SYS_EXEC_H_
