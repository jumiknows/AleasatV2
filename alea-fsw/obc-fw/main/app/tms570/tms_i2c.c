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

// FreeRTOS
#include "rtos.h"
#include "asm_utils.h"

// HAL
#include "i2c.h"

// Standard Library
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Number of ms to wait for I2C interrupts to fire
 */
#define I2C_TIMEOUT_MAX 1000

/**
 * @brief Delay before attempting a register check
 *
 * Typically takes ~150-1000 for a correct wait (usually used within a for loop)
 */
#define I2C_REGISTER_TIMEOUT 3000

/**
 * @brief I2C pin function bit value for I2C mode
 */
#define I2CPFNC_I2C 0U

/**
 * @brief I2C pin function bit value for GPIO mode
 */
#define I2CPFNC_IO  1U

/**
 * @brief Position of IGNACK (ignore NACK) bit in the EMDR (extended mode) register
 */
#define I2C_EMDR_IGNACK_BIT 1U

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
static SemaphoreHandle_t xI2CSemaphoreISR;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void i2c_init_voltage_levels(void);
static i2c_err_t i2c_reset(uint8_t max_retry);
static i2c_err_t i2c_read(uint8_t addr, uint8_t reg_bytes, const uint8_t *reg_data, uint32_t rcv_bytes, uint8_t *rcv_data, bool ignore_nack);
static i2c_err_t i2c_write(uint8_t addr, uint8_t reg_bytes, const uint8_t *reg_data, uint32_t send_bytes, const uint8_t *send_data, bool ignore_nack);

static inline void i2c_setup_transaction(uint8_t addr, uint32_t dir, uint32_t num_bytes, bool ignore_nack);
static i2c_err_t i2c_send(uint32_t reg_bytes, const uint8_t *reg_data, uint32_t send_bytes, const uint8_t *send_data);
static inline void i2c_clear_nack(void);
static inline bool i2c_force_stop_cond(void);
static void i2cReceiveInterrupt(uint32_t length, uint8_t *data);
static void i2cSendInterrupt(uint32_t reg_length, const uint8_t *reg_data, uint32_t send_length, const uint8_t *send_data);


/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Instantiates the mutex used to protect I2C access
 */
void tms_i2c_pre_init(void) {
    static StaticSemaphore_t xI2CStaticMutex;
    static StaticSemaphore_t xI2CStaticSemaphoreISR;
    xI2CMutex = xSemaphoreCreateMutexStatic(&xI2CStaticMutex);
    xI2CSemaphoreISR = xSemaphoreCreateBinaryStatic(&xI2CStaticSemaphoreISR);
}

/**
 * @brief Initializes I2C
 *
 * Function has to be called before using any I2C devices
 */
void tms_i2c_init_hw(void) {
    /*
     * FIX: voltage levels are low when initializing I2C pins without initializing as GIO pins first
     */
    i2c_init_voltage_levels();

    i2cInit();
}

/**
 * @brief ISR handler for RX events on the I2C peripheral
 *
 * @param xHigherPriorityTaskWoken Pointer to store flag that a higher priority task was woken
 */
void tms_i2c_isr(BaseType_t *xHigherPriorityTaskWoken) {
    xSemaphoreGiveFromISR(xI2CSemaphoreISR, xHigherPriorityTaskWoken);
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
i2c_err_t tms_i2c_read(uint8_t addr,
                       uint8_t reg_bytes, const uint8_t *reg_data,
                       uint32_t rcv_bytes, uint8_t *rcv_data,
                       bool ignore_nack, uint32_t mtx_timeout_ms) {

    i2c_err_t result;

    if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(mtx_timeout_ms)) == pdTRUE) {
        result = i2c_read(addr, reg_bytes, reg_data, rcv_bytes, rcv_data, ignore_nack);
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
i2c_err_t tms_i2c_write(uint8_t addr,
                        uint8_t reg_bytes, const uint8_t *reg_data,
                        uint32_t send_bytes, const uint8_t *send_data,
                        bool ignore_nack, uint32_t mtx_timeout_ms) {

    i2c_err_t result;

    if (xSemaphoreTake(xI2CMutex, pdMS_TO_TICKS(mtx_timeout_ms)) == pdTRUE) {
        result = i2c_write(addr, reg_bytes, reg_data, send_bytes, send_data, ignore_nack);
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
 *  Must initialize:
 *    - default values to high
 *    - direction to input/output
 *    - enable open drain
 *  In order for I2C module to work correctly.
 *
 *  Must then wait a 1 tick delay.
 */
static void i2c_init_voltage_levels(void) {
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

    // This is necessary, otherwise pin voltage doesn't settle
    asm_busy_wait(1920000); // Roughly 10ms
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
            asm_busy_wait(I2C_RESET_PULSE_WIDTH);
            I2C->DOUT ^= I2C->DOUT; // Set SCL low
            asm_busy_wait(I2C_RESET_PULSE_WIDTH);
        }

        I2C->DOUT |= I2C_SCL; // Set SCL high

        // Check that the data line has gone high (idle)
        if ((I2C->DIN & I2C_SDA) == I2C_SDA) {
            break;
        } else if (num_retry >= max_retry) {
            xTaskResumeAll();
            return I2C_ERR_RESET;
        } else {
            ++num_retry;
        }
    } while (true);

    // Directly call HALCoGen init function because tms_i2c_init eventually calls vTaskDelay
    // which is forbidden while the scheduler is suspended
    i2cInit();

    xTaskResumeAll();

    return I2C_SUCCESS;
}

/**
 * @brief Reads specified number of bytes from I2C device
 *
 * @pre Should hold the I2C mutex
 *
 * @param[in]  addr        I2C device address
 * @param[in]  reg_bytes   Size of the register address in bytes
 * @param[in]  reg_data    Pointer to the register address bytes
 * @param[in]  rcv_bytes   Number of bytes to read from the device
 * @param[out] rcv_data    Buffer to store the read bytes
 * @param[in]  ignore_nack Set to true to ignore NACK bits sent by the slave
 *
 * @returns I2C module status
 */
static i2c_err_t i2c_read(uint8_t addr, uint8_t reg_bytes, const uint8_t *reg_data, uint32_t rcv_bytes, uint8_t *rcv_data, bool ignore_nack) {
    i2c_err_t status = I2C_SUCCESS;

    if (i2cIsBusBusy(I2C)) {
        return I2C_BUS_BUSY;
    }

    i2c_setup_transaction(addr, I2C_TRANSMITTER, reg_bytes, ignore_nack);

    /*
     * Although repeat mode is not strictly necessary for repeated starts, there is plenty
     * of documentation regarding undefined behaviour and quirks when using repeated starts
     * with interrupts enabled due to poor documentation in the TRM or bugs in the underlying calls.
     */

    // Enable repeat mode and clear stop bit so that we can send a repeated start
    I2C->MDR |= (uint32_t)I2C_REPEATMODE;
    I2C->MDR &= ~((uint32_t)I2C_STOP_COND);

    // Send the register to read from
    i2cSetStart(I2C);
    status = i2c_send(reg_bytes, reg_data, 0, NULL);

    if (status != I2C_SUCCESS) {
        i2c_force_stop_cond();

        if (status == I2C_ERR_NACK) {
            i2c_clear_nack();
        }

        return status;
    }

    // Receive the data from the slave
    i2cSetDirection(I2C, I2C_RECEIVER);
    i2cReceiveInterrupt(rcv_bytes, rcv_data);
    i2cSetStart(I2C);

    if (xSemaphoreTake(xI2CSemaphoreISR, pdMS_TO_TICKS(I2C_TIMEOUT_MAX)) == pdTRUE) { // I2C interrupt happened
        /*
         * Interrupt fires when last byte in receive shift register (RSR) is copied into data receive register (DRR)
         * A small timeout loop is used to ensure that register access is ready (ARDY) before we force a stop condition.
         */
        for (uint16_t timeout_count = 0; timeout_count < I2C_REGISTER_TIMEOUT; timeout_count++) {
            if (I2C->STR & I2C_ARDY) {
                return i2c_force_stop_cond() ? I2C_SUCCESS : I2C_RX_TIMEOUT;
            }
        }
    }

    // Receive timed out
    i2c_force_stop_cond();
    return I2C_RX_TIMEOUT;
}

/**
 * @brief Writes specified number of bytes to I2C device
 *
 * @pre Should hold the I2C mutex
 *
 * @param[in] addr        I2C device address
 * @param[in] reg_bytes   Size of the register address in bytes
 * @param[in] reg_data    Pointer to the register address bytes
 * @param[in] send_bytes  Size of the data in bytes
 * @param[in] send_data   Pointer to the data bytes
 * @param[in] ignore_nack Set to true to ignore NACK bits sent by the slave
 *
 * @returns I2C module status
 */
static i2c_err_t i2c_write(uint8_t addr, uint8_t reg_bytes, const uint8_t *reg_data, uint32_t send_bytes, const uint8_t *send_data,
                           bool ignore_nack) {
    i2c_err_t status = I2C_SUCCESS;

    if (i2cIsBusBusy(I2C)) {
        return I2C_BUS_BUSY;
    }

    i2c_setup_transaction(addr, I2C_TRANSMITTER, reg_bytes, ignore_nack);

    // Enable repeat mode and clear stop bit so that we can send a repeated start
    I2C->MDR |= (uint32_t)I2C_REPEATMODE;
    I2C->MDR &= ~((uint32_t)I2C_STOP_COND);

    i2cSetStart(I2C);
    status = i2c_send(reg_bytes, reg_data, send_bytes, send_data);

    if (status != I2C_SUCCESS) {
        i2c_force_stop_cond();

        if (status == I2C_ERR_NACK) {
            i2c_clear_nack();
        }

        return status;
    }

    return i2c_force_stop_cond() ? I2C_SUCCESS : I2C_TX_TIMEOUT;
}

/**
 * @brief Sets up the I2C bus for a transaction, required before sending data
 *
 * @param[in] addr        Address of I2C slave device
 * @param[in] dir         Mode of I2C module (I2C_TRANSMITTER or I2C_RECEIVER)
 * @param[in] num_bytes   Number of bytes sent / transmitted for this transaction
 * @param[in] ignore_nack Set to true to ignore NACK bits sent by the slave
 */
static inline void i2c_setup_transaction(uint8_t addr, uint32_t dir, uint32_t num_bytes, bool ignore_nack) {
    i2cSetMode(I2C, I2C_MASTER);
    i2cSetSlaveAdd(I2C, addr);
    i2cSetDirection(I2C, dir);
    i2cSetCount(I2C, num_bytes);

    // Set IGNACK bit (no HAL function for this)
    uint32_t IGNNACK_mask = (1U << I2C_EMDR_IGNACK_BIT);
    uint32_t tmp_EMDR = i2cREG1->EMDR & ~IGNNACK_mask;
    i2cREG1->EMDR = tmp_EMDR | ((uint32_t)ignore_nack << I2C_EMDR_IGNACK_BIT);
}

/**
 * @brief Sends a register address followed by data to a slave I2C device
 *
 * @param[in] send_bytes Size of the data in bytes
 * @param[in] send_data  Pointer to the data bytes
 *
 * @returns I2C module status
 *            - I2C_TX_TIMEOUT if timeout occurred waiting for bus to free
 *            - I2C_ERR_NACK if NACK was received
 *            - I2C_SUCCESS otherwise
 */
static i2c_err_t i2c_send(uint32_t reg_bytes, const uint8_t *reg_data, uint32_t send_bytes, const uint8_t *send_data) {
    i2cSendInterrupt(reg_bytes, reg_data, send_bytes, send_data);

    if (xSemaphoreTake(xI2CSemaphoreISR, pdMS_TO_TICKS(I2C_TIMEOUT_MAX)) == pdTRUE) { // I2C interrupt happened
        /*
         * This interrupt is fired when the last byte is shifted into the data transmit
         * register (DXR) and before it has been copied into the transmit shift register (XSR)
         * Thus, even if the interrupt was fired, there is a possibility that the slave NACKed the last
         * byte, and we need to check both inside and outside the interrupt for this.
         * A small timeout loop to wait for the register values to update before checking them.
        */
        for (uint16_t timeout_count = 0; timeout_count < I2C_REGISTER_TIMEOUT; timeout_count++) {
            if (I2C->STR & I2C_ARDY) {
                // Check that slave hasn't NACKed
                return (I2C->STR & I2C_NACK) ? I2C_ERR_NACK : I2C_SUCCESS;
            }
        }
    }

    return (I2C->STR & I2C_NACK) ? I2C_ERR_NACK : I2C_TX_TIMEOUT;
}

/**
 * @brief Clears the NACK condition by resetting the NACK bit and the TX ready
 * bit in the status register.
 */
static inline void i2c_clear_nack(void) {
    I2C->STR |= (I2C_NACK | I2C_TX);
}

/**
 * @brief Forces a stop condition and then clears SCD.
 *
 * @returns true if stop condition was set and cleared successfully.
 */
static inline bool i2c_force_stop_cond(void) {
    // Enter repeat mode
    if (!(I2C->MDR & (uint32_t)I2C_REPEATMODE)) {
        I2C->MDR |= (uint32_t)I2C_REPEATMODE;
    }

    I2C->MDR |= (uint32_t)I2C_STOP_COND;

    for (uint16_t timeout_count = 0; timeout_count < I2C_REGISTER_TIMEOUT; timeout_count++) {
        if (I2C->STR & (uint32_t)I2C_SCD_INT) { // SCD detected
            I2C->STR = (uint32_t)I2C_SCD_INT; // Clear SCD
            I2C->MDR &= ~((uint32_t)I2C_REPEATMODE); // Repeat mode disabled
            return true;
        }
    }

    // Exit repeat mode
    I2C->MDR &= ~((uint32_t)I2C_REPEATMODE);
    return false;
}

/******************************************************************************/
/*                      I N T E R R U P T S                                   */
/******************************************************************************/

/*
 * This section contains modified HALCoGen code that improves the handling of
 * TX and RX interrupts
 */

static struct g_i2cReceive {
    uint32_t length;
    uint8_t *data;
} g_i2cReceive_t;

static struct g_i2cTransmit {
    uint8_t reg_length;
    const uint8_t *reg_data;
    uint32_t send_length;
    const uint8_t *send_data;
} g_i2cTransmit_t;

static void i2cReceiveInterrupt(uint32_t length, uint8_t *data) {
    I2C->STR = (uint32)I2C_AL_INT | (uint32)I2C_NACK_INT;

    g_i2cReceive_t.length = length;
    g_i2cReceive_t.data = data;
}

static void i2cSendInterrupt(uint32_t reg_length, const uint8_t *reg_data, uint32_t send_length, const uint8_t *send_data) {
    g_i2cTransmit_t.send_data = send_data;
    g_i2cTransmit_t.send_length = send_length;
    g_i2cTransmit_t.reg_data = reg_data;
    g_i2cTransmit_t.reg_length = reg_length;

    if (reg_length > 0U) {
        I2C->DXR = *g_i2cTransmit_t.reg_data;
        g_i2cTransmit_t.reg_data++;
        g_i2cTransmit_t.reg_length--;
    } else if (send_length > 0U) {
        I2C->DXR = *g_i2cTransmit_t.send_data;
        g_i2cTransmit_t.send_data++;
        g_i2cTransmit_t.send_length--;
    } else {
        return;
    }

    I2C->IMR |= (uint32)I2C_TX_INT;
}

#pragma CODE_STATE(i2cInterrupt, 32)
#pragma INTERRUPT(i2cInterrupt, IRQ)

void i2cInterrupt(void) {
    uint32_t vec = (I2C->IVR & 0x00000007U);

    switch (vec) {
    case 1U:
        i2cNotification(I2C, (uint32_t)I2C_AL_INT);
        break;

    case 2U:
        i2cNotification(I2C, (uint32_t)I2C_NACK_INT);
        break;

    case 3U:
        i2cNotification(I2C, (uint32_t)I2C_ARDY_INT);
        break;

    case 4U:
        /* receive */
    {
        uint8_t byte = I2C->DRR;

        if (g_i2cReceive_t.length > 0U) {
            *g_i2cReceive_t.data = byte;
            g_i2cReceive_t.data++;
            g_i2cReceive_t.length--;

            if (g_i2cReceive_t.length == 0U) {
                i2cNotification(I2C, (uint32_t)I2C_RX_INT);
            }
        }

        break;
    }

    case 5U:

        /* transmit */

        if (g_i2cTransmit_t.reg_length > 0U) {
            I2C->DXR = *g_i2cTransmit_t.reg_data;
            g_i2cTransmit_t.reg_data++;
            g_i2cTransmit_t.reg_length--;
        } else if (g_i2cTransmit_t.send_length > 0U) {
            I2C->DXR = *g_i2cTransmit_t.send_data;
            g_i2cTransmit_t.send_data++;
            g_i2cTransmit_t.send_length--;
        } else {
            I2C->IMR &= (uint32_t)(~(uint32_t)I2C_TX_INT);
            i2cNotification(I2C, (uint32_t)I2C_TX_INT);
        }

        break;

    case 6U:
        i2cNotification(I2C, (uint32_t)I2C_SCD_INT);
        break;

    case 7U:
        i2cNotification(I2C, (uint32_t)I2C_AAS_INT);
        break;

    default:
        /* phantom interrupt, clear flags and return */
        I2C->STR = 0x000007FFU;
        break;
    }
}
