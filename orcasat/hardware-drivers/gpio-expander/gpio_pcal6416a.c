/**
 * @file gpio_pcal6416a.c
 * @brief Driver for the PCAL616A GPIO expander.
 * @author Richard A
 * @date August 29, 2019
 */

#include "gpio-expander/gpio_pcal6416a.h"
#include "gpio-expander/gpio_pcal6416a_defs.h"
#include "i2c_freertos.h"
#include "sys_common.h"
#include "reg_gio.h"
#include "obc_hardwaredefs.h"
#include "FreeRTOS.h"
#include "obc_task_info.h"
#include "logger.h"
#include "assert.h"
#include "obc_misra.h"
#include "obc_featuredefs.h"
#include "gio.h"

#if GPIO_EXPANDER_EN
/**
 * @brief Timeout for I2C transactions before errors will be raised and transactions abandoned.
 */
#define I2C_TIMEOUT_MS 500

/* Default Expander RW Register Configuration
 * 	- All pins are inputs
 * 	- Identical to device's power-on configuration
 */

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
 */
gpio_expand_rw_t expander = {
    // PORT 0							     		PORT 1
    .output_val      = {{.val = 0x00, .addr = OUTPUT_PORT_0}, {.val = 0x00, .addr = OUTPUT_PORT_1}},           // 1 = high, 0 = low
    .polarity_invert = {{.val = 0x00, .addr = POL_INV_PORT_0}, {.val = 0x00, .addr = POL_INV_PORT_1}},         // always set 0 (normal polarity)
    .input           = {{.val = 0xFF, .addr = CONFIG_PORT_0}, {.val = 0xFF, .addr = CONFIG_PORT_1}},           // 1 = input (default), 0 = output
    .output_drive_a  = {{.val = 0xFF, .addr = OUTPUT_DRV_STR_0}, {.val = 0xFF, .addr = OUTPUT_DRV_STR_1}},     // Leave at default values
    .output_drive_b  = {{.val = 0xFF, .addr = OUTPUT_DRV_STR_A0}, {.val = 0xFF, .addr = OUTPUT_DRV_STR_A1}},   // Leave at default values
    .input_latch     = {{.val = 0x00, .addr = INPUT_LATCH_REG_0}, {.val = 0x00, .addr = INPUT_LATCH_REG_1}},   // 1 = inputs are latched, 0 = inputs not latched
    .pull_enable     = {{.val = 0x00, .addr = PULL_UP_DOWN_ENA_0}, {.val = 0x00, .addr = PULL_UP_DOWN_ENA_1}}, // 1 = pull up/down is enabled, 0 = disabled
    .pull_select     = {{.val = 0x00, .addr = PULL_UP_DOWN_SEL_0}, {.val = 0x00, .addr = PULL_UP_DOWN_SEL_1}}, // 1 = pullup, 0 = pulldown
    .interrupt_mask  = {{.val = 0xFF, .addr = IRQ_MASK_0}, {.val = 0xFF, .addr = IRQ_MASK_1}},                 // 1 = masked, 0 = enabled
    .output_port_cfg = {.val = 0x00, .addr = OUTPUT_PORT_CONFIG}                                               // 1 = open drain, 0 = push-pull.
                                                                                                               // Both configured to push-pull.
};

/**
 * @brief On-board copy of read-only (RO) registers.
 *
 * RO registers are the input state and the interrupt status. These can't be checked by OBC firmware
 * because their values are dependent on what is hooked up to the GPIO expander.
 */
gpio_expand_ro_t expander_ro_regs = {.input_value      = {{.val = 0x00, .addr = INPUT_PORT_0}, {.val = 0x00, .addr = INPUT_PORT_1}},
                                     .interrupt_status = {{.val = 0x00, .addr = IRQ_STATUS_0}, {.val = 0x00, .addr = IRQ_STATUS_1}}};

/**
 * @brief Mapping between interrupt port/pin and the callback function to execute when the interrupt
 * fires.
 */
typedef struct irq_callback {
    gioPORT_t* port;
    uint8_t pin;
    void (*callback)(uint32_t input_val);
} irq_pin_t;

/**
 * @brief Container for all interrupt callbacks registered with @ref configure_input_interrupt().
 *
 * Each time the expander's IRQ line is deasserted (it's active low), firmware runs through all
 * registered callbacks and executes them, if the configured port/pin combination is the one that
 * registered an interrupt.
 */
irq_pin_t interrupt_pins[NUM_INTERRUPT_PINS];

// Private API
static gpio_err_t configure_pull_regs(gioPORT_t* port, uint8_t pin,
                                      pull_cfg_t pull_sel); // Configure - updates the GPIO expander
static gpio_err_t configure_bit(uint8_t pin, gpio_reg_t* reg,
                                bool val); // Configure - updates the GPIO expander
static void setup_bit(uint8_t pin, gpio_reg_t* reg,
                      bool val);                                      // Setup - only updates the struct, does not write to expander
static gpio_err_t i2c_write_reg(gpio_reg_t* reg);                     // Write a register
static gpio_err_t i2c_read_reg(gpio_reg_t* reg, uint8_t* rx_data);    // Read a register
static gpio_err_t issue_i2c_submission(const i2c_command_t* command); // Issue I2C commands to the worker task
static bool register_mismatch(gpio_reg_t* predicted,
                              uint8_t actual);  // Compare register structures, typically expected
                                                // (internal) vs. values on the chip
static uint8_t handle_to_port(gioPORT_t* port); // Convert gioPort_t* to port 0 or port 1 based on settings in obc_gpio.h
static uint8_t port_to_idx(uint8_t port,
                           uint8_t pin); // Converts a port/pin into an index between 0-15

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
 * expander, which may be modified with calls like @ref configure_output().
 * @return GPIO_SUCCESS indicating that initialization was successful, or GPIO_I2C_ERR indicating an
 * IO error when communicating with the expander over I2C.
 */
gpio_err_t init_gpio_expander(void) {
    gpio_reg_t* reg = (gpio_reg_t*)&expander;
    uint8_t i       = 0;

    /* Iterate through each reg in the RW representation and write the configuration to the device
     */
    for (i = 0; i < (sizeof(gpio_expand_rw_t) / sizeof(gpio_reg_t)); i++) {
        if (i2c_write_reg(reg) != GPIO_SUCCESS) {
            return GPIO_I2C_ERR;
        }
        reg++;
    }

    return GPIO_SUCCESS;
}

/**
 * @brief Runs through all RW registers in the GPIO expander and compares their values to the
 * internal @ref expander representation. Mismatches are printed.
 *
 * The register values inside the device may change because of single event upsets. By periodically
 * verifying the gpio expander and running @ref init_gpio_expander() if any mismatches are seen, we
 * can ensure that the device is properly configured most of the time.
 *
 * @pre @ref obc_i2c_init from @ref orcasat/interfaces/i2c_freertos.h has been called.
 *
 * @return GPIO_SUCCESS indicating that verification had no IO errors, or GPIO_I2C_ERR indicating an
 * IO error when communicating with the expander over I2C.
 */
gpio_err_t verify_gpio_expander(void) {
    gpio_reg_t* reg              = (gpio_reg_t*)&expander;
    uint8_t i                    = 0;
    uint8_t actual_reg_val       = 0;
    const uint8_t all_regs_count = (sizeof(gpio_expand_rw_t) / sizeof(gpio_reg_t));
    uint8_t correct_count        = all_regs_count;

    // Iterate through each RW register pair on the device, compare with expected value, and attempt
    // to correct the problem.
    for (i = 0; i < all_regs_count; i++) {
        if (i2c_read_reg(reg, &actual_reg_val) != GPIO_SUCCESS) {
            return GPIO_I2C_ERR;
        }

        if (register_mismatch(reg, actual_reg_val)) {
            correct_count--;

            // Attempt to correct the value
            if (i2c_write_reg(reg) != GPIO_SUCCESS) {
                log_str(ERROR, GPIO_EXPAND_LOG, true, "Failed to correct reg val");
            }
        }
        reg++;
    }

    log_str(INFO, GPIO_EXPAND_LOG, true, "Expander scan: %d/%d passed.", correct_count, all_regs_count);

    return GPIO_SUCCESS;
}

/**
 * @brief Configure a GPIO expander pin to be an output.
 *
 * Updates the internal copy of the expander registers and writes the configuration to the device.
 *
 * @pre @ref obc_i2c_init from @ref orcasat/interfaces/i2c_freertos.h has been called.
 *
 * @param  port: EXPANDER_PORT_0 or EXPANDER_PORT_1
 * @param pin: 0-7
 * @param default_val: 0 or 1, corresponding to LOW or HIGH output when the pin is configured
 * @param pull_sel: @ref pull_cfg_t PULLUP, PULLDOWN, NONE (floating)
 * @return GPIO_SUCCESS indicating that configuration had no IO errors, or GPIO_I2C_ERR indicating
 * an IO error when communicating with the expander over I2C.
 */
gpio_err_t configure_output(gioPORT_t* port, uint8_t pin, bool default_val, pull_cfg_t pull_sel) {
    uint8_t port_num = handle_to_port(port);

    if (configure_bit(pin, &expander.output_val[port_num], default_val) != GPIO_SUCCESS) { // Set the default high/low output state
        return GPIO_I2C_ERR;
    }

    if (configure_pull_regs(port, pin, pull_sel) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    return configure_bit(pin, &expander.input[port_num], 0); // 0: output
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
 * @param  port: EXPANDER_PORT_0 or EXPANDER_PORT_1
 * @param pin: 0-7
 * @param default_val: 0 or 1, corresponding to LOW or HIGH output when the pin is configured
 * @param pull_sel: @ref pull_cfg_t PULLUP, PULLDOWN, NONE (floating)
 * @return GPIO_SUCCESS indicating that configuration had no IO errors, or GPIO_I2C_ERR indicating
 * an IO error when communicating with the expander over I2C.
 */
gpio_err_t configure_input(gioPORT_t* port, uint8_t pin, pull_cfg_t pull_sel) {
    uint8_t port_num = handle_to_port(port);

    if (configure_bit(pin, &expander.output_val[port_num], 0) != GPIO_SUCCESS) { // Set the output value to 0 for good measure
        return GPIO_I2C_ERR;
    }

    if (configure_pull_regs(port, pin, pull_sel) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    if (configure_bit(pin, &expander.input_latch[port_num], 0) != GPIO_SUCCESS) { // 0: input reg always matches pin state (no latching)
        return GPIO_I2C_ERR;
    }

    return configure_bit(pin, &expander.input[port_num], 1); // 1: input
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
 * @param  port: EXPANDER_PORT_0 or EXPANDER_PORT_1
 * @param pin: 0-7
 * @param default_val: 0 or 1, corresponding to LOW or HIGH output when the pin is configured
 * @param pull_sel: @ref pull_cfg_t PULLUP, PULLDOWN, NONE (floating)
 * @param callback: pointer to a void function that will be called when the expander raises an
 * interrupt on the pin.
 * @return GPIO_SUCCESS indicating that configuration had no IO errors, or GPIO_I2C_ERR indicating
 * an IO error when communicating with the expander over I2C.
 */
gpio_err_t configure_input_interrupt(gioPORT_t* port, uint8_t pin, bool latch, pull_cfg_t pull_sel, void (*callback)(uint32_t input_val)) {
    uint8_t port_num                                  = handle_to_port(port);
    static uint8_t num_callbacks_registered           = 0;
    interrupt_pins[num_callbacks_registered].port     = port;
    interrupt_pins[num_callbacks_registered].pin      = pin;
    interrupt_pins[num_callbacks_registered].callback = callback;

    num_callbacks_registered++;
    assert(num_callbacks_registered <= NUM_INTERRUPT_PINS); // Ensure that the callback array is large enough

    if (configure_bit(pin, &expander.output_val[port_num], 0) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    if (configure_pull_regs(port, pin, pull_sel) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    if (configure_bit(pin, &expander.input[port_num], 1) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    if (configure_bit(pin, &expander.input_latch[port_num], latch) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    return configure_bit(pin, &expander.interrupt_mask[port_num], 0);
}

/**
 * @brief Write a value to a previously-configured output pin.
 * @pre @ref obc_i2c_init from @ref orcasat/interfaces/i2c_freertos.h has been called.
 * @pre configure_output() has been called
 * @param  port: EXPANDER_PORT_0 or EXPANDER_PORT_1
 * @param pin: 0-7
 * @param val: 0 = LOW, 1 = HIGH
 * @return GPIO_SUCCESS indicating that set had no IO errors, or GPIO_I2C_ERR indicating an IO error
 * when communicating with the expander over I2C.
 */
gpio_err_t set_output(gioPORT_t* port, uint8_t pin, bool val) {
    uint8_t port_num = handle_to_port(port);
    return configure_bit(pin, &expander.output_val[port_num], val);
}

/**
 * Get input value from GPIO expander pin.
 *
 * Reads the input state of GPIO expander pins.
 *
 * @pre @ref obc_i2c_init() from @ref orcasat/interfaces/i2c_freertos.h has been called.
 * @pre Pin is configured with @ref configure_input() or else garbage will be read.
 *
 * @param  port: EXPANDER_PORT_0 or EXPANDER_PORT_1
 * @param  pin: 0-7
 * @param[out] val 0 provide a uint32_t to take the return value of  0 or 1, indicating the
 * requested port/pin is LOW or HIGH
 * @return GPIO_SUCCESS indicating @ref val is valid, or GPIO_I2C_ERR indicating an IO error when
 * reading from the expander over I2C
 */
gpio_err_t get_input(gioPORT_t* port, uint8_t pin, uint32_t* val) {
    uint8_t rx_data[1] = {0x00};
    uint8_t port_num   = handle_to_port(port);

    if (i2c_read_reg(&expander_ro_regs.input_value[port_num], rx_data) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    *val = (rx_data[0] >> pin) & 0x01; // Return 0 or 1 based on the pin's value, mask off anything else.
    return GPIO_SUCCESS;
}

/**
 * @brief Hard reset of GPIO expander
 */
void reset_gpio_expander(void) {
#ifdef PLATFORM_ORCA_V1
    log_str(INFO, PRINT_GENERAL, false, "Resetting GPIO expander");
    gioSetBit(OBC_EXPAND_RESET_PORT, OBC_EXPAND_RESET_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(1));
    gioSetBit(OBC_EXPAND_RESET_PORT, OBC_EXPAND_RESET_PIN, 1);
#endif /* PLATFORM_ORCA_V1 */
}

/**
 * @brief Check interrupt status when GPIO expander IRQ is reported.
 *
 *	Check all IRQ status bits, call appropriate pre-configured callbacks.
 *	@pre callbacks are registered and pins are set up with @ref configure_input_interrupt
 */
void check_expander_interrupts(void) {
    uint8_t interrupt_status[2] = {0x00, 0x00};

    /* Read the interrupt status reg on each port */
    if (i2c_read_reg(&expander_ro_regs.interrupt_status[0], &interrupt_status[0]) != GPIO_SUCCESS) {
        log_str(ERROR, GPIO_EXPAND_LOG, true, "Expander irq check fail");
    }

    if (i2c_read_reg(&expander_ro_regs.interrupt_status[1], &interrupt_status[1]) != GPIO_SUCCESS) {
        log_str(ERROR, GPIO_EXPAND_LOG, true, "Expander irq check fail");
    }

    uint16_t interrupt_bits = (uint16_t)((uint16_t)interrupt_status[1] << 8) | interrupt_status[0]; /* Stick the registers beside each other: port 1 | port 0, this lets us
                                                                                                       easily shift through and check each bit */
    uint8_t i          = 0;
    uint8_t pin        = 0;
    uint32_t input_val = 0;

    /* Check all registered interrupt-capable pins */
    for (i = 0; i < NUM_INTERRUPT_PINS; i++) {
        uint8_t port_num = handle_to_port(interrupt_pins[i].port);

        /* Waiving warning for 12.2 - value of expression shall be the same under any order of
         * evaluation (RA) */
        OBC_MISRA_CHECK_OFF
        pin = port_to_idx(port_num, interrupt_pins[i].pin);
        OBC_MISRA_CHECK_ON

        if ((interrupt_bits >> pin) == 1) {
            log_str(INFO, GPIO_EXPAND_LOG, false, "Expander IRQ port: %d pin: %d", port_num, interrupt_pins[i].pin);

            /* Read the value of the input pin, which clears the interrupt on the expander */
            /* Waiving warning for 12.2 - value of expression shall be the same under any order of
             * evaluation (RA) */
            OBC_MISRA_CHECK_OFF
            if (obc_gpio_read(interrupt_pins[i].port, interrupt_pins[i].pin, &input_val) != GPIO_SUCCESS) {
                log_str(ERROR, GPIO_EXPAND_LOG, false, "Read failed from IRQ handler");
            }
            OBC_MISRA_CHECK_ON

            /* Call the callback */
            interrupt_pins[i].callback(input_val);
        }
    }
}

/**
 * @brief Default callback for GPIO expander interrupt.
 *
 * All expander callbacks must take a uint32_t, which will be populated with the value of the pin
 * read immediately after the interrupt is raised by the GPIO expander.
 *
 * @param input val: the input value of the expander pin.
 */
void default_expander_callback(uint32_t input_val) {
    log_str(DEBUG, GPIO_EXPAND_LOG, false, "Expand IRQ callback. Val: %d", input_val);
}

/* ---- Private Helpers ---- */

/**
 * @brief Set a bit in a configuration structure.
 *
 * Use this function to update a bit on the TMS570's representation of the GPIO expander registers.
 *
 * @param pin: 0-7
 * @param [out] reg: pointer to a register that will be updated
 * @param val: 0 or 1, the value that the bit will take on
 */
static void setup_bit(uint8_t pin, gpio_reg_t* reg, bool val) {
    if (val == 1) {
        reg->val |= (uint8_t)(1 << pin);
    } else {
        reg->val &= (uint8_t)(~(uint8_t)(1 << pin));
    }
}

/**
 * @brief Set a bit in the configuration structure and write the value to the GPIO expander.
 *
 * @param pin: 0-7
 * @param [out] reg: pointer to a register that will be updated
 * @param val: 0 or 1, the value that the bit will take on
 * @return GPIO_SUCCESS indicating that no IO errors occurred, or GPIO_FAILURE indicating an IO
 * error when reading from the expander over I2C
 */
static gpio_err_t configure_bit(uint8_t pin, gpio_reg_t* reg, bool val) {
    setup_bit(pin, reg, val);
    return i2c_write_reg(reg);
}

/**
 * @brief Set up the GPIO pull configuration registers for a pin and update them on the GPIO
 * expander.
 *
 * Pull type is set before the pull is enabled.
 * @param  port: EXPANDER_PORT_0 or EXPANDER_PORT_1
 * @param pin: 0-7
 * @param pull_sel: PULLUP, PULLDOWN, NONE, see @ref pull_cfg_t
 * @return GPIO_SUCCESS indicating that no IO errors occurred, or GPIO_I2C_ERR indicating an IO
 * error when reading from the expander over I2C
 */
static gpio_err_t configure_pull_regs(gioPORT_t* port, uint8_t pin, pull_cfg_t pull_sel) {
    uint8_t port_num = handle_to_port(port);

    if (configure_bit(pin, &expander.pull_select[port_num], (uint8_t)((pull_sel & 0x03) >> 1)) != GPIO_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    return configure_bit(pin, &expander.pull_enable[port_num], (uint8_t)(pull_sel & 0x01));
}

/**
 * @brief Write a register value to the GPIO expander.
 *
 * Values are written one register at a time - it is not possible to update a single bit.
 * The value written is the value provided by the internal representation of a GPIO expander
 * register.
 *
 * @param reg: pointer to a @ref gpio_reg_t containing the register value to be written, and the
 * address of the register in the GPIO expander.
 * @return GPIO_SUCCESS indicating that no IO errors occurred, or GPIO_FAILURE indicating an IO
 * error when reading from the expander over I2C
 */
static gpio_err_t i2c_write_reg(gpio_reg_t* reg) {
    i2c_command_t command = {reg->addr, EXPANDER_RECEIVE_QUEUE_ID, EXPANDER_ADDR, WRITE_DATA, 1, &reg->val};
    return issue_i2c_submission(&command);
}

/**
 * @brief Read a register value from the GPIO expander.
 *
 * @param reg: pointer to a @ref gpio_reg_t - the address in this this register will be read.
 * @param [out] rx_data: pointer to uint8_t. Must be at least 1 byte wide, additional bytes will be
 * unfilled. If GPIO_SUCCESS is returned, the data returned to this pointer is valid.
 * @return GPIO_SUCCESS if the data in rx_data is valid, or GPIO_FAILURE indicating an IO error when
 * reading from the expander over I2C
 */
static gpio_err_t i2c_read_reg(gpio_reg_t* reg, uint8_t* rx_data) {
    i2c_command_t command = {reg->addr, EXPANDER_RECEIVE_QUEUE_ID, EXPANDER_ADDR, READ_DATA, 1, rx_data};
    return issue_i2c_submission(&command);
}

/**
 * @brief Issue a read/write command to the I2C FreeRTOS subsystem that will handle the execution of
 * the command.
 * @param command: the read/write command to issue.
 * @return GPIO_SUCCESS if the i2c subsystem executed the command without error, or GPIO_FAILURE
 * indicating an IO error when reading from the expander over I2C
 */
static gpio_err_t issue_i2c_submission(const i2c_command_t* command) {
    result_t result = I2C_SUCCESS;

    if (send_command_to_i2c_worker(command, I2C_WORKER_DEFAULT_PRIORITY) != I2C_SUCCESS) {
        return GPIO_I2C_ERR;
    }

    if (xQueueReceive(expander_rx_q, (void*)&result, pdMS_TO_TICKS(I2C_TIMEOUT_MS)) == pdFALSE) {
        log_str(ERROR, GPIO_EXPAND_LOG, true, "I2C queue rx timeout");
        return GPIO_I2C_ERR;
    }

    if (result != I2C_SUCCESS) {
        log_str(ERROR, GPIO_EXPAND_LOG, true, "I2C failure: %d", result);
        return GPIO_I2C_ERR;
    }

    return GPIO_SUCCESS;
}

/**
 * @brief Compare a register value read from the GPIO expander with a provided value.
 *
 * Any mismatches will be logged using the logging system.
 * @param predicted: the .val field will be used as the truth value to test against. The .address
 * field will be logged to indicate which register the mismatch occurred in.
 * @param actual: the actual value, should be read from the GPIO expander.
 * @return: 1 = a mismatch was detected, 0 = no mismatches were detected.
 */
static bool register_mismatch(gpio_reg_t* predicted, uint8_t actual) {
    bool mismatches = false;

    if (predicted->val != actual) {
        log_str(ERROR, GPIO_EXPAND_LOG, true, "Reg: %02X, Expect: %02X Act: %02X", predicted->addr, predicted->val, actual);
        mismatches = true;
    }

    return mismatches;
}

/**
 * @brief convert a gioPORT_t type to integer port 0 or 1.
 *
 * To maintain API compatibility across GPIO expander ports and internal TMS570 GPIO ports,
 * EXPANDER_PORT_0 and EXPANDER_PORT_1 are defined in @ref obc_gpio.h. These fake pointers are odd
 * values - values that are impossible to conflict with an internal GPIO port, which will always be
 * word-aligned because they are memory mapped. By detecting the use of these fake pointers, the
 * GPIO handlers know to call GPIO expander port functions, and the two different values correspond
 * to ports 0 and 1 on the GPIO expander chip.
 *
 * @param port: EXPANDER_PORT_0 or EXPANDER_PORT_1
 * @return: 0 if EXPANDER_PORT_0 is provided, 1 if EXPANDER_PORT_1 is provided.
 */
static uint8_t handle_to_port(gioPORT_t* port) {
    /* RA: disabling because the handle_to_port assertion fails when nRST is pressed or entered by
     * command, which causes the system to hang without completing the nRST. assert((port ==
     * EXPANDER_PORT_0) || (port == EXPANDER_PORT_1));
     *
     * See issue: 63
     */
    if (port == EXPANDER_PORT_0) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief Convert a port (0 or 1) and a pin (0-7) into a number from 0-15.
 *
 * Values 0-7 correspond to pins on port 0, values 8-15 are pins on port 1.
 * @param port: 0 or 1
 * @param pin: 0-7, the pin on each port
 * @return 0-15
 */
static uint8_t port_to_idx(uint8_t port, uint8_t pin) {
    /* RA: disabling because the handle_to_port assertion fails when nRST is pressed or entered by
    command, which causes the system to hang without completing the nRST. assert((port == 0) ||
    (port == 1));

    See issue: 63
*/
    assert(pin <= 7);

    return ((uint8_t)(port * 8) + pin);
}

#endif /* GPIO_EXPANDER_EN true */
