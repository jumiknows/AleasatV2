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
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Function pointer for callback after command execution
 */
typedef void (*cmd_sys_exec_callback_t)(cmd_sys_err_t status, void *arg);

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void cmd_sys_exec_pre_init(void);

cmd_sys_err_t cmd_sys_exec_enqueue(cmd_sys_cmd_t *cmd, cmd_sys_exec_callback_t callback, void *arg, uint32_t timeout_ticks);

#endif // CMD_SYS_EXEC_H_
