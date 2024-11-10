/**
 * @file temperature sensor stts75 driver
 * @brief Contains methods to get temperature from temp sensor, and set different modes
 * @author Melvin & Lana (edited by Julie to shift towards I2C freeRTOS)
 * @date Feb 4, 2019 (last edited Oct 10, 2019)
 */

#include "temp_stts75.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"
#include "i2c_freertos.h"
#include "obc_i2c.h"
#include "logger.h"
#include "obc_task_info.h"
#include "obc_hardwaredefs.h"

#define TEMP_SENSOR_TIMEOUT_MS 500

static temp_err_t stts75_write_reg(uint8_t reg, uint8_t* data, uint8_t len);
static temp_err_t stts75_issue_i2c_submission(const i2c_command_t* command);
static int16_t convert_temp_bytes_to_c(uint8_t* temp_bytes);
static uint8_t* convert_temp_c_to_bytes(int16_t temp_deg_c);

/**
 * @brief Pings STT75 and checks for response.
 * CAUTION!! Function does not use FreeRTOS i2c, so use is considered less reliable
 *
 * @return I2C_SUCCESS if STSS75 replied, error otherwise (error comes from obc_i2c)
 */

i2c_err_t stts75_ping(void) {
    return i2c_ping_device(OBC_TEMP_ADDR);
}

/**
 * @brief Reads temperature from the temp sensor
 *
 * @param [out] val: pointer to store temperature in Celsius
 * @return if operation successful, appropriate error otherwise
 */
temp_err_t stts75_temp_get(int16_t* val) {
    uint8_t data[2] = {'\0'};
    temp_err_t ret  = stts75_read_reg(STTS75_TEMPERATURE, data, NUM_BYTES_TEMP);
    if (ret != TEMP_SUCCESS) {
        return ret;
    }

    //
    *val = convert_temp_bytes_to_c(data);
    return ret;
}

/**
 * @brief Get the mode in which the sensor currently in (one of: continuous conversion, shutdown,
 * and oneshot)
 *
 * @param [out] val: pointer in which the mode is stored
 * @return TEMP_SUCCESS if mode was successfully read, error code otherwise
 */

temp_err_t stts75_mode_get(stts75_mode_t* val) {
    uint8_t config_byte = 0;
    temp_err_t ret      = stts75_read_config_reg(&config_byte);
    if (ret != TEMP_SUCCESS) {
        return ret;
    }
    uint8_t one_shot_val = (config_byte & STTS75_OSM) >> STTS75_OFFSET_OSM;
    uint8_t shutdown_val = (config_byte & STTS75_SD) >> STTS75_OFFSET_SD;
    if (shutdown_val == 0) {
        *val = continuous_conversion;
    } else if (one_shot_val == 0) {
        *val = shutdown;
    } else {
        *val = one_shot;
    }
    return TEMP_SUCCESS;
}

/**
 * @brief Sets mode of the sensor
 *
 * @param The desired mode (one of continuous conversion, shutdown, or one_shot)
 * @param [out]
 * @return TEMP_SUCCESS if mode was set successfully, error code otherwise
 */

temp_err_t stts75_mode_set(stts75_mode_t val) {
    uint8_t config_byte = 0;
    temp_err_t ret      = stts75_read_config_reg(&config_byte);
    if (ret == TEMP_SUCCESS) {
        bool one_shot_bit = 0;
        bool shutdown_bit = 0;
        switch (val) {
            case continuous_conversion:
                one_shot_bit = 0;
                shutdown_bit = 0;
                break;
            case shutdown:
                one_shot_bit = 0;
                shutdown_bit = 1;
                break;
            case one_shot:
                one_shot_bit = 1;
                shutdown_bit = 0;
                break;
            default:
                one_shot_bit = 0;
                shutdown_bit = 0;
                break;
        }
        config_byte                 = config_byte & (uint8_t)(~(STTS75_SD | STTS75_OSM));
        const uint8_t config_byte_r = config_byte | (uint8_t)(one_shot_bit << STTS75_OFFSET_OSM) | (uint8_t)(shutdown_bit << STTS75_OFFSET_SD);
        ret                         = stts75_write_config_reg(config_byte_r);
    }
    return ret;
}

/**
 * @brief Get the variable resolution of the sensor (which is stored in a 1 byte configuration
 * register). 9bit, 10bit, 11bit, and 12bit resolutions available
 *
 * @param [out] val: pointer to store current resolution
 * @return I2C module status: TEMP_SUCCESS if mode was set successfully, error code otherwise
 */

temp_err_t stts75_resolution_get(stts75_resolution_t* val) {
    uint8_t config_byte = 0;
    temp_err_t ret      = stts75_read_config_reg(&config_byte);
    if (ret != TEMP_SUCCESS) {
        return ret;
    }
    uint8_t resolution = (config_byte & STTS75_RC) >> STTS75_OFFSET_RC;
    *val               = (stts75_resolution_t)resolution;
    return TEMP_SUCCESS;
}

/**
 * @brief Set the variable resolution of the sensor by manipulating the configuration register of
 * the sensor (1 byte) 9bit, 10bit, 11bit, and 12bit resolutions available
 *
 * @param val: desired resolution
 * @return I2C module status: TEMP_SUCCESS if mode was set successfully, error code otherwise
 */

temp_err_t stts75_resolution_set(stts75_resolution_t val) {
    uint8_t config_byte = 0;
    temp_err_t ret      = stts75_read_config_reg(&config_byte);
    if (ret == TEMP_SUCCESS) {
        const uint8_t config_byte_w = (config_byte & (uint8_t)(~STTS75_RC)) | (uint8_t)(val << STTS75_OFFSET_RC);
        ret                         = stts75_write_config_reg(config_byte_w);
    }
    return ret;
}

/**
 * @brief Get the number of faults allowed before activating the OS/INT pin by manipulating the
 * configuration register of the sensor (1 byte) 1, 2, 4, and 6 faults available
 *
 * @param
 * @param [out] val: pointer to store current fault value
 * @return I2C module status: TEMP_SUCCESS if mode was read successfully, error code otherwise
 */

temp_err_t stts75_fault_get(stts75_fault_t* val) {
    uint8_t config_byte = 0;
    temp_err_t ret      = stts75_read_config_reg(&config_byte);
    if (ret != TEMP_SUCCESS) {
        return ret;
    }
    uint8_t fault = (config_byte & STTS75_FT) >> STTS75_OFFSET_FT;
    *val          = (stts75_fault_t)fault;
    return TEMP_SUCCESS;
}

/**
 * @brief  Set the number of faults allowed before activating the OS/INT pin by manipulating the
 * configuration register of the sensor (1 byte) 1, 2, 4, and 6 faults available
 *
 * @param val: desired fault number
 * @return  I2C module status: TEMP_SUCCESS if mode was set successfully, error code otherwise
 */

temp_err_t stts75_fault_set(stts75_fault_t val) {
    uint8_t config_byte = 0;
    temp_err_t ret      = stts75_read_config_reg(&config_byte);
    if (ret == TEMP_SUCCESS) {
        const uint8_t config_byte_w = (config_byte & (uint8_t)(~STTS75_FT)) | (uint8_t)(val << STTS75_OFFSET_FT);
        ret                         = stts75_write_config_reg(config_byte_w);
    }
    return ret;
}

/**
 * @brief  Get the hysterisis temperature stored in the hysterisis register of the sensor (2 bytes)
 * This temperature acts as the minimum temperature when triggering faults
 *
 * @param [out] val: pointer to store current hysteresis temperature in Celsius
 * @return I2C module status: TEMP_SUCCESS if hysteresis temperature was read successfully, error
 * code otherwise
 */

temp_err_t stts75_hysteresis_get(int16_t* val) {
    uint8_t hyst_bytes[NUM_BYTES_TEMP] = {0x00, 0x00};
    temp_err_t ret                     = stts75_read_reg(STTS75_HYSTERESIS, hyst_bytes, NUM_BYTES_TEMP);
    if (ret != TEMP_SUCCESS) {
        return ret;
    }
    *val = convert_temp_bytes_to_c(hyst_bytes);
    return ret;
}

/**
 * @brief Set the hysteresis temperature stored in the hysteresis register of the sensor (2 bytes)
 * This temperature acts as the minimum temperature when triggering faults
 *
 * @param val: desired hysteresis temperature in Celsius
 * @return I2C module status: TEMP_SUCCESS if hysteresis temperature was set successfully, error
 * code otherwise
 */

temp_err_t stts75_hysteresis_set(int16_t hyst_deg_c) {
    uint8_t* hyst_bytes = convert_temp_c_to_bytes(hyst_deg_c);
    temp_err_t ret      = stts75_write_reg(STTS75_HYSTERESIS, hyst_bytes, NUM_BYTES_TEMP);
    return ret;
}

/* stts75_overtemp_get
 *
 * Get the overlimit temperature stored in the overlimit register of the sensor (2 bytes)
 * This temperature acts as the maximum temperature when triggering faults
 *
 * val: pointer to store current overlimit temperature in Celsius
 *
 * returns: I2C module status: TEMP_SUCCESS if hysteresis temperature was set successfully, error
 * code otherwise
 */
/**
 * @brief Get the overlimit temperature stored in the overlimit register of the sensor (2 bytes)
 * This temperature acts as the maximum temperature when triggering faults
 *
 * @param [out] val: pointer to store current overlimit temperature in Celsius
 * @return  I2C module status: TEMP_SUCCESS if hysteresis temperature was set successfully, error
 * code otherwise
 */

temp_err_t stts75_overtemp_get(int16_t* overtemp_deg_c) {
    uint8_t data[NUM_BYTES_TEMP] = {0x00, 0x00};
    temp_err_t ret               = stts75_read_reg(STTS75_OVERTEMP, data, NUM_BYTES_TEMP);
    if (ret != TEMP_SUCCESS) {
        return ret;
    }
    *overtemp_deg_c = convert_temp_bytes_to_c(data);
    return ret;
}

/**
 * @brief Set overlimit temperature stored in the overlimit register of the sensor (2 bytes)
 * This temperature acts as the maximum temperature when triggering faults
 *
 * @param val: desired overlimit temperature in Celsius
 * @param [out]
 * @return I2C module status: TEMP_SUCCESS if hysteresis temperature was set successfully, error
 * code otherwise
 */

temp_err_t stts75_overtemp_set(int16_t overtemp_deg_c) {
    uint8_t* overtemp_bytes = convert_temp_c_to_bytes(overtemp_deg_c);
    temp_err_t ret          = stts75_write_reg(STTS75_OVERTEMP, overtemp_bytes, NUM_BYTES_TEMP);
    return ret;
}

/**
 * @brief Read a series of bytes from the sensor via I2C bus
 *
 * @param reg: register to read data from
 * 		  len: number of bytes to read
 * @param [out] data_rcv: pointer to store read bytes
 * @return I2C module status: TEMP_SUCCESS if hysteresis temperature was set successfully, error
 * code otherwise
 */

temp_err_t stts75_read_reg(uint8_t reg, uint8_t* data_rcv, uint8_t len) {
    i2c_command_t command = {reg, TEMP_SENSOR_RECEIVE_QUEUE_ID, OBC_TEMP_ADDR, READ_DATA, len, data_rcv};
    return stts75_issue_i2c_submission(&command);
}

/**
 * @brief Write a series of bytes to the sensor via I2C bus
 *
 * @param reg: register to write data to
 * 		  data: bytes of data to write
 * 		  len: number of bytes to write
 * @return I2C module status: TEMP_SUCCESS if hysteresis temperature was set successfully, error
 * code otherwise
 */

static temp_err_t stts75_write_reg(uint8_t reg, uint8_t* data, uint8_t len) {
    i2c_command_t command = {reg, TEMP_SENSOR_RECEIVE_QUEUE_ID, OBC_TEMP_ADDR, WRITE_DATA, len, data};
    return stts75_issue_i2c_submission(&command);
}

/*
 * Issue I2C Submission for temp sensor (modelled after gpio_expander method)
 */
/**
 * @brief Issue an I2C submission for the temp sensor
 *
 * @param command: the command to issue to the I2C task
 * @return TEMP_SUCCESS if successful, error code otherwise
 */

static temp_err_t stts75_issue_i2c_submission(const i2c_command_t* command) {
    result_t result = I2C_SUCCESS;

    send_command_to_i2c_worker(command, I2C_WORKER_DEFAULT_PRIORITY);
    if (xQueueReceive(temp_sensor_rx_q, (void*)&result, pdMS_TO_TICKS(TEMP_SENSOR_TIMEOUT_MS)) == pdFALSE) {
        log_str(ERROR, TEMP_LOG, true, "I2C queue rx timeout");
        return TEMP_I2C_ERR;
    }
    if (result != I2C_SUCCESS) {
        log_str(ERROR, TEMP_LOG, true, "I2C failure: %d", result);
        return TEMP_I2C_ERR;
    }
    return TEMP_SUCCESS;
}

/**
 * @brief Read one byte from config register on temperature sensor
 *
 * @param
 * @param [out] data_rcv: pointer to store config byte
 * @return  I2C module status: TEMP_SUCCESS if hysteresis temperature was set successfully, error
 * code otherwise
 */
temp_err_t stts75_read_config_reg(uint8_t* data_rcv) {
    return stts75_read_reg(STTS75_CONFIGURATION, data_rcv, NUM_BYTES_CONFIG);
}

/**
 * @brief Write one byte to config register on temperature sensor
 *
 * @param data_send: config value to write
 * @return I2C module status: TEMP_SUCCESS if hysteresis temperature was set successfully, error
 * code otherwise
 */

temp_err_t stts75_write_config_reg(uint8_t reg_value) {
    return stts75_write_reg(STTS75_CONFIGURATION, &reg_value, NUM_BYTES_CONFIG);
}

/**
 * @brief Converts 2 bytes of temperature delta into Celsius
 *
 * @param temp_bytes: array containing temperature bytes
 * @return temperature in Celsius
 */

static int16_t convert_temp_bytes_to_c(uint8_t* temp_bytes) {
    int16_t temp_deg_c = (int16_t)((int16_t)temp_bytes[0] << 8) + temp_bytes[1];
    return (temp_deg_c + 0x80) >> 8; // add 128 for rounding, divide by 256 to get num to *C
}

/**
 * @brief Converts temperature in Celsius to 2-byte digital representation
 *
 * @param temp_deg_c: temperature in Celsius
 * @return array containing temperature bytes
 */

static uint8_t* convert_temp_c_to_bytes(int16_t temp_deg_c) {
    static uint8_t temp_bytes[NUM_BYTES_TEMP];
    temp_deg_c    = temp_deg_c * 256;
    temp_bytes[0] = temp_deg_c >> 8;
    temp_bytes[1] = temp_deg_c;
    return temp_bytes;
}
