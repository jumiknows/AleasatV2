/**
 * @file gpio_pcal6416a.c
 * @brief Driver for the PCAL616A GPIO expander.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "gpio_pcal6416a.h"
#include "gpio_pcal6416a_defs.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"
#include "obc_gpio_err.h"
#include "tms_i2c.h"
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// HAL
#include "gio.h"

// Standard Library
#include <stdint.h>
#include "assert.h"

#if FEATURE_GPIO_EXPANDER

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Number of ports on the GPIO expander
 */
#define NUM_PORTS 2

/**
 * @brief Number of pins on each port of the GPIO expander
 */
#define NUM_PINS  8

/**
 * @brief Bit value for output direction
 */
#define DIRECTION_OUTPUT 0

/**
 * @brief Bit value for input direction
 */
#define DIRECTION_INPUT 1

/**
 * @brief Maximum time to wait to acquire the I2C mutex in milliseconds
 */
#define I2C_MUTEX_TIMEOUT_MS 500

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief GPIO register structure, representing an 8-bit reg value and its address in the device.
 *
 * GPIO register structure, representing an 8-bit reg value and its address in the device. Allows
 * for register maps (I.E. the entire set of RW registers in the device)
 */
typedef struct {
    uint8_t val;
    uint8_t addr;
} pcal6416a_reg_t;

/**
 * @brief All read-write (RW) registers in the GPIO expander.
 *
 * This structure allows the expander configuration to be mirrored in firmware, and will be
 * kept up-to-date with the desired configuration. This allows the regs to be scanned and compared
 * to the desired values, and updated if mismatches occur.
 *
 * Arrays of @ref gpio_exp_reg_t. Array index corresponds to the port, 0 or 1.
 */
typedef struct {
    pcal6416a_reg_t output_val[2];
    pcal6416a_reg_t polarity_invert[2]; // always make 0 (normal polarity)
    pcal6416a_reg_t direction[2];       // 1 = input (default), 0 = output
    pcal6416a_reg_t output_drive_a[2];
    pcal6416a_reg_t output_drive_b[2];
    pcal6416a_reg_t input_latch[2];     // 1 = inputs are latched, 0 = inputs not latched
    pcal6416a_reg_t pull_enable[2];     // 1 = pull up/down is enabled, 0 = disabled
    pcal6416a_reg_t pull_select[2];     // 1 = pullup, 0 = pulldown
    pcal6416a_reg_t interrupt_mask[2];  // 1 = masked, 0 = enabled
    pcal6416a_reg_t output_port_cfg;    // 1 = open drain, 0 = push-pull
} pcal6416a_rw_regs_t;

/**
 * @brief All read-only (RO) registers in the GPIO expander
 *
 * Read-only register values are placed in this structure for access when they are read from the
 * device. Since we do not know what these values should be, they are not part of the register
 * scanning.
 */
typedef struct {
    const pcal6416a_reg_t input_value[2];
    const pcal6416a_reg_t interrupt_status[2];
} pcal6416a_ro_regs_t;

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief On-board copy of gpio expander read-write (RW) registers.
 *
 * Before modifying settings on the device, this struct is updated to ensure the OBC always
 * maintains the source of truth in ECC RAM. This allows register settings on the expander to be
 * verified and updated any time.
 *
 * Output drive settings are kept at the default maximum value of about 10 mA, although the
 * datasheet is unclear about the actual value.
 *
 * Register pairs are for ports 0 and 1, and all registers except output_port_cfg are in pairs.
 * 
 * All pins are configured as inputs by default (same as the chip reset configuration)
 */
pcal6416a_rw_regs_t rw_regs = {
    .output_val      = {{.val = 0x00, .addr = PCAL6416A_OUTPUT_PORT_0},      {.val = 0x00, .addr = PCAL6416A_OUTPUT_PORT_1}},      // 1 = high, 0 = low
    .polarity_invert = {{.val = 0x00, .addr = PCAL6416A_POL_INV_PORT_0},     {.val = 0x00, .addr = PCAL6416A_POL_INV_PORT_1}},     // always set 0 (normal polarity)
    .direction       = {{.val = 0xFF, .addr = PCAL6416A_CONFIG_PORT_0},      {.val = 0xFF, .addr = PCAL6416A_CONFIG_PORT_1}},      // 1 = input (default), 0 = output
    .output_drive_a  = {{.val = 0xFF, .addr = PCAL6416A_OUTPUT_DRV_STR_0},   {.val = 0xFF, .addr = PCAL6416A_OUTPUT_DRV_STR_1}},   // Leave at default values
    .output_drive_b  = {{.val = 0xFF, .addr = PCAL6416A_OUTPUT_DRV_STR_A0},  {.val = 0xFF, .addr = PCAL6416A_OUTPUT_DRV_STR_A1}},  // Leave at default values
    .input_latch     = {{.val = 0x00, .addr = PCAL6416A_INPUT_LATCH_REG_0},  {.val = 0x00, .addr = PCAL6416A_INPUT_LATCH_REG_1}},  // 1 = inputs are latched, 0 = inputs not latched
    .pull_enable     = {{.val = 0x00, .addr = PCAL6416A_PULL_UP_DOWN_ENA_0}, {.val = 0x00, .addr = PCAL6416A_PULL_UP_DOWN_ENA_1}}, // 1 = pull up/down is enabled, 0 = disabled
    .pull_select     = {{.val = 0x00, .addr = PCAL6416A_PULL_UP_DOWN_SEL_0}, {.val = 0x00, .addr = PCAL6416A_PULL_UP_DOWN_SEL_1}}, // 1 = pullup, 0 = pulldown
    .interrupt_mask  = {{.val = 0xFF, .addr = PCAL6416A_IRQ_MASK_0},         {.val = 0xFF, .addr = PCAL6416A_IRQ_MASK_1}},         // 1 = masked, 0 = enabled
    .output_port_cfg = {.val = 0x00, .addr = PCAL6416A_OUTPUT_PORT_CONFIG}                                               // 1 = open drain, 0 = push-pull.
                                                                                                               // Both configured to push-pull.
};

/**
 * @brief On-board copy of read-only (RO) registers.
 *
 * RO registers are the input state and the interrupt status. These can't be checked by OBC firmware
 * because their values are dependent on what is hooked up to the GPIO expander.
 */
const pcal6416a_ro_regs_t ro_regs = {
    .input_value      = {{.val = 0x00, .addr = PCAL6416A_INPUT_PORT_0}, {.val = 0x00, .addr = PCAL6416A_INPUT_PORT_1}},
    .interrupt_status = {{.val = 0x00, .addr = PCAL6416A_IRQ_STATUS_0}, {.val = 0x00, .addr = PCAL6416A_IRQ_STATUS_1}}
};

/**
 * @brief Mapping between interrupt port/pin and the callback function to execute when the interrupt
 * fires.
 */
pcal6416a_irq_callback_t irq_callbacks[NUM_PORTS][NUM_PINS] = {
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
};

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static gpio_err_t configure_pull_regs(pcal6416a_port_t port, uint8_t pin, pcal6416a_pull_cfg_t pull_sel);
static gpio_err_t configure_bit(pcal6416a_reg_t* reg, uint8_t pin, bool val);
static gpio_err_t i2c_write_reg(pcal6416a_reg_t* reg);
static gpio_err_t i2c_read_reg(const pcal6416a_reg_t* reg, uint8_t* rx_data);
static bool register_mismatch(pcal6416a_reg_t* predicted, uint8_t actual);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the GPIO expander with the values in @ref expander, which are the default
 * PCAL6416A power-up settings upon OBC boot.
 *
 * This function is useful for ensuring that the expander has all values currently represented in
 * the @ref expander struct, whether it be at startup or after a problem with the expander's
 * configuration is detected by a register scan.
 *
 * @pre obc_i2c_init from @ref orcasat/interfaces/i2c_freertos.h has been called.
 * @warning this function will write whatever is currently in the @ref expander struct to the
 * expander, which may be modified with calls like @ref pcal6416a_configure_output().
 * 
 * @return GPIO_SUCCESS indicating that initialization was successful, or GPIO_I2C_ERR indicating an
 * IO error when communicating with the expander over I2C.
 */
gpio_err_t pcal6416a_init(void) {
    pcal6416a_reg_t* reg = (pcal6416a_reg_t*)&rw_regs;

    // Iterate through each reg in the RW representation and write the configuration to the device
    uint8_t i;
    for (i = 0; i < (sizeof(rw_regs) / sizeof(*reg)); i++) {
        if (i2c_write_reg(reg) != GPIO_SUCCESS) {
            return GPIO_I2C_ERR;
        }
        reg++;
    }

    return GPIO_SUCCESS;
}

/**
 * @brief Hard reset of GPIO expander
 */
void pcal6416a_reset(void) {
    log_str(INFO, LOG_PRINT_GENERAL, false, "Resetting GPIO expander");
    gioSetBit(OBC_EXPAND_RESET_PORT, OBC_EXPAND_RESET_PIN, 0);
    vTaskDelay(1); // Delay the minimum amount of time (only 30 ns is required by the chip)
    gioSetBit(OBC_EXPAND_RESET_PORT, OBC_EXPAND_RESET_PIN, 1);
}

/**
 * @brief Runs through all RW registers in the GPIO expander and compares their values to the
 * internal @ref expander representation. Mismatches are printed.
 *
 * The register values inside the device may change because of single event upsets. By periodically
 * verifying the gpio expander and running @ref pcal6416a_init() if any mismatches are seen, we
 * can ensure that the device is properly configured most of the time.
 *
 * @pre @ref obc_i2c_init from @ref orcasat/interfaces/i2c_freertos.h has been called.
 *
 * @return GPIO_SUCCESS indicating that verification had no IO errors, or GPIO_I2C_ERR indicating an
 * IO error when communicating with the expander over I2C.
 */
gpio_err_t pcal6416a_validate_regs(void) {
    pcal6416a_reg_t* reg         = (pcal6416a_reg_t*)&rw_regs;
    const uint8_t all_regs_count = (sizeof(rw_regs) / sizeof(*reg));
    uint8_t correct_count        = all_regs_count;

    // Iterate through each RW register pair on the device, compare with expected value, and attempt
    // to correct the problem.
    uint8_t i;
    for (i = 0; i < all_regs_count; i++) {
        uint8_t actual_reg_val = 0;
        if (i2c_read_reg(reg, &actual_reg_val) != GPIO_SUCCESS) {
            return GPIO_I2C_ERR;
        }

        if (register_mismatch(reg, actual_reg_val)) {
            correct_count--;

            // Attempt to correct the value
            if (i2c_write_reg(reg) != GPIO_SUCCESS) {
                log_str(ERROR, LOG_GPIO_EXPAND, true, "Failed to correct reg val");
            }
        }
        reg++;
    }

    log_str(INFO, LOG_GPIO_EXPAND, true, "Expander scan: %d/%d passed.", correct_count, all_regs_count);

    return GPIO_SUCCESS;
}

/**
 * @brief Configure a GPIO expander pin to be an output.
 *
 * Updates the internal copy of the expander registers and writes the configuration to the device.
 *
 * @pre @ref obc_i2c_init from @ref orcasat/interfaces/i2c_freertos.h has been called.
 *
 * @param[in] port        GPIO_EXP_PORT_0 or GPIO_EXP_PORT_1
 * @param[in] pin         0-7
 * @param[in] default_val 0 or 1, corresponding to LOW or HIGH output when the pin is configured
 * @param[in] pull_sel    @ref pcal6416a_pull_cfg_t PULLUP, PULLDOWN, NONE (floating)
 * @return GPIO_SUCCESS indicating that configuration had no IO errors, or GPIO_I2C_ERR indicating
 * an IO error when communicating with the expander over I2C.
 */
gpio_err_t pcal6416a_configure_output(pcal6416a_port_t port, uint8_t pin, bool default_val, pcal6416a_pull_cfg_t pull_sel) {
     // Set the default high/low output state
    if (configure_bit(&rw_regs.output_val[port], pin, default_val) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    // Configure pulls
    if (configure_pull_regs(port, pin, pull_sel) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    // Configure as output
    return configure_bit(&rw_regs.direction[port], pin, DIRECTION_OUTPUT);
}

/**
 * @brief Configure a GPIO expander pin to be an input.
 *
 * Updates the internal copy of the expander registers and writes the configuration to the device.
 *
 * @pre @ref obc_i2c_init from @ref orcasat/interfaces/i2c_freertos.h has been called.
 * @warning Latching capability is turned off, so reading the pin will always return the current
 * value seen at the pin.
 *
 * @param[in] port     GPIO_EXP_PORT_0 or GPIO_EXP_PORT_1
 * @param[in] pin      0-7
 * @param[in] pull_sel @ref pcal6416a_pull_cfg_t PULLUP, PULLDOWN, NONE (floating)
 * @return GPIO_SUCCESS indicating that configuration had no IO errors, or GPIO_I2C_ERR indicating
 * an IO error when communicating with the expander over I2C.
 */
gpio_err_t pcal6416a_configure_input(pcal6416a_port_t port, uint8_t pin, pcal6416a_pull_cfg_t pull_sel) {
    // Set the output value to 0 for good measure
    if (configure_bit(&rw_regs.output_val[port], pin, 0) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    if (configure_pull_regs(port, pin, pull_sel) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    // Disable latching
    if (configure_bit(&rw_regs.input_latch[port], pin, 0) != GPIO_SUCCESS) { 
        return GPIO_I2C_ERR;
    }

    // Configure as input
    return configure_bit(&rw_regs.direction[port], pin, DIRECTION_INPUT);
}

/**
 * @brief Configure a GPIO expander pin to be an input with an interrupt callback.
 *
 * Updates the internal copy of the expander registers and writes the configuration to the device.
 *
 * @pre @ref obc_i2c_init from @ref orcasat/interfaces/i2c_freertos.h has been called.
 *
 * Pullup and pulldown values are nominally 100 kΩ
 * @warning You will usually want to configure pins to be latched (latch = 1), or else a quickly
 * changing pin's value that triggered the interrupt may not be accurately known when FW gets around
 * to checking the interrupt pin.
 * @warning Interrupts are generated on rising and falling edges of pins.
 * @param[in] port     GPIO_EXP_PORT_0 or GPIO_EXP_PORT_1
 * @param[in] pin      0-7
 * @param[in] latch    if true the pin value when the interrupt fires will be latched by the expander,
 *                     otherwise the interrupt will be cleared if the pin value changes again
 * @param[in] pull_sel @ref pcal6416a_pull_cfg_t PULLUP, PULLDOWN, NONE (floating)
 * @param[in] callback Function pointer (see pcal6416a_irq_callback_t) that will be called when the
 *                     expander raises an interrupt on the pin.
 * @return GPIO_SUCCESS indicating that configuration had no IO errors, or GPIO_I2C_ERR indicating
 *         an IO error when communicating with the expander over I2C.
 */
gpio_err_t pcal6416a_configure_interrupt(pcal6416a_port_t port, uint8_t pin, bool latch, pcal6416a_pull_cfg_t pull_sel, pcal6416a_irq_callback_t callback) {
    if (configure_bit(&rw_regs.output_val[port], pin, 0) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    if (configure_pull_regs(port, pin, pull_sel) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    if (configure_bit(&rw_regs.direction[port], pin, DIRECTION_INPUT) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    if (configure_bit(&rw_regs.input_latch[port], pin, latch) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    // Setting interrupt mask to 0 enables the interrupt on that pin
    if (configure_bit(&rw_regs.interrupt_mask[port], pin, 0) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    // Store the callback
    irq_callbacks[port][pin] = callback;
    return GPIO_SUCCESS;
}

/**
 * @brief Write a value to a previously-configured output pin.
 * 
 * @pre @ref obc_i2c_init from @ref orcasat/interfaces/i2c_freertos.h has been called.
 * @pre pcal6416a_configure_output() has been called
 * 
 * @param[in] port GPIO_EXP_PORT_0 or GPIO_EXP_PORT_1
 * @param[in] pin  0-7
 * @param[in] val  0 = LOW, 1 = HIGH
 * @return GPIO_SUCCESS indicating that set had no IO errors, or GPIO_I2C_ERR indicating an IO error
 * when communicating with the expander over I2C.
 */
gpio_err_t pcal6416a_gpio_write(pcal6416a_port_t port, uint8_t pin, bool val) {
    return configure_bit(&rw_regs.output_val[port], pin, val);
}

/**
 * Get input value from GPIO expander pin.
 *
 * Reads the input state of GPIO expander pins.
 *
 * @pre @ref obc_i2c_init() from @ref orcasat/interfaces/i2c_freertos.h has been called.
 * @pre Pin is configured with @ref pcal6416a_configure_input() or else garbage will be read.
 *
 * @param[in]  port GPIO_EXP_PORT_0 or GPIO_EXP_PORT_1
 * @param[in]  pin  0-7
 * @param[out] val  Provide a uint32_t to store the read value (0 = LOW, 1 = HIGH)
 * @return GPIO_SUCCESS indicating @ref val is valid, or GPIO_I2C_ERR indicating an IO error when
 * reading from the expander over I2C
 */
gpio_err_t pcal6416a_gpio_read(pcal6416a_port_t port, uint8_t pin, uint32_t* val) {
    uint8_t rx_data[1] = {0};

    if (i2c_read_reg(&ro_regs.input_value[port], rx_data) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    *val = (rx_data[0] >> pin) & 1U; // Return 0 or 1 based on the pin's value, mask off anything else.
    return GPIO_SUCCESS;
}

/**
 * @brief Check interrupt status when GPIO expander IRQ is reported.
 *
 *	Check all IRQ status bits, call appropriate pre-configured callbacks.
 *	@pre callbacks are registered and pins are set up with @ref pcal6416a_configure_interrupt
 */
void pcal6416a_handle_interrupts(void) {
    uint8_t interrupt_status[2] = {0x00, 0x00};

    // Check interrupt status on each port
    if (i2c_read_reg(&ro_regs.interrupt_status[0], &interrupt_status[0]) != GPIO_SUCCESS) {
        log_str(ERROR, LOG_GPIO_EXPAND, true, "Expander irq check fail");
    }

    if (i2c_read_reg(&ro_regs.interrupt_status[1], &interrupt_status[1]) != GPIO_SUCCESS) {
        log_str(ERROR, LOG_GPIO_EXPAND, true, "Expander irq check fail");
    }

    // Iterate over the ports
    uint8_t port;
    for (port = 0; port < NUM_PORTS; port++) {
        uint8_t status = interrupt_status[port];

        // Iterate over the bits (i.e. pins), aborting if status is/becomes 0
        uint8_t pin;
        for (pin = 0; ((pin < NUM_PINS) && (status > 0)); pin++) {
            if (((status & 1U) == 1U)) {
                // Interrupt is set --> read the pin value to clear the interrupt
                uint32_t input_val = 0;
                if (pcal6416a_gpio_read((pcal6416a_port_t)port, pin, &input_val) == GPIO_SUCCESS) {
                    // Invoke the callback
                    pcal6416a_irq_callback_t callback = irq_callbacks[port][pin];
                    if (callback != NULL) {
                        callback(input_val);
                    }
                } else {
                    // Don't call the callback if the read failed because we don't know
                    // what input value to pass in
                    log_str(ERROR, LOG_GPIO_EXPAND, false, "Read failed from IRQ handler");
                }
            }

            status >>= 1;
        }
    }
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Set a bit in the configuration structure and write the value to the GPIO expander.
 * 
 * @sideeffect Updates the value of the provided register pointer
 *
 * @param[in] pin: 0-7
 * @param[in] reg: pointer to a register that will be updated
 * @param[in] val: 0 or 1, the value that the bit will take on
 * @return GPIO_SUCCESS indicating that no IO errors occurred, or GPIO_FAILURE indicating an IO
 * error when reading from the expander over I2C
 */
static gpio_err_t configure_bit(pcal6416a_reg_t* reg, uint8_t pin, bool val) {
    if (val == 1) {
        reg->val |= (uint8_t)(1U << pin);
    } else {
        reg->val &= (uint8_t)(~(uint8_t)(1U << pin));
    }
    return i2c_write_reg(reg);
}

/**
 * @brief Set up the GPIO pull configuration registers for a pin and update them on the GPIO
 * expander.
 *
 * Pull type is set before the pull is enabled.
 * 
 * @param[in] port     GPIO_EXP_PORT_0 or GPIO_EXP_PORT_1
 * @param[in] pin      0-7
 * @param[in] pull_sel PULLUP, PULLDOWN, NONE, see @ref pcal6416a_pull_cfg_t
 * @return GPIO_SUCCESS indicating that no IO errors occurred, or GPIO_I2C_ERR indicating an IO
 * error when reading from the expander over I2C
 */
static gpio_err_t configure_pull_regs(pcal6416a_port_t port, uint8_t pin, pcal6416a_pull_cfg_t pull_sel) {
    if (configure_bit(&rw_regs.pull_select[port], pin, (uint8_t)((pull_sel >> 1) & 1U)) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    return configure_bit(&rw_regs.pull_enable[port], pin, (uint8_t)(pull_sel & 1U));
}

/**
 * @brief Write a register value to the GPIO expander.
 *
 * Values are written one register at a time - it is not possible to update a single bit.
 * The value written is the value provided by the internal representation of a GPIO expander
 * register.
 *
 * @param[in] reg: pointer to a @ref pcal6416a_reg_t containing the register value to be written, and the
 * address of the register in the GPIO expander.
 * @return GPIO_SUCCESS indicating that no IO errors occurred, or GPIO_FAILURE indicating an IO
 * error when reading from the expander over I2C
 */
static gpio_err_t i2c_write_reg(pcal6416a_reg_t* reg) {
    i2c_err_t status = tms_i2c_write(EXPANDER_ADDR, sizeof(reg->addr), &reg->addr, sizeof(reg->val), &reg->val, I2C_MUTEX_TIMEOUT_MS);
    if (status != I2C_SUCCESS) {
        log_str(ERROR, LOG_GPIO_EXPAND, true, "I2C write failure: %d", status);
        return GPIO_I2C_ERR;
    }
    return GPIO_SUCCESS;
}

/**
 * @brief Read a register value from the GPIO expander.
 *
 * @param[in]  reg     pointer to a @ref pcal6416a_reg_t - the address in this register will be read.
 * @param[out] rx_data pointer to uint8_t. Must be at least 1 byte wide, additional bytes will be unfilled.
 *                     If GPIO_SUCCESS is returned, the data returned to this pointer is valid.
 * @return GPIO_SUCCESS if the data in rx_data is valid, or GPIO_FAILURE indicating an IO error when
 * reading from the expander over I2C
 */
static gpio_err_t i2c_read_reg(const pcal6416a_reg_t* reg, uint8_t* rx_data) {
    i2c_err_t status = tms_i2c_read(EXPANDER_ADDR, sizeof(reg->addr), &reg->addr, sizeof(reg->val), rx_data, I2C_MUTEX_TIMEOUT_MS);
    if (status != I2C_SUCCESS) {
        log_str(ERROR, LOG_GPIO_EXPAND, true, "I2C read failure: %d", status);
        return GPIO_I2C_ERR;
    }
    return GPIO_SUCCESS;
}

/**
 * @brief Compare a register value read from the GPIO expander with a provided value.
 *
 * Any mismatches will be logged using the logging system.
 * @param[in] predicted the .val field will be used as the truth value to test against. The .address
 *                      field will be logged to indicate which register the mismatch occurred in.
 * @param[in] actual    the actual value, should be read from the GPIO expander.
 * @return 1 = a mismatch was detected, 0 = no mismatches were detected.
 */
static bool register_mismatch(pcal6416a_reg_t* predicted, uint8_t actual) {
    bool mismatches = false;

    if (predicted->val != actual) {
        log_str(ERROR, LOG_GPIO_EXPAND, true, "Reg: %02X, Expect: %02X Act: %02X", predicted->addr, predicted->val, actual);
        mismatches = true;
    }

    return mismatches;
}

#endif // FEATURE_GPIO_EXPANDER
