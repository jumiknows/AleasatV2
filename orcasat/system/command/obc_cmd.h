/** @file obc_cmd.h
 *  @brief OBC Command System
 *  @author Alireza Alidousti
 *  @date 2020-02-15
 */

#ifndef OBC_CMD_H_
#define OBC_CMD_H_

#include "FreeRTOS.h"
#include "obc_rtc.h"
#include "scheduler.h"
#include "obc_error.h"

// definitions
#define ARGUMENT_SIZE     20 // bytes
#define NUM_ARGUMENTS     7
#define MAX_ARGUMENT_SIZE (NUM_ARGUMENTS * ARGUMENT_SIZE)

typedef char cmd_argument_t[ARGUMENT_SIZE];

typedef struct {
    uint8_t sequence_num;
    scheduler_action_t action;
    real_time_t start_at;
    uint32_t interval;
    cmd_argument_t arguments[NUM_ARGUMENTS];
    uint8_t num_arguments;
} cmd_t;

uint8_t next_seq_num(void);
uint32_t num_args(uint32_t arg_len);
obc_cmd_err_t obc_cmd_parse_and_invoke(char* buff, scheduler_submission_source_t source);
obc_cmd_err_t cmd_opcode_from_action(scheduler_action_t action, const char** opcode);

/**
 * @brief For testing we require control over the sequence number because
 * Ceedling does not isolate individual tests from each other. It needs
 * to be reset after each test or else it will carry over its value at
 * the end of the current test into subsequent tests.
 */
#ifdef CEEDLING_TEST
void ceedling_set_seq_num(uint8_t num);
#endif /* CEEDLING_TEST */
#endif /* OBC_CMD_H_ */
