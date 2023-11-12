/**
 * @file cmd_impl_rtos.c
 * @brief Implementation of commands related to the RTOS
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// OBC
#include "obc_rtos.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

cmd_sys_resp_code_t cmd_impl_RTOS_TASKS(const cmd_sys_cmd_t *cmd) {
    return CMD_SYS_RESP_CODE_NOT_IMPL;
}

cmd_sys_resp_code_t cmd_impl_RTOS_INFO(const cmd_sys_cmd_t *cmd) {
    return CMD_SYS_RESP_CODE_NOT_IMPL;
}

cmd_sys_resp_code_t cmd_impl_RTOS_STATE(const cmd_sys_cmd_t *cmd) {
    // TODO: requires work from ALEA-828
    return CMD_SYS_RESP_CODE_NOT_IMPL;
}
