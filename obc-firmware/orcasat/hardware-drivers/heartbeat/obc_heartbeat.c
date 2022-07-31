/**
 * @file obc_heartbeat.c
 * @brief OBC heartbeat feature, which triggers or prevents a satellite-wide power cycle.
 * @date March 24, 2020
 * @author Richard A
 *
 * Design doc: https://docs.google.com/document/d/1g--ja0yEUBZPScYM87hw2_pdPdQKeEMpJbMtlJZgjgA/edit#
 *
 * The OBC heartbeat is a signal provided by the OBC to the EPS to indicate that the OBC
 * is functioning. If the EPS detects that this signal is gone, it will trigger a complete
 * power cycle and re-deployment sequence. For reliability purposes, the OBC generates
 * the heartbeat entirely in hardware, with minimal firmware overhead. A command is provided
 * to suspend heartbeat generation, which gives operators the ability to power cycle
 */

#include "obc_heartbeat.h"
#include "obc_hardwaredefs.h"
#include "het.h"

#ifdef FEATURE_HEARTBEAT

/**
 * @brief Enables the heartbeat by starting PWM generation.
 *
 * This will only be used during testing. If the heartbeat has been
 * suspended but we don't want to reset the OBC for some reason (which
 * would trigger @ref hetInit() at boot to start the heartbeat), this
 * can be used to restart the heartbeat.
 *
 * During flight, the EPS will reset the OBC so quickly that there
 * likely would not be time to process and execute this command, so
 * the heartbeat would be restarted as the OBC boots after the EPS reset.
 */
void start_heartbeat(void) {
    pwmStart(OBC_HEARTBEAT_HET, OBC_HEARTBEAT_PWM);
}

/**
 * @brief Suspends the heartbeat by stopping PWM generation.
 */
void suspend_heartbeat(void) {
    pwmStop(OBC_HEARTBEAT_HET, OBC_HEARTBEAT_PWM);
}

#endif // FEATURE_HEARTBEAT
