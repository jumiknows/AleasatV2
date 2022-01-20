/** @file obc_cmds_two_stage.h
 *  @brief Two-stage command header
 *  @author Lana
 *  @date 23-January-2020
 */

#ifndef OBC_CMDS_TWO_STAGE_H_
#define OBC_CMDS_TWO_STAGE_H_

#include "obc_rtc.h"

void cmd_two_stage_arm(uint32_t arg_len, void* arg);
void cmd_two_stage_fire(uint32_t arg_len, void* arg);

#endif /* OBC_CMDS_TWO_STAGE_H_ */
