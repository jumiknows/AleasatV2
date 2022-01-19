/**
 * @file comms_cc1110.h
 * @brief High-level OBC to Comms driver
 *
 * @warning This API is not re-entrant
 *
 * @see https://gitlab.com/alea-2020/communications/comms-firmware-openlst-1
 */

#ifndef COMMS_CC1110_H_
#define COMMS_CC1110_H_

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

comms_err_t comms_send_cmd(
    hwid_t dest_hwid,
    uint8_t cmd_num,
    uint8_t* cmd_data,
    uint8_t cmd_data_len,
    uint32_t timeout_ms
);

comms_err_t comms_send_recv_cmd(
    hwid_t dest_hwid,
    uint8_t cmd_num,
    uint8_t* cmd_data,
    uint8_t cmd_data_len,
    comms_command_t* recv_cmd,
    uint32_t timeout_ms
);


#endif /* COMMS_CC1110_H_ */
