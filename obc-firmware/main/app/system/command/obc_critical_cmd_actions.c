/** @file obc_critical_cmd_actions.c
 *  @brief OBC Command function implementations.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_critical_cmd_actions.h"
#include "obc_heartbeat.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/**
 * @brief See application_defined_privileged_functions.h
 */
void vPrivilegedCPUReset(void);

/**
 * @brief Resets the OBC.
 */
void cmd_nrst(uint32_t arg_len, void* arg) {
    vPrivilegedCPUReset();
}

#ifndef PLATFORM_ALEA_V1 // TODO use COMMS_WD signal for heartbeat (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/60)
/**
 * @brief Suspends the OBC heartbeat, which will trigger a satellite-wide
 * power cycle.
 */
void cmd_hb_suspend(uint32_t arg_len, void* arg) {
    suspend_heartbeat();
}

/**
 * @brief Starts the OBC heartbeat signal. This signal defaults to on at boot,
 * so this command is only necessary during testing to enable the signal
 * without needing to do a reset after it has been previously suspended.
 */
void cmd_hb_start(uint32_t arg_len, void* arg) {
    start_heartbeat();
}
#endif
