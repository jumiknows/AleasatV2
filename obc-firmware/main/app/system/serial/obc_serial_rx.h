/**
 * @file obc_serial_rx.h
 * @brief OBC serial rx driver
 */

#ifndef OBC_SERIAL_RX_H_
#define OBC_SERIAL_RX_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC Serial
#include "obc_serial.h"

// Utils
#include "buffered_io.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

extern const buffered_block_istream_t obc_serial_comms_in;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void obc_serial_rx_create_infra(void);
void obc_serial_rx_init_irq(void);
void obc_serial_rx_start_task(void);

void obc_serial_rx_isr(BaseType_t *pxHigherPriorityTaskWoken);

#endif // OBC_SERIAL_RX_H_
