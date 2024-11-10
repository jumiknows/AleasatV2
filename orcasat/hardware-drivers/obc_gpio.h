/**
 * @file obc_gpio.h
 * @brief OBC GPIO abstraction to handle GPIO expander and internal TMS570 GPIOs with the same API.
 * @author Richard A
 * @date August 29, 2019
 *
 *      OBC GPIO ABSTRACTION
 *      		- Provides a single API for use of GPIO pins from the expander or the TMS570.
 *
 *      	PORT SETTING:
 *      		In hardwaredefs, use either the halcogen-generated GIO port pointers or the expander
 * ports: TMS570: gioPORTA, gioPORTB Expander: EXPANDER_PORT0, EXPANDER_PORT1
 *
 *      PIN SETTING:
 *      		- Pins are always numbers 0-7
 */

#ifndef ORCASAT_OBC_GPIO_H_
#define ORCASAT_OBC_GPIO_H_

#include "sys_common.h"
#include "obc_hardwaredefs.h"
#include "obc_error.h"
#include "reg_gio.h"
#include "reg_het.h"

/**
 * @brief Fake GPIO port pointers that will map calls to use the GPIO expander.
 *
 * HWdefs GPIO Expander Defines
 * 	- Dummy gioPORT pointers that when seen, will trigger usage of the GPIO expander API.
 * 	- Defined as odd numbers which will never conflict with a real gpio base pointer.
 * 		- Memory-mapped register addresses are always word-aligned and therefore even.
 */
#define EXPANDER_PORT_0 (gioPORT_t*)0x77777777
#define EXPANDER_PORT_1 (gioPORT_t*)0x99999999

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

/* Startup Stages */
void gpio_create_infra(void);
void gpio_init_hw(void);
void gpio_start_task(void);
void gpio_init_irq(void);
void gpio_expander_init(void);
void gpio_expander_reset(void);

gpio_err_t obc_gpio_write(gioPORT_t* port, uint32_t pin, uint32_t value);
gpio_err_t obc_gpio_read(gioPORT_t* port, uint32_t pin, uint32_t* value);

/* Interrupt Handling */
void service_gpio_irq(gpio_irq_t irq_info);

#endif /* ORCASAT_OBC_GPIO_H_ */
