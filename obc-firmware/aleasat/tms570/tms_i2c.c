/**
 * @file tms_i2c.c
 * @brief I2C driver wrapper
 *
 * I2C driver functions that implement some error checking. Application can optionally attempt
 * to retry upon return of an error code. However, applications must only attempt a few times
 * before returning so that the bus gets freed up.
 *
 * USEFUL RESOURCES:
 *   - http://processors.wiki.ti.com/index.php/I2C_Tips (this one is great)
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "tms_i2c.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_utils.h"
#include "logger.h"
#include "log_ids.h"

// FreeRTOS
#include "rtos.h"

// HAL
#include "i2c.h"

// Standard Library
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Number of attempts before timing out (units are attempts at register check)
 * 
 * Typically takes ~3500 for a correct wait
 */
#define I2C_TIMEOUT_MAX 200000

/**
 * @brief I2C pin function bit value for I2C mode
 */
#define I2CPFNC_I2C 0U

/**
 * @brief I2C pin function bit value for GPIO mode
 */
#define I2CPFNC_IO  1U

/**
 * @brief SCL pulse width for I2C reset function, measured in for-loop ticks (NOT FREERTOS TICKS)
 */
#define I2C_RESET_PULSE_WIDTH 300

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Fields for the following registers: I2CDIN, I2CDOUT, I2CPDIR
 */
typedef enum {
    I2C_SDA = 2U, // Serial data
    I2C_SCL = 1U  // Serial clock
} i2c_gpio_in_t;

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static SemaphoreHandle_t xI2CMutex;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void i2c_init_voltage_levels(void);
static i2c_err_t i2c_reset(uint8_t max_retry);
static i2c_err_t i2c_read(uint8_t addr, uint8_t reg_bytes, const uint8_t* reg_data, uint32_t rcv_bytes, uint8_t* rcv_data);
static i2c_err_t i2c_write(uint8_t addr, uint8_t reg_bytes, const uint8_t* reg_data, uint32_t send_bytes, const uint8_t* send_data);

static inline void i2c_setup_transaction(uint8_t addr, uint32_t dir, uint32_t num_bytes);
static i2c_err_t i2c_send(uint8_t reg_bytes, const uint8_t* reg_data, uint32_t send_bytes, const uint8_t* send_data);
static inline i2c_err_t i2c_send_byte(uint8_t byte);
static i2c_err_t i2c_receive(uint32_t length, uint8_t* data);
static inline void i2c_clear_nack(void);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Instantiates the mutex used to protect I2C access
 */
void tms_i2c_create_infra(void) {
    static StaticSemaphore_t xI2CStaticMutex;
    xI2CMutex = xSemaphoreCreateMutexStatic(&xI2CStaticMutex);
}

/**
 * @brief Initializes I2C
 *
 * Function has to be called before using any I2C devices
 */
void tms_i2c_init(void) {
    /*
     * FIX: voltage levels are low when initializing I2C pins without initializing as GIO pins first
     */
    i2c_init_voltage_levels();

    i2cInit();
}

/**
 * @brief Wrapper around i2c_reset to add synchronization via a mutex
 * 
 * See i2c_reset for information about the parameters (other than mtx_timeout_ms).
 * 
 * @param[in] mtx_timeout_ms Timeout (in milliseconds) to acquire the mutex and
 *                           gain control of the I2C peripheral. This does not
 *                           include completing the reset.
 */
i2c_err_t tms_i2c_reset(uint8_t max_retry, uint32_t mtx_timeout_ms) {
    i2c_err_t result;
    if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(mtx_timeout_ms)) == pdTRUE) {
        result = i2c_reset(max_retry);
        xSemaphoreGive(xI2CMutex);
    } else {
        result = I2C_MUTEX_TIMEOUT;
    }
    return result;
}

/**
 * @brief Wrapper around i2c_read to add synchronization via a mutex
 * 
 * See i2c_read for information about the parameters (other than mtx_timeout_ms).
 * 
 * @param[in] mtx_timeout_ms Timeout (in milliseconds) to acquire the mutex and
 *                           gain control of the I2C peripheral. This does not
 *                           include completing the I2C transaction.
 */
i2c_err_t tms_i2c_read(uint8_t addr, uint8_t reg_bytes, const uint8_t* reg_data,
                       uint32_t rcv_bytes, uint8_t* rcv_data, uint32_t mtx_timeout_ms) {

    i2c_err_t result;
    if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(mtx_timeout_ms)) == pdTRUE) {
        result = i2c_read(addr, reg_bytes, reg_data, rcv_bytes, rcv_data);
        xSemaphoreGive(xI2CMutex);
    } else {
        result = I2C_MUTEX_TIMEOUT;
    }
    return result;
}

/**
 * @brief Wrapper around i2c_write to add synchronization via a mutex
 * 
 * See i2c_write for information about the parameters (other than mtx_timeout_ms).
 * 
 * @param[in] mtx_timeout_ms Timeout (in milliseconds) to acquire the mutex and
 *                           gain control of the I2C peripheral. This does not
 *                           include completing the I2C transaction.
 */
i2c_err_t tms_i2c_write(uint8_t addr, uint8_t reg_bytes, const uint8_t* reg_data,
                        uint32_t send_bytes, const uint8_t* send_data, uint32_t mtx_timeout_ms) {

    i2c_err_t result;
    if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(mtx_timeout_ms)) == pdTRUE) {
        result = i2c_write(addr, reg_bytes, reg_data, send_bytes, send_data);
        xSemaphoreGive(xI2CMutex);
    } else {
        result = I2C_MUTEX_TIMEOUT;
    }
    return result;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Initialize I2C pins as GPIO in order to get correct voltage levels.
 *
 * 	Must initialize:
 * 	  - default values to high
 * 	  - direction to input/output
 * 	  - enable open drain
 * 	In order for I2C module to work correctly.
 *
 * 	Must then wait a 1 tick delay.
 */
static void i2c_init_voltage_levels(void)
{
    // Enter reset
    i2cREG1->MDR = (uint32_t)((uint32_t)0U << 5U);

    // Set I2C pins functional mode
    i2cREG1->PFNC = (1U);

    // Set I2C pins default output value
    i2cREG1->DOUT = (uint32_t)((uint32_t)1U << 1U) // SDA pin
                  | (uint32_t)(1U);                // SCL pin

    // Set I2C pins output direction
    i2cREG1->DIR  = (uint32_t)((uint32_t)1U << 1U) // SDA pin
                  | (uint32_t)(1U);                // SCL pin

    // Set I2C pins open drain enable
    i2cREG1->PDR  = (uint32_t)((uint32_t)1U << 1U) // SDA pin
                  | (uint32_t)(1U);                // SCL pin

    // Bring I2C out of reset
    i2cREG1->MDR |= (uint32_t)I2C_RESET_OUT;

    // Wait for a single tick. This is necessary, otherwise pin voltage doesn't settle
    vTaskDelay(pdMS_TO_TICKS(10));
}

/**
 * @brief Reset the I2C bus. The bus can hang for many reasons, so out error handlers
 * must therefore be able to reset the bus. To do this, we reset the I2C peripheral
 * and clock out 10 pulses: http://www.microchip.com/forums/m175368.aspx
 *
 * @pre Should hold the I2C mutex
 *
 * @param[in] max_retry Maximum number of failed resets allowed before error code is returned
 * @returns I2C module status
 */
static i2c_err_t i2c_reset(uint8_t max_retry) {
    uint8_t num_retry = 0;

    vTaskSuspendAll(); // Don't want any scheduling happening while we reset
    do {
        I2C->MDR &= ~(uint32_t)I2C_RESET_OUT; // Reset i2c peripheral
        I2C->PFNC = I2CPFNC_IO;               // I2C pins to GPIO mode
        I2C->DIR  = I2C_SCL;                  // SDA is input [bit 1], SCL is output [bit 0], output = 1

        // Send out some pulses
        uint8_t i;
        for (i = 0; i < 10; i++) {
            I2C->DOUT |= I2C_SCL;   // Set SCL high
            busy_wait(I2C_RESET_PULSE_WIDTH);
            I2C->DOUT ^= I2C->DOUT; // Set SCL low
            busy_wait(I2C_RESET_PULSE_WIDTH);
        }
        I2C->DOUT |= I2C_SCL; // Set SCL high

        // Check that the data line has gone high (idle)
        if ((I2C->DIN & I2C_SDA) == I2C_SDA) {
            break;
        } else if (num_retry >= max_retry) {
            xTaskResumeAll();
            log_str(ERROR, LOG_I2C, "I2C max resets hit");
            return I2C_ERR_RESET;
        } else {
            ++num_retry;
        }
    } while (true);

    // Directly call HALCoGen init function because tms_i2c_init eventually calls vTaskDelay
    // which is forbidden while the scheduler is suspended
    i2cInit();

    xTaskResumeAll();

    log_str(DEBUG, LOG_I2C, "I2C RESET");
    return I2C_SUCCESS;
}

/**
 * @brief Reads specified number of bytes from I2C device
 *
 * @pre Should hold the I2C mutex
 *
 * @param[in]  addr      I2C device address
 * @param[in]  reg_bytes Size of the register address in bytes
 * @param[in]  reg_data  Pointer to the register address bytes
 * @param[in]  rcv_bytes Number of bytes to read from the device
 * @param[out] rcv_data  Buffer to store the read bytes
 *
 * @returns I2C module status
 */
static i2c_err_t i2c_read(uint8_t addr, uint8_t reg_bytes, const uint8_t* reg_data, uint32_t rcv_bytes, uint8_t* rcv_data) {
    i2c_err_t err = I2C_SUCCESS;
    if (i2cIsBusBusy(I2C)) {
        return I2C_BUS_BUSY;
    }

    i2c_setup_transaction(addr, I2C_TRANSMITTER, reg_bytes);

    // Send the register to read from
    I2C->MDR &= ~((uint32_t)I2C_STOP_COND); // Clear STOP condition since this is immediately followed by a read
    i2cSetStart(I2C);
    err = i2c_send(reg_bytes, reg_data, 0, NULL);
    if (err != I2C_SUCCESS) {
        i2cSetStop(I2C);
        i2cClearSCD(I2C);

        if (err == I2C_ERR_NACK) {
            log_str(ERROR, LOG_I2C, "NACK from dev 0x%02x", addr);
            i2c_clear_nack();
        }
        return err;
    }

    // Receive the data from the slave
    i2cSetDirection(I2C, I2C_RECEIVER);
    i2cSetCount(I2C, rcv_bytes);
    I2C->MDR |= (I2C_STOP_COND | I2C_START_COND);

    err = i2c_receive(rcv_bytes, rcv_data);
    if (err != I2C_SUCCESS) {
        i2cSetStop(I2C);
        i2cClearSCD(I2C);
        return err;
    }

    // Wait for a stop condition to occur
    uint32_t timeout_count;
    for (timeout_count = 0; timeout_count < I2C_TIMEOUT_MAX; timeout_count++) {
        if (i2cIsStopDetected(I2C)) {
            i2cClearSCD(I2C);
            return I2C_SUCCESS;
        }
    }

    return I2C_RX_TIMEOUT;
}

/**
 * @brief Writes specified number of bytes to I2C device
 *
 * @pre Should hold the I2C mutex
 *
 * @param[in] addr       I2C device address
 * @param[in] reg_bytes  Size of the register address in bytes
 * @param[in] reg_data   Pointer to the register address bytes
 * @param[in] send_bytes Size of the data in bytes
 * @param[in] send_data  Pointer to the data bytes
 *
 * @returns I2C module status
 */
static i2c_err_t i2c_write(uint8_t addr, uint8_t reg_bytes, const uint8_t* reg_data, uint32_t send_bytes, const uint8_t* send_data) {
    i2c_err_t err = I2C_SUCCESS;
    if (i2cIsBusBusy(I2C)) {
        return I2C_BUS_BUSY;
    }

    i2c_setup_transaction(addr, I2C_TRANSMITTER, (reg_bytes + send_bytes));

    // Set the stop bit so that a stop condition is generated by the
    // hardware once all bytes are sent
    i2cSetStop(I2C);

    // Send the data to the slave
    i2cSetStart(I2C);
    err = i2c_send(reg_bytes, reg_data, send_bytes, send_data);
    if (err != I2C_SUCCESS) {
        i2cSetStop(I2C);
        i2cClearSCD(I2C);

        if (err == I2C_ERR_NACK) {
            log_str(ERROR, LOG_I2C, "NACK from dev 0x%02x", addr);
            i2c_clear_nack();
        }

        return err;
    }

    // Clear stop condition, note that the i2c_send function guarantees a stop
    // condition will occur before it exits successfully when the STP bit is set
    i2cClearSCD(I2C);

    return I2C_SUCCESS;
}

/**
 * @brief Sets up the I2C bus for a transaction, required before sending data
 *
 * @param[in] addr      Address of I2C slave device
 * @param[in] dir       Mode of I2C module (I2C_TRANSMITTER or I2C_RECEIVER)
 * @param[in] num_bytes Number of bytes sent / transmitted for this transaction
 */
static inline void i2c_setup_transaction(uint8_t addr, uint32_t dir, uint32_t num_bytes) {
    i2cSetMode(I2C, I2C_MASTER);
    i2cSetSlaveAdd(I2C, addr);
    i2cSetDirection(I2C, dir);
    i2cSetCount(I2C, num_bytes);
}

/**
 * @brief Sends a register address followed by data to a slave I2C device
 *
 * @param[in] reg_bytes  Size of the register address in bytes
 * @param[in] reg_data   Pointer to the register address bytes
 * @param[in] send_bytes Size of the data in bytes
 * @param[in] send_data  Pointer to the data bytes
 *
 * @returns I2C module status
 *            - I2C_TX_TIMEOUT if timeout occurred waiting for bus to free
 *            - I2C_ERR_NACK if NACK was received
 *            - I2C_SUCCESS otherwise
 */
static i2c_err_t i2c_send(uint8_t reg_bytes, const uint8_t* reg_data, uint32_t send_bytes, const uint8_t* send_data) {
    // Send register
    for (; reg_bytes > 0; reg_bytes--) {
        i2c_err_t status = i2c_send_byte(*reg_data);
        if (status != I2C_SUCCESS) {
            return status;
        }
        reg_data++;
    }

    // Send data
    for (; send_bytes > 0; send_bytes--) {
        i2c_err_t status = i2c_send_byte(*send_data);
        if (status != I2C_SUCCESS) {
            return status;
        }
        send_data++;
    }

    // Check if STP bit is set. If it is we wait for stop condition otherwise
    // we wait for ARDY to be asserted. The first case occurs during i2c_write,
    // the second case occurs during i2c_read when we write the register address.
    uint32_t stop_set = I2C->MDR & I2C_STOP_COND;

    // Wait for the send to complete
    uint32_t timeout_count = 0;
    for (timeout_count = 0; timeout_count < I2C_TIMEOUT_MAX; timeout_count++) {
        uint32_t status_reg = I2C->STR;

        // Check that slave hasn't NACKed
        if (status_reg & I2C_NACK) {
            return I2C_ERR_NACK;
        }

        // Check that send has completed
        if (stop_set) {
            if (status_reg & I2C_SCD) {
                return I2C_SUCCESS;
            }
        } else {
            if (status_reg & I2C_ARDY) {
                return I2C_SUCCESS;
            }
        }
    }

    return I2C_TX_TIMEOUT;
}

/**
 * @brief Sends a single byte on the I2C interface
 * 
 * @param[in] byte The byte to send
 * @returns I2C module status
 *            - I2C_TX_TIMEOUT if timeout occurred waiting for bus to free
 *            - I2C_ERR_NACK if NACK was received
 *            - I2C_SUCCESS otherwise
 */
static inline i2c_err_t i2c_send_byte(uint8_t byte) {
    uint32_t timeout_count = 0;
    for (timeout_count = 0; true; timeout_count++) {
        uint32_t status_reg = I2C->STR;

        // Check that slave hasn't NACKed
        if (status_reg & I2C_NACK) {
            return I2C_ERR_NACK;
        }

        // Check if data register is ready to accept another byte
        if (status_reg & I2C_TX) {
            break;
        }

        // Check timeout
        if (timeout_count >= I2C_TIMEOUT_MAX) {
            return I2C_TX_TIMEOUT;
        }
    }

    I2C->DXR = byte;
    return I2C_SUCCESS;
}

/**
 * @brief Receive a specified number of bytes from a slave I2C device
 *
 * @param[in]  length Number of bytes to wait for
 * @param[out] data   Buffer to store received data
 *
 * @returns I2C module status
 *            - I2C_RX_FAIL if timeout occurred waiting for data
 *            - I2C_SUCCESS otherwise
 */
static i2c_err_t i2c_receive(uint32_t length, uint8_t* data) {
    for (; length > 0; length--) {
        uint32_t timeout_count = 0;
        for (timeout_count = 0; true; timeout_count++) {
            // Check if we have received the next byte
            if (I2C->STR & I2C_RX) {
                break;
            }

            // Check timeout
            if (timeout_count >= I2C_TIMEOUT_MAX) {
                return I2C_RX_TIMEOUT;
            }
        }

        *data = I2C->DRR;
        data++;
    }

    return I2C_SUCCESS;
}

/**
 * @brief Clears the NACK condition by resetting the NACK bit and the TX ready
 * bit in the status register.
 */
static inline void i2c_clear_nack(void) {
    I2C->STR |= (I2C_NACK | I2C_TX);
}
