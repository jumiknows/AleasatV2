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
#include <stdbool.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

comms_err_t comms_check_cmd_struct(comms_command_t* cmd_struct);
comms_err_t comms_check_buffer(uint16_t* buf);

comms_err_t comms_buffer_to_cmd_struct(uint16_t* buf, comms_command_t* cmd_struct);
comms_err_t comms_cmd_struct_to_buffer(comms_command_t* cmd_struct, uint16_t* buf);
#endif /* COMMS_UTILS_H_ */
