/**
 * @file gpio_pcal6416a.h
 * @brief Driver for the PCAL616A GPIO expander.
 *
 *      Device: NXP PCAL6416A
 *      Datasheet: https://www.nxp.com/docs/en/data-sheet/PCAL6416A.pdf
 *
 *      USAGE:
 *          - Use EXP_PORT(GPIO_EXP_PORT_x) as the port handle in obc_gpio functions
 *          - In obc_gpio.c/obc_gpio_init(), configure the IO pins using @ref pcal6416a_configure_output(),
 *            @ref pcal6416a_configure_input(), @ref pcal6416a_configure_interrupt() calls.
 *          - Read or write the pin values using obc_gpio.c
 *
 *      INTERRUPT Configuration:
 *          - Increase NUM_INTERRUPT_PINS defined below.
 *          - NUM_INTERRUPT_PINS must match the number of configured input interrupt pins, or bad
 *            things will happen.
 *          - Configure the pin in obc_gpio.c/obc_gpio_init() with @ref pcal6416a_configure_interrupt().
 *              - This function accepts a callback that will be provided with the pin's value
 *                (uint32_t) when an interrupt is raised from the GPIO expander.
 *
 *      NOTES:
 *          - Outputs are all configured to push-pull as opposed to open drain.
 *          - Output drive strength is configured to the maximum of about 10 mA (datasheet is
 *            unclear about the actual maximum value).
 */

#ifndef GPIO_PCAL6416A_H_
#define GPIO_PCAL6416A_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_gpio_err.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Enumeration of GPIO expander ports
 */
typedef enum {
    GPIO_EXP_PORT_0 = 0,
    GPIO_EXP_PORT_1 = 1,
} pcal6416a_port_t;

/**
 * @brief Callback function to be invoked when an interrupt fires
 */
typedef void (*pcal6416a_irq_callback_t)(uint32_t input_val);

/**
 * @brief Pin pull state setting enum.
 *
 * Allows only valid pull cases to be configured.
 *
 *  - bit 0 = enable
 *  - bit 1 = up/down (1 = up, 0 = down)
 */
typedef enum {
    NONE     = 0b00, // pull = 0 | enable = 0
    PULLDOWN = 0b01, // pull = 0 | enable = 1
    PULLUP   = 0b11  // pull = 1 | enable = 1
} pcal6416a_pull_cfg_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// Control Functions
gpio_err_t pcal6416a_init(void);
void pcal6416a_reset(void);
gpio_err_t pcal6416a_validate_regs(void);

// Pin setup
gpio_err_t pcal6416a_configure_output(pcal6416a_port_t port, uint8_t pin, bool default_val, pcal6416a_pull_cfg_t pull_sel);
gpio_err_t pcal6416a_configure_input(pcal6416a_port_t port, uint8_t pin, pcal6416a_pull_cfg_t pull_sel);
gpio_err_t pcal6416a_configure_interrupt(pcal6416a_port_t port, uint8_t pin, bool latch, pcal6416a_pull_cfg_t pull_sel,
        pcal6416a_irq_callback_t callback);

// Pin read/write
gpio_err_t pcal6416a_gpio_write(pcal6416a_port_t port, uint8_t pin, bool val);
gpio_err_t pcal6416a_gpio_read(pcal6416a_port_t port, uint8_t pin, uint32_t *val);
void pcal6416a_handle_interrupts(void);

#endif // GPIO_PCAL6416A_H_
