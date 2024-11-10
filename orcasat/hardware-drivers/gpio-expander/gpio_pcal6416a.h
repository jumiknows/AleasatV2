/**
 * @file gpio_pcal6416a.h
 *
 * @date August 29, 2019
 * @author Richard A
 *
 * @brief Driver for the PCAL616A GPIO expander.
 *
 *      Device: NXP PCAL6416A
 *      Datasheet: https://www.nxp.com/docs/en/data-sheet/PCAL6416A.pdf
 *
 *      USAGE:
 *      		- Use EXPANDER_PORT0, EXPANDER_PORT1 as the port handle in obc_gpio functions
 *      		- In obc_gpio.c/obc_gpio_init(), configure the IO pins using @ref
 *configure_output(),
 *@ref configure_input(), @ref configure_input_interrupt() calls.
 *      		- Read or write the pin values using obc_gpio.c
 *
 *		INTERRUPT Configuration:
 *			- Increase NUM_INTERRUPT_PINS defined below.
 *			- NUM_INTERRUPT_PINS must match the number of configured input interrupt pins, or bad
 *things will happen.
 *			- Configure the pin in obc_gpio.c/obc_gpio_init() with @ref configure_input_interrupt().
 *				- This function accepts a callback that will be provided with the pin's value
 *(uint32_t) when an interrupt is raised from the GPIO expander.
 *
 *		NOTES:
 *			- Outputs are all configured to push-pull as opposed to open drain.
 *			- Output drive strength is configured to the maximum of about 10 mA (datasheet is
 *unclear about the actual maximum value).
 *
 */

#ifndef GPIO_PCAL6416A_H_
#define GPIO_PCAL6416A_H_

#include <stdbool.h>
#include "obc_gpio.h"
#include "sys_common.h"
#include "reg_gio.h"
#include "obc_featuredefs.h"

#if GPIO_EXPANDER_EN

/**
 * @brief number of enabled interrupt pins, these must be configured with callbacks using @ref
 * configure_input().
 */
#define NUM_INTERRUPT_PINS 1

/**
 * @brief GPIO register structure, representing an 8-bit reg value and its address in the device.
 *
 * GPIO register structure, representing an 8-bit reg value and its address in the device. Allows
 * for register maps (I.E. the entire set of RW registers in the device)
 */
typedef struct gpio_reg {
    uint8_t val;
    uint8_t addr;
} gpio_reg_t;

/**
 * @brief All read-write (RW) registers in the GPIO expander.
 *
 * This structure allows the expander configuration to be mirrored in firmware, and will be
 * kept up-to-date with the desired configuration. This allows the regs to be scanned and compared
 * to the desired values, and updated if mismatches occur.
 *
 * Arrays of @ref gpio_reg_t. Array index corresponds to the port, 0 or 1.
 */
typedef struct gpio_expand_rw_regs {
    gpio_reg_t output_val[2];
    gpio_reg_t polarity_invert[2]; // always make 0 (normal polarity)
    gpio_reg_t input[2];           // 1 = input (default), 0 = output
    gpio_reg_t output_drive_a[2];
    gpio_reg_t output_drive_b[2];
    gpio_reg_t input_latch[2];    // 1 = inputs are latched, 0 = inputs not latched
    gpio_reg_t pull_enable[2];    // 1 = pull up/down is enabled, 0 = disabled
    gpio_reg_t pull_select[2];    // 1 = pullup, 0 = pulldown
    gpio_reg_t interrupt_mask[2]; // 1 = masked, 0 = enabled
    gpio_reg_t output_port_cfg;   // 1 = open drain, 0 = push-pull
} gpio_expand_rw_t;

/**
 * @brief All read-only (RO) registers in the GPIO expander
 *
 * Read-only register values are placed in this structure for access when they are read from the
 * device. Since we do not know what these values should be, they are not part of the register
 * scanning.
 */
typedef struct gpio_expand_ro_regs {
    gpio_reg_t input_value[2];
    gpio_reg_t interrupt_status[2];
} gpio_expand_ro_t;

/**
 * @brief Pin pull state setting enum.
 *
 * Allows only valid pull cases to be configured.
 *
 *  - bit 0 = enable
 * 	- bit 1 = up/down (1 = up, 0 = down)
 */
typedef enum pull_cfg { // bit 1	   |  bit 0      |  Combined (bin)
    NONE     = 0,       // pull = 0, enable = 0		00
    PULLDOWN = 1,       // pull = 0, enable = 1		01
    DONT_USE = 2,       // pull = 1, enable = 0      10
    PULLUP   = 3        // pull = 1, enable = 1      11
} pull_cfg_t;

/* Control Functions*/
gpio_err_t init_gpio_expander(void);   // Initialize expander with reset values for all registers
void reset_gpio_expander(void);        // Reset I2C state machine of the expander
gpio_err_t verify_gpio_expander(void); // Prints any mismatches between actual and desired configuration

/* Pin Setup Functions */
gpio_err_t configure_output(gioPORT_t* port, uint8_t pin, bool default_val,
                            pull_cfg_t pull_sel); // Set port, pin to an output
gpio_err_t configure_input(gioPORT_t* port, uint8_t pin,
                           pull_cfg_t pull_sel); // Set port, pin to an input
gpio_err_t configure_input_interrupt(gioPORT_t* port, uint8_t pin, bool latch, pull_cfg_t pull_sel,
                                     void (*callback)(uint32_t input_val)); // Set port, pin to an input with interrupt callback

/* GPIO Functionality */
gpio_err_t set_output(gioPORT_t* port, uint8_t pin, bool val);     // Set the output value 1/0
gpio_err_t get_input(gioPORT_t* port, uint8_t pin, uint32_t* val); // Read the input pin
void check_expander_interrupts(void);                              // Check interrupt pin status

/* GPIO Expander Interrupt Default Callback */
void default_expander_callback(uint32_t input_val); // Example callback

#endif /* GPIO_EXPANDER_EN true */
#endif /* GPIO_PCAL6416A_H_ */
