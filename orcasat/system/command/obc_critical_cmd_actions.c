/** @file obc_critical_cmd_actions.c
 *  @brief OBC Command function implementations.
 *  @author ORCASat C&DH team
 *  @date 2020-07-06
 */

#include "FreeRTOS.h"
#include "obc_critical_cmd_actions.h"
#include "obc_heartbeat.h"
#include "rtos_mpu_wrappers.h"

/**
 * @brief Resets the OBC.
 */
void cmd_nrst(uint32_t arg_len, void* arg) {
    tms570_nrst();
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
