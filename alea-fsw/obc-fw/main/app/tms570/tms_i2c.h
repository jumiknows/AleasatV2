/**
 * @file tms_i2c.h
 * @brief I2C driver wrapper
 *
 *      - Main goal of the tms_i2c adaptations from halcogen I2C functions is to ensure that we
 * never get stuck in an infinite loop
 *      - DO NOT use the halcogen functions that do anything more than set or read a register.
 *          - Complicated halcogen functions will wait forever for things to happen and lock up the
 * system.
 *
 *      ERROR HANDLING
 *      - calling functions are responsible for dealing with errors.
 *      - errors should be logged in our system, and retries may optionally be attempted
 *      - retry attempts must be limited
 *
 */

#ifndef TMS_I2C_H_
#define TMS_I2C_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>
#include <stdbool.h>

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Return values for I2C functions.
 */
typedef enum {
    /**
     * @brief I2C transaction was successful.
     */
    I2C_SUCCESS = 0,

    /**
     * @brief Initialization of command queue failed.
     */
    FAILED_TO_INITIALIZE_COMMAND_QUEUE = -1,

    /**
     * @brief I2C worker write command did not have a 2 byte size (address + data).
     */
    UNEXPECTED_NUM_BYTES = -2,

    /**
     * @brief I2C worker task received an unexpected number of bytes.
     */
    CONTRACT_VIOLATION_UNEXPECTED_NUM_BYTES = -3,

    /**
     * @brief Unexpected NACK received when trying to write to and I2C device.
     */
    I2C_ERR_NACK = -4,

    /**
     * @brief Unable to start transaction because the bus is busy.
     */
    I2C_BUS_BUSY = -5,

    /**
     * @brief Timeout occurred during a transmit transaction. Can happen if the transmit data register
     * doesn't free up for the next byte, or if a stop condition is not detected at the end of transmission.
     */
    I2C_TX_TIMEOUT = -6,

    /**
     * @brief Timeout occurred during a receive transaction. Can happen if the receive data register
     * doesn't receive any new bytes, or if a stop condition is not detected at the end of transmission.
     */
    I2C_RX_TIMEOUT = -7,

    /**
     * @brief I2C bus was reset too many times.
     */
    I2C_ERR_RESET = -8,

    /**
     * @brief Timeout occurred waiting for the mutex to use the I2C bus
     */
    I2C_MUTEX_TIMEOUT = -9,
} i2c_err_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void tms_i2c_create_infra(void);
void tms_i2c_init(void);
void tms_i2c_isr(BaseType_t *xHigherPriorityTaskWoken);

i2c_err_t tms_i2c_reset(uint8_t max_retry, uint32_t mtx_timeout_ms);
i2c_err_t tms_i2c_read(uint8_t addr, uint8_t reg_bytes, const uint8_t *reg_data, uint32_t rcv_bytes, uint8_t *rcv_data, bool ignore_nack,
                       uint32_t mtx_timeout_ms);
i2c_err_t tms_i2c_write(uint8_t addr, uint8_t reg_bytes, const uint8_t *reg_data, uint32_t send_bytes, const uint8_t *send_data, bool ignore_nack,
                        uint32_t mtx_timeout_ms);

#endif // TMS_I2C_H_
