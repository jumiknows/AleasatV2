/**
 * @file comms_obc_serial.h
 * @brief COMMS device implementation using the OBC serial port
 */

#ifndef COMMS_OBC_SERIAL_H_
#define COMMS_OBC_SERIAL_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// COMMS
#include "comms_device.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void comms_obc_serial_init(void);
comms_dev_handle_t comms_obc_serial_get_handle(void);
void comms_obc_serial_invoke_cb(void);

#endif // COMMS_OBC_SERIAL_H_
