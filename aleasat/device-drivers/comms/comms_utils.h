/**
 * @file comms_utils.h
 * @brief Comms related utility functions
 */

#ifndef COMMS_UTILS_H_
#define COMMS_UTILS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_defs.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

comms_err_t comms_check_cmd_struct(comms_command_t* cmd_struct);
comms_err_t comms_check_buffer(uint16_t* buf);

comms_err_t comms_check_cmd_params(
    comms_hwid_t hwid,
    uint8_t system,
    uint16_t seqnum,
    uint8_t cmd_num,
    uint8_t* cmd_data,
    uint8_t cmd_data_len
);

comms_err_t comms_buffer_to_cmd_struct(uint16_t* buf, comms_command_t* cmd_struct);
comms_err_t comms_cmd_struct_to_buffer(comms_command_t* cmd_struct, uint16_t* buf);

comms_err_t comms_build_cmd_struct(
    comms_hwid_t hwid,
    uint8_t system,
    uint16_t seqnum,
    uint8_t cmd_num,
    uint8_t* cmd_data,
    uint8_t cmd_data_len,
    comms_command_t* cmd_struct
);

comms_err_t comms_build_buffer(
    comms_hwid_t hwid,
    uint8_t system,
    uint16_t seqnum,
    uint8_t cmd_num,
    uint8_t* cmd_data,
    uint8_t cmd_data_len,
    uint16_t* buf
);

#endif /* COMMS_UTILS_H_ */
