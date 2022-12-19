/**
 * @file obc_serial_tx.h
 * @brief OBC serial tx driver
 */

#ifndef OBC_SERIAL_TX_H_
#define OBC_SERIAL_TX_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC Serial
#include "obc_serial.h"

// Utils
#include "io_stream.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

extern const io_ostream_t obc_serial_comms_out;
extern const io_ostream_t obc_serial_logs_out;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void obc_serial_tx_create_infra(void);
void obc_serial_tx_init_irq(void);
void obc_serial_tx_start_task(void);

#endif // OBC_SERIAL_TX_H_
