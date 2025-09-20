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
#include "obc_serial_defs.h"

// Utils
#include "buffered_io.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef bool (*obc_serial_rx_handler_t)(const uint8_t *data, uint8_t data_len, uint32_t timeout_ticks);

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void obc_serial_rx_create_infra(void);
void obc_serial_rx_register_handler(obc_serial_datagram_type_t datagram_type, obc_serial_rx_handler_t handler);

void obc_serial_rx_init_irq(void);
void obc_serial_rx_create_task(void);

void obc_serial_rx_isr(BaseType_t *pxHigherPriorityTaskWoken);

#endif // OBC_SERIAL_RX_H_
