/**
 * @file gps_serial_rx.h
 * @brief GPS serial rx driver
 */

#ifndef GPS_SERIAL_RX_H_
#define GPS_SERIAL_RX_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Utils
#include "buffered_io.h"

// FreeRTOS
#include "rtos.h"

// HAL
#include "sci.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/



/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void gps_serial_rx_init_irq(void);
void gps_serial_rx_start_task(void);
void gps_serial_rx_create_infra(void);

void gps_serial_rx_isr(BaseType_t *pxHigherPriorityTaskWoken);

#endif // GPS_SERIAL_RX_H_
