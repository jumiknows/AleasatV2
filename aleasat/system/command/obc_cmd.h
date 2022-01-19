/** @file obc_cmd.h
 *  @brief OBC Command System
 */

#ifndef OBC_CMD_H_
#define OBC_CMD_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_rtc.h"
#include "scheduler.h"
#include "obc_error.h"


/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// BYTES
#define MAX_ARGUMENT_SIZE 200
#define MAX_HEADER_SIZE 50
#define MAX_CMD_SIZE (MAX_HEADER_SIZE + MAX_ARGUMENT_SIZE)

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

//typedef char cmd_argument_t[ARGUMENT_SIZE];

typedef struct {
    uint8_t sequence_num;
    scheduler_action_t action;
    real_time_t start_at;
    uint32_t interval;
    char* arguments;
    uint8_t num_of_arguments;
} cmd_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

uint8_t obc_cmd_next_seq_num(void);
obc_cmd_err_t obc_cmd_parse_and_invoke(char* buff, scheduler_submission_source_t source);
obc_cmd_err_t obc_cmd_opcode_from_action(scheduler_action_t action, const char** opcode);
uint8_t obc_cmd_read_str_arguments(char* data, uint8_t num, char** args);

#endif /* OBC_CMD_H_ */
