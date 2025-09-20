/**
 * @file comms_mibspi.h
 * @brief COMMS device implementation using MIBSPI
 */

#ifndef COMMS_MIBSPI_H_
#define COMMS_MIBSPI_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// COMMS
#include "comms_device.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void comms_mibspi_init(void);
comms_dev_handle_t comms_mibspi_get_handle(void);
void comms_mibspi_invoke_irq_cb(void);

#endif // COMMS_MIBSPI_H_
