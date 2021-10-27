/**
 * @file obc_gpio.h
 * @brief OBC GPIO abstraction to handle internal TMS570 GPIO pins or the GPIO expander.
 *
 * OBC GPIO ABSTRACTION
 *  - Provides a single API for use of internal TMS570 GPIO pins or the GPIO expander.
 * 
 * PORT SETTING:
 *  - In hardwaredefs, use the xxx_PORT macros to specify different GPIO ports
 */

#ifndef ORCASAT_OBC_GPIO_H_
#define ORCASAT_OBC_GPIO_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_error.h"

// HAL
#include "reg_gio.h"
#include "reg_can.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Macro to specify a GIO port (built into the TMS570) for the obc_gpio_read/obc_gpio_write functions
 * @param _port Pointer to a gioPORT_t (ex. gioPORTA or gioPORTB)
 */
#define GIO_PORT(_port) (gpio_port_t){.type = GPIO_PORT_GIO, .reg = {.gio = (_port)}}

/**
 * @brief Macro to specify a CAN port for the obc_gpio_read/obc_gpio_write functions
 * @param _port Pointer to a canBASE_t (canREG1, canREG2 or canREG3)
 */
#define CAN_PORT(_port) (gpio_port_t){.type = GPIO_PORT_CAN, .reg = {.can = (_port)}}

/**
 * @brief Macro to specify a GPIO Expander port for the obc_gpio_read/obc_gpio_write functions
 * @param _port One of the gpio_expand_port_t enum values (GPIO_EXP_PORT_0 or GPIO_EXP_PORT_1)
 */
#define EXP_PORT(_port) (gpio_port_t){.type = GPIO_PORT_EXP, .reg = {.exp = (_port)}}

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Enumeration of different ports that can act as GPIO ports
 */
typedef enum {
    GPIO_PORT_GIO,
    GPIO_PORT_CAN,
    GPIO_PORT_EXP
} gpio_port_type_t;

/**
 * @brief Enumeration of GPIO expander ports
 */
typedef enum {
    GPIO_EXP_PORT_0 = 0,
    GPIO_EXP_PORT_1 = 1,
} gpio_expand_port_t;

/**
 * @brief Wrapper for GIO notification information.
 *
 * Contains a gioPORT_t pointer, corressponding to the port that raised the interrupt.
 * Pin is the pin on the port that raised the interrupt.
 */
typedef struct gpio_irq {
    gioPORT_t* port;
    uint32_t pin;
} gpio_irq_t;

/**
 * @brief Struct to specify any GPIO port (the type and register information)
 * 
 * In general, don't instantiate this struct directly, use one of the xxx_PORT macros instead.
 */
typedef const struct {
    const gpio_port_type_t type;
    const union {
        gioPORT_t* const gio;
        canBASE_t* const can;
        const gpio_expand_port_t exp;
    } reg;
} gpio_port_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// Startup Stages
void gpio_create_infra(void);
void gpio_init_hw(void);
void gpio_start_task(void);
void gpio_init_irq(void);
void gpio_expander_init(void);
void gpio_expander_reset(void);

// Read / Write
gpio_err_t obc_gpio_write(gpio_port_t port, uint32_t pin, uint32_t value);
gpio_err_t obc_gpio_read(gpio_port_t port, uint32_t pin, uint32_t* value);

// Interrupt Handling
void service_gpio_irq(gpio_irq_t irq_info);

#endif /* ORCASAT_OBC_GPIO_H_ */
