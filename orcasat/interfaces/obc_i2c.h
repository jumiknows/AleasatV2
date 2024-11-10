/*
 * obc_i2c.h
 *
 *  Created on: Apr 22, 2018
 *      Author: Richard & Lana
 *
 *      - Wrap these functions with a mutex wherever you call them
 *      - Main goal of the obc_i2c adaptations from halcogen I2C functions is to ensure that we
 * never get stuck in an infinite loop
 *      - DO NOT use the halcogen functions that do anything more than set or read a register.
 *      	- Complicated halcogen functions will wait forever for things to happen and lock up the
 * system.
 *
 *      ERROR HANDLING
 *      - calling functions are responsible for dealing with errors.
 *      - errors should be logged in our system, and retries may optionally be attempted
 *      - retry attempts must be limited
 *
 */

#ifndef OBC_I2C_H_
#define OBC_I2C_H_

#include "i2c.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"
#include "obc_error.h"

extern SemaphoreHandle_t xI2CMutex;

// I2C COMMANDS
#define RD_CMD 0x1
#define WR_CMD 0x0

// I2C error handling
#define I2C_TIMEOUT_MAX \
    200000 // number of attempts before timing out (units are attempts at register check), typically \
           // takes ~3500 for a correct wait

#define I2CPFNC_I2C 0x0
#define I2CPFNC_IO  0x1

/** @enum i2cRegs
 *   @brief Fields for the following registers: I2CDIN, I2CDOUT, I2CPDIR
 */
enum i2cRegBit {
    I2C_SDA = 0x0002, // Serial data
    I2C_SCL = 0x0001  // Serial clock data
};

void i2c_init(void);
i2c_err_t i2c_read(uint8_t addr, uint8_t reg_read, uint32_t bytes_rcv, uint8_t* data_rcv);
i2c_err_t i2c_write(uint8_t addr, uint32_t bytes_send, const uint8_t* data_send);
i2c_err_t i2c_reset(uint8_t max_resets);
i2c_err_t i2c_ping_device(uint8_t addr);

#endif /* OBC_I2C_H_ */
