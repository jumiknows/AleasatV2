/**
 * @file tms_can.h
 * @brief Allows reading and writing from CAN ports as GIO
 */

#ifndef TMS_CAN_H_
#define TMS_CAN_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// HAL
#include "reg_can.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    CAN_PIN_RX = 0,
    CAN_PIN_TX = 1,
} can_pin_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void tms_can_init(void);
void tms_can_gpio_write(canBASE_t* port, can_pin_t pin, uint32_t value);
uint32_t tms_can_gpio_read(canBASE_t* port, can_pin_t pin);

#endif // TMS_CAN_H_
