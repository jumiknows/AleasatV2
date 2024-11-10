/*
 * obc_i2c.c
 *
 *  Created on: Apr 22, 2018
 *      Author: Richard, Lana, & Ethan
 *
 *      I2C driver functions that implement some error checking. Application can optionally attempt
 *      to retry upon return of an error code. However, applications must only attempt a few times
 *      before returning so that the bus gets freed up.
 *
 *      USEFUL RESOURCES:
 *      - http://processors.wiki.ti.com/index.php/I2C_Tips (this one is great)
 */

#include "obc_i2c.h"
#include "obc_uart.h"
#include "obc_utils.h"
#include "i2c.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"
#include "rtos_task.h"
#include "logger.h"
#include "obc_hardwaredefs.h"

void i2c_setup_transaction(uint8_t addr, uint32_t dir);
i2c_err_t i2c_send(uint32_t length, const uint8_t* data);
i2c_err_t i2c_receive(uint32_t length, uint8_t* data);
i2c_err_t i2c_complete_transaction(void);
bool i2c_is_nack(void);
void i2c_clear_nack(void);
bool stop_detected_and_ready(void);
bool is_busy_and_not_ready(void);
static void i2c_initialize_voltage_levels(void);

// Debugging functions to quickly look at I2C register values
void i2c_print_str(void);
void i2c_print_mdr(void);

/**
 * i2c_init
 *
 * Initializes I2C
 *
 * Function has to be called before using any I2C devices
 *
 */
void i2c_init(void) {
    /*
     * FIX: voltage levels are low when initializing I2C pins without initializing as GIO pins first
     */
    i2c_initialize_voltage_levels();

    i2cInit();
}

/**
 * i2c_read
 *
 * Reads specified number of bytes from I2C device
 *
 * addr: I2C device address
 * reg_read: register address to read data from
 * bytes_rcv: number of bytes to read from the device
 * data_rcv: buffer to store read bytes
 *
 * returns: I2C module status
 */
i2c_err_t i2c_read(uint8_t addr, uint8_t reg_read, uint32_t bytes_rcv, uint8_t* data_rcv) {
    i2c_err_t errcode = I2C_SUCCESS;
    if (i2cIsBusBusy(I2C)) {
        return I2C_ERR_START;
    }
    i2cSetStop(I2C);
    i2c_setup_transaction(addr, I2C_TRANSMITTER);
    I2C->MDR |= I2C_REPEATMODE;
    i2cSetStart(I2C);

    errcode = i2c_send(1, &reg_read);
    if (errcode != I2C_SUCCESS) {
        i2cSetStop(I2C);
        i2cClearSCD(I2C);
        if (errcode == I2C_ERR_NACK) {
            log_str(ERROR, I2C_LOG, true, "NACK from dev 0x%02x while attempting to read", addr);
            i2c_clear_nack();
        }
        return errcode;
    }
    errcode = i2c_complete_transaction();
    if (errcode != I2C_SUCCESS) {
        return errcode;
    }
 
    i2c_setup_transaction(addr, I2C_RECEIVER);
    i2cSetStart(I2C);
    i2c_receive(bytes_rcv - 1, data_rcv);
    /* TMS570 Manual, Section 28.3.2 Master Receiver Mode:
     * "Due to the double buffer implementation on the receive side, the master
     * must generate the stop condition (STP =1) after reading the (message size - 1)th data."
     * http://www.ti.com/lit/ug/spnu499c/spnu499c.pdf
     */
    i2cSetStop(I2C);
    i2c_receive(1, &data_rcv[bytes_rcv - 1]);
    errcode = i2c_complete_transaction();
    if (errcode != I2C_SUCCESS) {
        return errcode;
    }
    i2cClearSCD(I2C);
    i2c_clear_nack();
    return I2C_SUCCESS;
}

/**
 * i2c_write
 *
 * Writes specified number of bytes to I2C device
 *
 * addr: I2C device address
 * bytes_send: number of bytes to send to the device
 * data_send: buffer containing bytes to write
 *
 * returns: I2C module status
 */
i2c_err_t i2c_write(uint8_t addr, uint32_t bytes_send, const uint8_t* data_send) {
    if (i2cIsBusBusy(I2C)) {
        return I2C_ERR_START;
    }
    i2cSetStop(I2C);
    I2C->MDR &= ~(uint32_t)I2C_REPEATMODE;
    i2c_setup_transaction(addr, I2C_TRANSMITTER);
    i2cSetCount(I2C, bytes_send);
    i2cSetStart(I2C);
    i2c_err_t errcode = i2c_send(bytes_send, data_send);
    if (errcode != I2C_SUCCESS) {
        i2cSetStop(I2C);
        i2cClearSCD(I2C);
        if (errcode == I2C_ERR_NACK) {
            log_str(ERROR, I2C_LOG, true, "NACK from dev 0x%02x while attempting to write", addr);
            i2c_clear_nack();
        }
        return errcode;
    }
    errcode = i2c_complete_transaction();
    if (errcode != I2C_SUCCESS) {
        return errcode;
    }
    i2cSetStop(I2C);
    i2cClearSCD(I2C);
    return I2C_SUCCESS;
}

/**
 * i2c_ping_device
 *
 * Zero-transfer ping function to test whether a slave I2C device with
 * a particular address responds to the I2C module
 *
 *  addr: address of I2C slave device
 *
 *  returns: I2C module status
 */
i2c_err_t i2c_ping_device(uint8_t addr) {
    uint32_t timeout_count;
    if (i2cIsBusBusy(I2C)) {
        return I2C_ERR_START;
    }
    i2cSetStop(I2C);
    I2C->MDR |= (I2C_REPEATMODE);
    i2c_setup_transaction(addr, I2C_TRANSMITTER);
    i2cSetCount(I2C, 0);
    i2cSetStart(I2C);
    // Calling stop_detected_and_ready() immediately after i2cSetStart()
    // will cause thread to hang if addr is not connected to the bus
    // or unresponsive, use delay to give time for the i2c hardware
    // to determine failure to communicate
    vTaskDelay(1);
    timeout_count = 0;
    while ((!stop_detected_and_ready()) && (timeout_count < I2C_TIMEOUT_MAX)) {
        timeout_count++;
    }
    if (timeout_count >= I2C_TIMEOUT_MAX) {
        return I2C_ERR_PING_TIMEOUT;
    }
    if (i2c_is_nack()) {
        i2cSetStop(I2C);
        i2c_clear_nack();
        return I2C_ERR_NACK_ADDR;
    }
    i2c_complete_transaction();
    i2cSetStop(I2C);
    i2cClearSCD(I2C);
    return I2C_SUCCESS;
}

/* stop_detected_and_ready
 * 	- checks for receipt of stop condition and i2c flag indicating that reg accesses are safe
 * 	returns:
 * 		1: ARDY and SCD bits are 1
 * 			- ARDY: previously programmed bits have been updated, i2c regs may be safely accessed
 * 			- SDC: stop condition has been detected
 * 		0: otherwise
 * 			- in this case, it is not safe to continue with i2c transactions
 */
bool stop_detected_and_ready(void) {
    return ((I2C->STR) & (I2C_ARDY | I2C_SCD));
}

/* Reset I2C
 *
 * the bus can hang for many reasons. Our error handlers must therefore be able to reset the bus.
 * To do this, we reset the I2C peripheral and clock out 10 pulses
 * http://www.microchip.com/forums/m175368.aspx
 *
 * Precondition: we should have the mutex
 *
 * max_resets: Maximum number of resets allowed before error code is returned
 *
 * returns: I2C module status
 */
i2c_err_t i2c_reset(uint8_t max_resets) {
    uint8_t num_resets = 0;
    vTaskSuspendAll(); /* don't want any scheduling happening while we reset */
    do {
        I2C->MDR &= ~(uint32_t)I2C_RESET_OUT; /* reset i2c peripheral */
        I2C->PFNC = I2CPFNC_IO;               /* I2C pins to GPIO mode */
        I2C->DIR  = I2C_SCL;                  /* SDA is input [bit 1], SCL is output [bit 0], output = 1 */
        uint8_t i;

        /* send out some pulses */
        for (i = 0; i < 10; i++) {
            I2C->DOUT = I2C->DOUT | I2C_SCL; /* set SCL high */
            busy_wait(300);
            I2C->DOUT ^= I2C->DOUT; /* set SCL low */
            busy_wait(300);
        }
        num_resets++;

    } while (((I2C->DIN & I2C_SDA) != I2C_SDA) && (num_resets < max_resets)); /* check that the data line has gone high (idle) */
    xTaskResumeAll();
    if (num_resets >= max_resets) {
        log_str(ERROR, I2C_LOG, true, "I2C max resets hit");
        return I2C_ERR_RESET;
    }

    log_str(DEBUG, I2C_LOG, false, "I2C RESET");
    i2c_init();
    return I2C_SUCCESS;
}

/**
 * i2c_setup_transaction
 *
 * Sets I2C bus for a transaction, required before sending data
 *
 *  addr: address of I2C slave device
 *  dir: mode of I2C module: I2C_TRANSMITTER or I2C_RECEIVER
 *  num_bytes: number of bytes to transmit or receive
 */
void i2c_setup_transaction(uint8_t addr, uint32_t dir) {
    i2cSetSlaveAdd(I2C, addr);
    i2cSetDirection(I2C, dir);
    i2cSetMode(I2C, I2C_MASTER);
}

/**
 * i2c_complete_transaction
 *
 * Waits for I2C Master/Slave and bus busy bits to reset, required before initiating new transaction
 *
 * returns: I2C module status
 * 			- I2C_ERR_BB_CLEAR if bus busy bit failed to clear
 * 			- I2C_ERR_MST if Master Bit failed to clear
 * 			- I2C_SUCCESS otherwise
 */
i2c_err_t i2c_complete_transaction(void) {
    uint32_t timeout_count = 0;

    // not ready and busy
    while (is_busy_and_not_ready()) {
        timeout_count++;
        if (timeout_count > I2C_TIMEOUT_MAX) {
            if (i2cIsBusBusy(I2C)) {
                return I2C_ERR_BB_CLEAR;
            } else {
                return I2C_ERR_MST;
            }
        }
    }
    return I2C_SUCCESS;
}

/* is_busy_and_not_ready
 * 	returns:
 * 		1 if master is not ready OR bus is busy
 * 		0 if bus is free and master is ready
 */
bool is_busy_and_not_ready(void) {
    bool master_ready = i2cIsMasterReady(I2C);
    bool bus_busy     = i2cIsBusBusy(I2C);
    return ((!master_ready) || bus_busy);
}

/**
 * i2c_send
 *
 * Sends a specified number of bytes to a slave I2C device

 *  length: number of bytes for transmission
 *  data: pointer to an array containing data for transmission
 *
 *  returns: I2C module status
 *  		- I2C_TX_FAIL if TX bit was not set
 *  		- I2C_ERR_NACK if NACK was received
 *  		- I2C_SUCCESS otherwise
 */
i2c_err_t i2c_send(uint32_t length, const uint8_t* data) {
    uint32_t timeout_count = 0;
    while (length > 0U) {
        while (((I2C->STR & I2C_TX_INT) == 0U) && (timeout_count < I2C_TIMEOUT_MAX)) {
            timeout_count++;
        }
        if (timeout_count >= I2C_TIMEOUT_MAX) {
            return I2C_TX_FAIL;
        }

        if (I2C->STR & I2C_NACK_INT) {
            i2cSetStop(I2C);
            i2c_clear_nack();
            return I2C_ERR_NACK;
        }

        I2C->DXR = *data;
        data++;
        length--;
    }
    return I2C_SUCCESS;
}

/**
 * i2c_receive
 *
 * Received a specified number of bytes from a slave I2C device
 *
 * length: number of bytes for transmission
 * data: pointer to a byte array to store received data
 *
 *  returns: I2C module status
 *  		- I2C_RX_FAIL if RX bit was not set
 *  		- I2C_SUCCESS otherwise
 */
i2c_err_t i2c_receive(uint32_t length, uint8_t* data) {
    uint32_t timeout_count;
    timeout_count = 0;
    while (length > 0U) {
        while (((I2C->STR & I2C_RX_INT) == 0U) && (timeout_count < I2C_TIMEOUT_MAX)) {
            timeout_count++;
        }
        if (timeout_count >= I2C_TIMEOUT_MAX) {
            return I2C_RX_FAIL;
        }

        *data = I2C->DRR;
        data++;
        length--;
    }
    return I2C_SUCCESS;
}

/**
 * i2c_is_nack
 *
 * Checks is NACK was detected
 *
 *  returns: boolean value to indicate whether NACK was detected on I2C bus
 *  		- true if NACK was received
 *  		- false if no NACK was received
 */
bool i2c_is_nack(void) {
    if (I2C->STR & I2C_NACK_INT) {
        return true;
    } else {
        return false;
    }
}

/**
 * i2c_clear_nack
 *
 * Clears NACK bit
 */
void i2c_clear_nack(void) {
    I2C->STR |= I2C_NACK;
}

/**
 * i2c_print_str
 *
 * Prints state of STR (status) register
 */
void i2c_print_str(void) {
    uint32_t status = I2C->STR;
    log_str(DEBUG, I2C_LOG, false, "STR: x%04X", status);
}

/**
 * i2c_print_mdr
 *
 * Prints state of MDR (mode) register
 */
void i2c_print_mdr(void) {
    uint32_t status = I2C->MDR;
    log_str(DEBUG, I2C_LOG, false, "MDR: x%04X", status);
}

static void i2c_initialize_voltage_levels(void)
{

    /** Initialize I2C pins as GPIO in order to get correct voltage levels.
     *
     * 	Must initialize:
     * 	  - default values to high
     * 	  - direction to input/output
     * 	  - enable open drain
     * 	In order for I2C module to work correctly.
     *
     * 	Must then wait a 1 tick delay.
     */

    /** - i2c Enter reset */
    i2cREG1->MDR = (uint32_t)((uint32_t)0U << 5U);

    /** - set i2c pins functional mode */
    i2cREG1->PFNC = (1U);

    /** - set i2c pins default output value */
    i2cREG1->DOUT = (uint32_t)((uint32_t)1U << 1U)     /* sda pin */
                  | (uint32_t)(1U);     /* scl pin */

    /** - set i2c pins output direction */
    i2cREG1->DIR  = (uint32_t)((uint32_t)1U << 1U)     /* sda pin */
                  | (uint32_t)(1U);     /* scl pin */

    /** - set i2c pins open drain enable */
    i2cREG1->PDR  = (uint32_t)((uint32_t)1U << 1U)     /* sda pin */
                  | (uint32_t)(1U);     /* scl pin */

    /** - i2c Out of reset */
    i2cREG1->MDR |= (uint32_t)I2C_RESET_OUT;

    // Wait for a single tick. This is necessary, otherwise pin voltage doesn't settle
    vTaskDelay(pdMS_TO_TICKS(10));

}

