/*
 * @file obc_eps.c
 * @brief Interface to communicate with EnduroSAT EPS via I2C
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_eps.h"

// OBC
#include "tms_i2c.h"

// HAL
#include "i2c.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define EPS_I2C_TIMEOUT_MS  1000U

#define READ_CMD_DATA_LEN   2U

/*
 * Coefficients for converting raw data to floats
 *
 * From EnduroSAT EPS I user manual rev4.1: https://drive.google.com/file/d/1d5Aa7zZRk3Kt3gV23Raj6aAEnOZ6MK1U/view?usp=sharing
 */
#define EPS_BATTERY_VOLTAGE_COEFF   0.0023394775f
#define EPS_BATTERY_CURRENT_COEFF   0.0030517578f
#define EPS_BCR_CURRENT_COEFF       0.0015258789f
#define EPS_SOLAR_VOLTAGE_COEFF     0.0024414063f
#define EPS_SOLAR_CURRENT_COEFF     0.0006103516f
#define EPS_BUS_CURRENT_COEFF       0.0020345052f
#define EPS_RBATT_RAW_COEFF         1.4972656f
#define EPS_VBATT_IDEAL_COEFF       2.3394775f

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static i2c_err_t eps_read(uint8_t cmd, uint16_t *raw_data);
static i2c_err_t eps_write(uint8_t cmd, uint8_t state);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Read a float-valued register from the EPS module
 *
 * @param cmd: the register to read
 * @param data: pointer to store the read data to
 * @return EPS_SUCCESS if no error, error code otherwise
 */
eps_err_t eps_read_float(eps_cmd_read_float_t cmd, float32 *data) {
    eps_err_t err = EPS_SUCCESS;
    float32 coeff = 0.0f;
    uint16_t raw_data = 0;
    bool is_temperature = false; // Whether the cmd is a temperature cmd and requires special handling

    // Determine conversion coefficient based on cmd
    switch (cmd) {
    case EPS_READ_BATTERY_VOLTAGE:
        coeff = EPS_BATTERY_VOLTAGE_COEFF;
        break;

    case EPS_READ_BATTERY_CURRENT:
        coeff = EPS_BATTERY_CURRENT_COEFF;
        break;

    case EPS_READ_BCR_VOLTAGE:
        coeff = EPS_BATTERY_VOLTAGE_COEFF;
        break;

    case EPS_READ_BCR_CURRENT:
        coeff = EPS_BCR_CURRENT_COEFF;
        break;

    case EPS_READ_X_VOLTAGE:
        coeff = EPS_SOLAR_VOLTAGE_COEFF;
        break;

    case EPS_READ_XM_CURRENT:
    case EPS_READ_XP_CURRENT:
        coeff = EPS_SOLAR_CURRENT_COEFF;
        break;

    case EPS_READ_Y_VOLTAGE:
        coeff = EPS_SOLAR_VOLTAGE_COEFF;
        break;

    case EPS_READ_YM_CURRENT:
    case EPS_READ_YP_CURRENT:
        coeff = EPS_SOLAR_CURRENT_COEFF;
        break;

    case EPS_READ_Z_VOLTAGE:
        coeff = EPS_SOLAR_VOLTAGE_COEFF;
        break;

    case EPS_READ_ZM_CURRENT:
    case EPS_READ_ZP_CURRENT:
        coeff = EPS_SOLAR_CURRENT_COEFF;
        break;

    case EPS_READ_BUS_3V3_CURRENT:
    case EPS_READ_BUS_5V_CURRENT:
        coeff = EPS_BUS_CURRENT_COEFF;
        break;

    case EPS_READ_LUP_3V3:
    case EPS_READ_LUP_5V:
        coeff = EPS_BATTERY_VOLTAGE_COEFF;
        break;

    case EPS_READ_MCU_TEMP:
    case EPS_READ_BATTERY_TEMP_C1:
    case EPS_READ_BATTERY_TEMP_C2:
    case EPS_READ_BATTERY_TEMP_C3:
    case EPS_READ_BATTERY_TEMP_C4:
    case EPS_READ_MAX_TEMP_1:
    case EPS_READ_MAX_TEMP_2:
    case EPS_READ_MAX_TEMP_3:
    case EPS_READ_MAX_TEMP_4:
    case EPS_READ_MIN_TEMP_1:
    case EPS_READ_MIN_TEMP_2:
    case EPS_READ_MIN_TEMP_3:
    case EPS_READ_MIN_TEMP_4:
    case EPS_READ_TEMP_SENSOR_5:
    case EPS_READ_TEMP_SENSOR_6:
    case EPS_READ_TEMP_SENSOR_7:
        /*
         * This command requires a formula for conversion: we will
         *   handle this as a special case later in this function
         */
        is_temperature = true;
        break;

    case EPS_READ_RBAT_INITIAL:
    case EPS_READ_RBAT_RAW:
        coeff = EPS_RBATT_RAW_COEFF;
        break;

    case EPS_READ_VBAT_IDEAL:
        coeff = EPS_VBATT_IDEAL_COEFF;
        break;

    case EPS_READ_BUS_3V3_VOLTAGE:
    case EPS_READ_BUS_5V_VOLTAGE:
        coeff = EPS_BATTERY_VOLTAGE_COEFF;
        break;

    default:
        err = EPS_INVALID_COMMAND; // Error: command is invalid
        break;
    }

    if (err != EPS_SUCCESS) {
        return err;
    }

    // Read the raw data from the EPS via I2C
    i2c_err_t read_err = eps_read((uint8_t)cmd, &raw_data);

    if (read_err != I2C_SUCCESS) {
        return EPS_I2C_ERROR;
    }

    // Return the final, converted value
    if (is_temperature) {
        if (cmd == EPS_READ_MCU_TEMP) {
            // MCU temperature is handled as its own case
            *data = (float32)((raw_data * 0.0006103516f) - 0.986f) / 0.00355f;
        } else {
            // Every other temperature cmd must account for +/- cases
            //   This logic was taken from the EnduroSAT user manual
            if (raw_data < 0x8000) {
                // Temperature is positive
                *data = (float32) raw_data * 0.00390625f;
            } else {
                // Temperature is negative
                *data = (float32)((((raw_data >> 4) - 1) ^ 0xFFFF) & 0x0FFF) * (-0.0624f);
            }
        }
    } else {
        // Every other command only requires a coefficient
        *data = (float32) raw_data * coeff;
    }

    return err;
}

/**
 * @brief Read the raw data of a float-valued register from the EPS module
 *
 * @param cmd: the register to read
 * @param data: pointer to store the read data to
 * @return EPS_SUCCESS if no error, error code otherwise
 */
eps_err_t eps_read_float_raw(eps_cmd_read_float_t cmd, uint16_t *data) {
    eps_err_t err = EPS_SUCCESS;
    uint16_t raw_data = 0;

    // Read the raw data from the EPS via I2C
    i2c_err_t read_err = eps_read((uint8_t)cmd, &raw_data);

    if (read_err != I2C_SUCCESS) {
        return EPS_I2C_ERROR;
    }

    // Return the final, converted value
    *data = raw_data;
    return err;
}

/**
 * @brief Read a int-valued register from the EPS module
 *
 * @param cmd: the register to read
 * @param data: pointer to store the read data to
 * @return EPS_SUCCESS if no error, error code otherwise
 */
eps_err_t eps_read_int(eps_cmd_read_int_t cmd, uint16_t *data) {
    eps_err_t err = EPS_SUCCESS;
    uint16_t raw_data = 0;

    // Read the raw data from the EPS via I2C
    i2c_err_t read_err = eps_read((uint8_t)cmd, &raw_data);

    if (read_err != I2C_SUCCESS) {
        return EPS_I2C_ERROR;
    }

    // Return the final, converted value
    *data = raw_data;
    return err;
}

/**
 * @brief Write to a state register on the EPS
 *
 * @param cmd: the register to write
 * @param state: new state of register
 * @return EPS_SUCCESS if no error, error code otherwise
 */
eps_err_t eps_write_state(eps_cmd_write_state_t cmd, eps_write_state_t state) {
    eps_err_t err = EPS_SUCCESS;

    // Write register
    i2c_err_t write_err = eps_write((uint8_t) cmd, (uint8_t) state);

    if (write_err != I2C_SUCCESS) {
        err = EPS_I2C_ERROR;
    }

    return err;
}

/**
 * @brief Write the raw initial battery resistance value to the EPS
 *
 * To calculate the raw Rbat value, multiply the resistance in ohms
 *  by 667.884172247694, and conver to the nearest uint8 representation.
 *  For example, Rbat raw = 0.03 ohms x 667.884172247694 = 20.
 *
 * This value is used for calculations performed by the EPS. For more
 *  information on how to use this function, please refer to the EPS
 *  user manual, pg 48.
 *
 * @param cmd: the register to write
 * @param state: new state of register
 * @return EPS_SUCCESS if no error, error code otherwise
 */
eps_err_t eps_write_initial_resistance(uint8_t rbat_raw) {
    // Write the raw value to the EPS
    i2c_err_t err = eps_write(EPS_WRITE_CMD_RBAT_RAW_INITIAL, rbat_raw);

    if (err != I2C_SUCCESS) {
        return EPS_I2C_ERROR;
    }

    return EPS_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/*
 * @brief Performs a read request
 *
 * @param[in] cmd         Read command to be sent to the EPS module
 * @param[out] ret_data   Pointer to data buffer where read data will be written
 */
static i2c_err_t eps_read(uint8_t cmd, uint16_t *raw_data) {
    i2c_err_t err;

    uint8_t read_data[READ_CMD_DATA_LEN] = { 0 };
    err = tms_i2c_read(EPS_ADDR, 1U, &cmd, 2U, read_data, false, EPS_I2C_TIMEOUT_MS);

    if (err != I2C_SUCCESS) {
        return err;
    }

    *raw_data = (uint16_t)((uint16_t) read_data[0] << 8) | read_data[1];  // First byte is MSByte, second is LSByte
    return I2C_SUCCESS;
}

/*
 * @brief Performs a write request
 *
 * @param[in] cmd    Write command to be sent to the EPS module. Determines register to write to.
 * @param[in] state  EPS state to be written
 */
static i2c_err_t eps_write(uint8_t cmd, uint8_t state) {
    return tms_i2c_write(EPS_ADDR, 1U, &cmd, 1U, &state, false, EPS_I2C_TIMEOUT_MS);
}
