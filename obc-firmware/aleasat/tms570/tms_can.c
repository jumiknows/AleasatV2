/**
 * @file tms_can.c
 * @brief Allows reading and writing from CAN ports as GPIO
 * Refer to TMS570LS0714 Technical Reference Manual
 * https://www.ti.com/lit/ug/spnu607a/spnu607a.pdf?ts=1617497224462&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FTMS570LS0714
 * pages 1296 - 1298 for relevant register descriptions
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "tms_can.h"

// HAL
#include "reg_can.h"
#include "can.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Position of Out bit in CAN RIOC/TIOC register
 */
#define CAN_OUT_BIT_POS 1U

/**
 * @brief Position of In bit in CAN RIOC/TIOC register
 */
#define CAN_IN_BIT_POS  0U

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef volatile uint32_t* canreg_ioc_ptr_t;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static canreg_ioc_ptr_t get_canreg_ioc_ptr(canBASE_t* port, can_pin_t pin);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize CAN with HALCoGen settings
 */
void tms_can_init(void) {
    canInit();
}

/**
 * @brief Write a value to the specified pin of the specified CAN port acting as a GPIO.
 * 
 * This function does not have any built-in thread safety. Appropriate synchronization should
 * be handled by the calling code if multiple tasks are reading/writing or re-configuring
 * the same CAN pin.
 * 
 * @param[in] port  Pointer to appropriate canREG
 * @param[in] pin   Pin to write to (CAN_PIN_RX or CAN_PIN_TX)
 * @param[in] value Bit value to set, 0 = LOW, 1 = HIGH
 */
void tms_can_gpio_write(canBASE_t* port, can_pin_t pin, uint32_t value) {
    canreg_ioc_ptr_t reg = get_canreg_ioc_ptr(port, pin);

    // Keep register set to output and move value into output position
    if (value != 0U)
    {
        // Set Out bit
        *reg |= ((uint32_t)1U << CAN_OUT_BIT_POS);
    }
    else
    {
        // Clear Out bit
        *reg &= ~((uint32_t)1U << CAN_OUT_BIT_POS);
    }
}

/**
 * @brief Read a value from the specified pin of the specified CAN port acting as a GPIO.
 * 
 * @param[in] port Pointer to appropriate canREG
 * @param[in] pin  Pin to read from (CAN_PIN_RX or CAN_PIN_TX)
 * @return The value read
 */
uint32_t tms_can_gpio_read(canBASE_t* port, can_pin_t pin) {
    canreg_ioc_ptr_t reg = get_canreg_ioc_ptr(port, pin);
    return ((*reg >> CAN_IN_BIT_POS) & 1U);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Get a pointer to the IOC register for the specified CAN port/pin
 * 
 * @param[in] port Pointer to CAN port
 * @param[in] pin  Pin to determine which IOC register is used
 * @return Pointer to the IOC register for the specified CAN port/pin
 */
static canreg_ioc_ptr_t get_canreg_ioc_ptr(canBASE_t* port, can_pin_t pin) {
    if (pin == CAN_PIN_RX) {
        return (canreg_ioc_ptr_t)(&(port->RIOC));
    } else {
        return (canreg_ioc_ptr_t)(&(port->TIOC));
    }
}
