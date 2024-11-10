/**
 * @file telem_obc.h
 * @brief OBC internal telemetry collection functions.
 * @author Richard A
 * @date October 31, 2019
 *
 * OBC telemetry collection functions. Once initialized, OBC telemetry collection is handled by
 * several actions that are executed by the scheduler.
 */

#ifndef OBC_TELEMETRY_H_
#define OBC_TELEMETRY_H_

void obc_slow_telem_collect(void);
void obc_fast_telem_collect(void);

#endif /* OBC_TELEMETRY_H_ */
