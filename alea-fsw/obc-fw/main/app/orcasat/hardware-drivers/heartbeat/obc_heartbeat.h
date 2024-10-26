/**
 * @file obc_heartbeat.h
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
 * the entire satellite.
 */

#ifndef OBC_HEARTBEAT_H_
#define OBC_HEARTBEAT_H_

void start_heartbeat(void);
void suspend_heartbeat(void);

#endif /* OBC_HEARTBEAT_H_ */
