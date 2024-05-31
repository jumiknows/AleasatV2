/**
 * @file imu_bmx160.c
 * @brief Implementation for BMX160 IMU driver
 */


/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

//OBC
#include "imu_bmx160.h"
#include "tms_i2c.h"
#include "obc_gpio.h"
#include "logger.h"
#include "obc_hardwaredefs.h"
#include "obc_featuredefs.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define BMX160_POLL_TIMEOUT 100
#define DATA_READ_BUF_SIZE 20

/**
 * @brief Maximum time to wait to acquire the I2C mutex in milliseconds
 */
#define I2C_MUTEX_TIMEOUT_MS 500

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/
bmx160_t bmx160_imu_1 = {
    .gyro_settings = { 0 },
    .accel_settings = { 0 },
    .mag_settings = { 0 },

    .addr = IMU1_I2C_ADDR,
    .en_port = IMU_1_EN_PORT,
    .en_pin = IMU_1_EN_PIN,

    .is_active = FALSE,

    .gyro_cal = 1,
    .accel_cal = 1,
    .mag_cal = 1,

    .trimming_data = { 0 }
};

#if IMU_2_EN
bmx160_t bmx160_imu_2 = {
    .gyro_settings = { 0 },
    .accel_settings = { 0 },
    .mag_settings = { 0 },

    .addr = IMU2_I2C_ADDR,
    .en_port = IMU_2_EN_PORT,
    .en_pin = IMU_2_EN_PIN,

    .is_active = FALSE,

    .gyro_cal = 1,
    .accel_cal = 1,
    .mag_cal = 1,

    .trimming_data = { 0 }
};
#endif // IMU_2_EN

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static imu_error_t bmx160_init(bmx160_t *imu, uint8_t gyro_range, uint8_t accel_range, uint8_t gyro_odr, uint8_t gyro_bw,
                               uint8_t accel_odr, uint8_t accel_bw);
static imu_error_t bmx160_whoami(bmx160_t *imu, uint8_t *buff);
static float32 mag_comp_x(bmx160_t *imu, uint16_t rhall, int16_t raw);
static float32 mag_comp_y(bmx160_t *imu, uint16_t rhall, int16_t raw);
static float32 mag_comp_z(bmx160_t *imu, uint16_t rhall, int16_t raw);
static imu_error_t bmx160_init_mag(bmx160_t *imu);
static imu_error_t bmx160_read_trimming_data(bmx160_t *imu);
static imu_error_t bmx160_get_data_status(bmx160_t *imu);
static imu_error_t bmx160_read_reg(bmx160_t *imu, uint8_t reg, uint8_t num_bytes, uint8_t *buff);
static imu_error_t bmx160_write_reg(bmx160_t *imu, uint8_t reg, uint8_t num_bytes, uint8_t *buff);
static imu_error_t bmx160_get_mag_iface_status(bmx160_t *imu);


/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Preform software reset of BMX160 IMU module
 *
 * @param[in] bmx160_t* imu, imu struct to reset
*/
imu_error_t bmx160_soft_reset(bmx160_t *imu) {

    uint8_t data = BMX160_SOFT_RESET_CMD;

    if (bmx160_write_reg(imu, BMX160_COMMAND_REG_ADDR, 1, &data) == IMU_ERROR) {
        LOG_ADCS_IMU__SOFT_RESET_ERROR();
        return IMU_ERROR;
    }

    return IMU_SUCCESS;
}


/**
 * @brief Sets imu device power mode. Saves power mode in the IMU struct
 *
 * @param[in] bmx160_t* imu: imu to change power mode of
 * @param[in] uint8_t power_mode: power mode command to execute:
 *              BMX160_ACCEL_NORMAL_MODE, BMX160_ACCEL_LOWPOWER_MODE, BMX160_ACCEL_SUSPEND_MODE
 *              BMX160_GYRO_SUSPEND_MODE, BMX160_GYRO_NORMAL_MODE, BMX160_GYRO_FASTSTARTUP_MODE
 *              BMX160_MAGN_SUSPEND_MODE, BMX160_MAGN_NORMAL_MODE, BMX160_MAGN_LOWPOWER_MODE
 *
*/
imu_error_t bmx160_set_power_mode(bmx160_t *imu, uint8_t power_mode) {

    switch (power_mode) {
    case BMX160_MAGN_SUSPEND_MODE:
    case BMX160_MAGN_NORMAL_MODE:
    case BMX160_MAGN_LOWPOWER_MODE:
        imu-> mag_settings.power_state = power_mode;
        vTaskDelay(pdMS_TO_TICKS(20)); //see datasheet page 90 (table 29)
        break;

    case BMX160_ACCEL_NORMAL_MODE:
    case BMX160_ACCEL_LOWPOWER_MODE:
    case BMX160_ACCEL_SUSPEND_MODE:
        imu-> accel_settings.power_state = power_mode;
        vTaskDelay(pdMS_TO_TICKS(10)); //see datasheet page 90 (table 29)
        break;

    case BMX160_GYRO_SUSPEND_MODE:
    case BMX160_GYRO_NORMAL_MODE:
    case BMX160_GYRO_FASTSTARTUP_MODE:
        imu -> gyro_settings.power_state = power_mode;
        vTaskDelay(pdMS_TO_TICKS(100)); //see datasheet page 90 (table 29)
        break;

    default:
        LOG_ADCS_IMU__WRONG_DEVICE_MODE(power_mode);
        return IMU_ERROR;
    }

    imu_error_t result = bmx160_write_reg(imu, BMX160_COMMAND_REG_ADDR, 1, &power_mode);
    return result;
}

/**
 * @brief Sets BMX160 IMU to low power mode on all sensors
 *
 * @param[in] bmx160_t* imu: IMU to power down
*/
imu_error_t bmx160_set_low_power(bmx160_t *imu) {

    if (bmx160_soft_reset(imu) == IMU_ERROR) {
        LOG_ADCS_IMU__SOFT_RESET_POWERDOWN_ERROR();
        return IMU_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    if (bmx160_set_power_mode(imu, BMX160_ACCEL_LOWPOWER_MODE) == IMU_ERROR) {
        LOG_ADCS_IMU__SET_ACCEL_POWER_MODE_LOW_ERROR();
        return IMU_ERROR;
    }

    if (bmx160_set_power_mode(imu, BMX160_GYRO_FASTSTARTUP_MODE) == IMU_ERROR) {
        LOG_ADCS_IMU__SET_GYRO_POWER_MODE_LOW_ERROR();
        return IMU_ERROR;
    }

    if (bmx160_set_power_mode(imu, BMX160_MAGN_LOWPOWER_MODE) == IMU_ERROR) {
        LOG_ADCS_IMU__SET_MAGN_POWER_MODE_LOW_ERROR();
        return IMU_ERROR;
    }

    return IMU_SUCCESS;
}

/**
 * @brief Sets BMX160 IMU to normal power mode on all sensors
 *
 * @param[in] bmx160_t* imu: IMU to power up
 *
 * Returns IMU_SUCCESS if operation successful, IMU_ERROR otherwise
*/
imu_error_t bmx160_wake_up(bmx160_t *imu) {

    if (bmx160_soft_reset(imu) == IMU_ERROR) {
        LOG_ADCS_IMU__SOFT_RESET_WAKEUP_ERROR();
        return IMU_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    if (bmx160_init_mag(imu) == IMU_ERROR) {
        LOG_ADCS_IMU__IMU_INIT_WAKEUP_ERROR();
        return IMU_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    if (bmx160_set_power_mode(imu, BMX160_ACCEL_NORMAL_MODE) == IMU_ERROR) {
        LOG_ADCS_IMU__SET_ACCEL_POWER_MODE_NORMAL_ERROR();
        return IMU_ERROR;
    }

    if (bmx160_set_power_mode(imu, BMX160_GYRO_NORMAL_MODE) == IMU_ERROR) {
        LOG_ADCS_IMU__SET_GYRO_POWER_MODE_NORMAL_ERROR();
        return IMU_ERROR;
    }

    if (bmx160_set_power_mode(imu, BMX160_MAGN_NORMAL_MODE) == IMU_ERROR) {
        LOG_ADCS_IMU__SET_MAGN_POWER_MODE_NORMAL_ERROR();
        return IMU_ERROR;
    }

    return IMU_SUCCESS;
}

/**
 * @brief Enables power to specified IMU
 *
 * @param bmx160_t *imu: imu to enable power
 * Returns IMU_SUCCESS upon sucess
*/
imu_error_t bmx160_enable_imu(bmx160_t *imu) {
    obc_gpio_write(imu->en_port, imu->en_pin, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    if (bmx160_init(imu, BMX160_GYRO_RANGE_2000_DPS, BMX160_ACCEL_RANGE_2G, BMX160_GYRO_ODR_100HZ, BMX160_GYRO_BW_NORMAL_MODE,
                    BMX160_ACCEL_ODR_100HZ, BMX160_ACCEL_BW_NORMAL_AVG4) == IMU_ERROR) {
        return IMU_ERROR;
    }

    return IMU_SUCCESS;
}

/**
 * @brief Disabled power to specified IMU
 *
 * @param bmx160_t *imu: imu to disable power
 * Returns IMU_SUCCESS upon sucess
*/
imu_error_t bmx160_disable_imu(bmx160_t *imu) {
    if (bmx160_soft_reset(imu) == IMU_ERROR) {
        return IMU_ERROR;
    }

    obc_gpio_write(imu->en_port, imu->en_pin, 0);
    imu -> is_active = FALSE;
    return IMU_SUCCESS;
}

/**
 * @brief Sets new sensor output gyro data range
 *
 * @param bmx160_t *imu: imu to set gyro range
 * @param[in] uint8_t new_gyro_range: new range to set to (register value)
 *
 * Possible ranges:
 *      BMX160_GYRO_RANGE_2000_DPS
 *      BMX160_GYRO_RANGE_1000_DPS
 *      BMX160_GYRO_RANGE_500_DPS
 *      BMX160_GYRO_RANGE_250_DPS
 *      BMX160_GYRO_RANGE_125_DPS
 *
 *  Change is recorded for future referance by IMU
 *
*/
imu_error_t bmx160_set_gyro_range(bmx160_t *imu, uint8_t new_gyro_range) {

    if (bmx160_write_reg(imu, BMX160_GYRO_RANGE_ADDR, 1, &new_gyro_range) == IMU_ERROR) {
        LOG_ADCS_IMU__ERROR_WRITING_NEW_GYRO_RANGE(new_gyro_range);
        return IMU_ERROR;
    }

    switch (new_gyro_range) {
    case BMX160_GYRO_RANGE_2000_DPS:
        imu -> gyro_settings.range = BMX160_GYRO_SENSITIVITY_2000DPS;
        break;

    case BMX160_GYRO_RANGE_1000_DPS:
        imu -> gyro_settings.range = BMX160_GYRO_SENSITIVITY_1000DPS;
        break;

    case BMX160_GYRO_RANGE_500_DPS:
        imu -> gyro_settings.range = BMX160_GYRO_SENSITIVITY_500DPS;
        break;

    case BMX160_GYRO_RANGE_250_DPS:
        imu -> gyro_settings.range = BMX160_GYRO_SENSITIVITY_250DPS;
        break;

    default:
        imu -> gyro_settings.range = BMX160_GYRO_SENSITIVITY_125DPS;
        break;
    }

    return IMU_SUCCESS;
}

/**
 * @brief Sets new sensor output accelerometer data range
 *
 * @param[in] bmx160_t *imu: imu to set accelerometer range
 * @param[in] uint8_t new_accel_range: new range to set to (register value)
 *
 * Possible ranges:
 *      BMX160_ACCEL_RANGE_16G
 *      BMX160_ACCEL_RANGE_8G
 *      BMX160_ACCEL_RANGE_4G
 *      BMX160_ACCEL_RANGE_2G
 *
 * Change is recorded for future referance by IMU
 *
*/
imu_error_t bmx160_set_accel_range(bmx160_t *imu, uint8_t new_accel_range) {

    if (bmx160_write_reg(imu, BMX160_ACCEL_RANGE_ADDR, 1, &new_accel_range) == IMU_ERROR) {
        LOG_ADCS_IMU__ERROR_WRITING_NEW_ACCEL_RANGE(new_accel_range);
        return IMU_ERROR;
    }

    switch (new_accel_range) {
    case BMX160_ACCEL_RANGE_2G:
        imu -> accel_settings.range = BMX160_ACCEL_MG_LSB_2G;
        break;

    case BMX160_ACCEL_RANGE_4G:
        imu -> accel_settings.range = BMX160_ACCEL_MG_LSB_4G;
        break;

    case BMX160_ACCEL_RANGE_8G:
        imu -> accel_settings.range = BMX160_ACCEL_MG_LSB_8G;
        break;

    default:
        imu -> accel_settings.range = BMX160_ACCEL_MG_LSB_16G;
        break;
    }

    return IMU_SUCCESS;
}

/**
 * @brief Data collection function gathers data from imu, gyro, accelerometer registers on bmx160
 *
 * @param[in] bmx160_t* imu: imu object pointer where data is stored
 */
imu_error_t bmx160_get_data(bmx160_t *imu, bmx160_data_t *data_out) {
    uint8_t buff[DATA_READ_BUF_SIZE] = {0};

    if (bmx160_get_data_status(imu) == IMU_SUCCESS) {

        if (bmx160_read_reg(imu, BMX160_MAG_DATA_ADDR, DATA_READ_BUF_SIZE, buff) == IMU_ERROR) {
            return IMU_ERROR;
        }

        /*Capture raw data here*/
        data_out -> mag.x_raw = (int16_t)(((uint16_t)buff[0]) | (((uint16_t)buff[1]) << 8));
        data_out -> mag.y_raw = (int16_t)(((uint16_t)buff[2]) | (((uint16_t)buff[3]) << 8));
        data_out -> mag.z_raw = (int16_t)(((uint16_t)buff[4]) | (((uint16_t)buff[5]) << 8));
        data_out -> rhall     = (int16_t)(((uint16_t)buff[6]) | (((uint16_t)buff[7]) << 8));

        data_out -> gyro.x_raw = (int16_t)(((uint16_t)buff[8]) | (((uint16_t)buff[9]) << 8));
        data_out -> gyro.y_raw = (int16_t)(((uint16_t)buff[10]) | (((uint16_t)buff[11]) << 8));
        data_out -> gyro.z_raw = (int16_t)(((uint16_t)buff[12]) | (((uint16_t)buff[13]) << 8));

        data_out -> accel.x_raw = (int16_t)(((uint16_t)buff[14]) | (((uint16_t)buff[15]) << 8));
        data_out -> accel.y_raw = (int16_t)(((uint16_t)buff[16]) | (((uint16_t)buff[17]) << 8));
        data_out -> accel.z_raw = (int16_t)(((uint16_t)buff[18]) | (((uint16_t)buff[19]) << 8));

        /*Process data here*/
        data_out -> gyro.x_proc = (data_out->gyro.x_raw) * (imu->gyro_settings.range) * (imu->gyro_cal);
        data_out -> gyro.y_proc = (data_out->gyro.y_raw) * (imu->gyro_settings.range) * (imu->gyro_cal);
        data_out -> gyro.z_proc = (data_out->gyro.z_raw) * (imu->gyro_settings.range) * (imu->gyro_cal);

        data_out -> accel.x_proc = (data_out->accel.x_raw) * (imu->accel_settings.range) * (imu->accel_cal);
        data_out -> accel.y_proc = (data_out->accel.y_raw) * (imu->accel_settings.range) * (imu->accel_cal);
        data_out -> accel.z_proc = (data_out->accel.z_raw) * (imu->accel_settings.range) * (imu->accel_cal);

        data_out -> mag.x_proc = mag_comp_x(imu, data_out->rhall, data_out->mag.x_raw) * (imu->mag_cal);
        data_out -> mag.y_proc = mag_comp_y(imu, data_out->rhall, data_out->mag.y_raw) * (imu->mag_cal);
        data_out -> mag.z_proc = mag_comp_z(imu, data_out->rhall, data_out->mag.z_raw) * (imu->mag_cal);
    }

    return IMU_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Function for calculating calibrated magnetometer values based on:
 *          https://drive.google.com/drive/u/0/folders/12OMjm2C4exUvoXbodJH9_WS-AOeu9xN8
*/
static float32 mag_comp_x(bmx160_t *imu, uint16_t rhall, int16_t raw) {
    float32 result = 0;

    if (raw != (-4096)) {
        if (((rhall != 0) && (imu->trimming_data.dig_xyz1 != 0))) {
            result = (((float32)imu->trimming_data.dig_xyz1) * ((float32)16384.0 / (float32)rhall)) - (float32)16384.0;
        } else {
            return result;
        }

        result = ((((float32)raw * ((((((float32)imu->trimming_data.dig_xy2) *
                                       (result * (result / (float32)268435456.0))) + (result *
                                           (((float32)imu->trimming_data.dig_xy1) / (float32)16384.0))) + (float32)256.0) *
                                    (((float32)imu->trimming_data.dig_x2) + (float32)160.0))) / (float32)8192.0) +
                  (((float32)imu->trimming_data.dig_x1) * (float32)8.0)) / (float32)16.0;
    } else {
        return result;
    }

    return result;
}

/**
 * @brief Function for calculating calibrated magnetometer values based on:
 *          https://drive.google.com/drive/u/0/folders/12OMjm2C4exUvoXbodJH9_WS-AOeu9xN8
*/
static float32 mag_comp_y(bmx160_t *imu, uint16_t rhall, int16_t raw) {
    float32 result = 0;

    if (raw != (-4096)) {
        if (((rhall != 0) && (imu->trimming_data.dig_xyz1 != 0))) {
            result = (((float32)imu->trimming_data.dig_xyz1) * ((float32)16384.0 / rhall)) - (float32)16384.0;
        } else {
            return result;
        }

        result = (((raw * (((((((float32)imu->trimming_data.dig_xy2) *
                               ((result * (result / (float32)268435456.0)))) + (result *
                                   (((float32)imu->trimming_data.dig_xy1) / (float32)16384.0)))) + (float32)256.0) *
                           (((float32)imu->trimming_data.dig_y2) + (float32)160.0))) / (float32)8192.0) +
                  (((float32)imu->trimming_data.dig_y1) * (float32)8.0)) / (float32)16.0;
    } else {
        return result;
    }

    return result;
}

/**
 * @brief Function for calculating calibrated magnetometer values based on:
 *          https://drive.google.com/drive/u/0/folders/12OMjm2C4exUvoXbodJH9_WS-AOeu9xN8
*/
static float32 mag_comp_z(bmx160_t *imu, uint16_t rhall, int16_t raw) {
    float32 result = 0;

    if (
        (raw != (-16384)) &&
        (imu->trimming_data.dig_z2 != 0) &&
        (imu->trimming_data.dig_z1 != 0) &&
        (imu->trimming_data.dig_xyz1 != 0) &&
        (rhall != 0)
    ) {
        result = ((((((float32)raw) - ((float32)imu->trimming_data.dig_z4)) *
                    (float32)131072.0) - (((float32)imu->trimming_data.dig_z3) * (((float32)rhall)
                                          - ((float32)imu->trimming_data.dig_xyz1)))) /
                  ((((float32)imu->trimming_data.dig_z2) + (((float32)imu->trimming_data.dig_z1) *
                      (((float32)raw) / (float32)32768.0))) * (float32)4.0)) / (float32)16.0;
    }

    return result;
}

/*!
 * @brief Wrapper function for writing i2c registers from the bmx160 imu
 *
 * Function writes upwards from base register address to not reset status bits of data reads
 *
 * @param[in] uint8_t reg: register address to read from
 * @param[in] uint8_t num_bytes: number of bytes to read
 * @param[in] uint8_t *buff: pointer to data buffer to send
 *
 */
static imu_error_t bmx160_write_reg(bmx160_t *imu, uint8_t reg, uint8_t num_bytes, uint8_t *buff) {
    if ((!imu) || (!buff)) {
        return IMU_ERROR;
    }

    i2c_err_t status = tms_i2c_write(imu->addr, 1, &reg, num_bytes, buff, false, I2C_MUTEX_TIMEOUT_MS);

    if (status != I2C_SUCCESS) {
        LOG_ADCS_IMU__I2C_WRITE_FAILURE(status);
        return IMU_ERROR;
    }

    return IMU_SUCCESS;
}

/*!
 * @brief Wrapper function for reading i2c registers from the bmx160 imu
 *
 * Function reads upwards from base register address to not reset status bits of data reads
 *
 * @param[in] uint8_t reg: register address to read from
 * @param[in] uint8_t num_bytes: number of bytes to read
 * @param[out] uint8_t *buff: pointer to read data buffer
 */
static imu_error_t bmx160_read_reg(bmx160_t *imu, uint8_t reg, uint8_t num_bytes, uint8_t *buff) {
    if ((!imu) || (!buff)) {
        return IMU_ERROR;
    }

    i2c_err_t status = tms_i2c_read(imu->addr, 1, &reg, num_bytes, buff, false, I2C_MUTEX_TIMEOUT_MS);

    if (status != I2C_SUCCESS) {
        LOG_ADCS_IMU__I2C_READ_FAILURE(status);
        return IMU_ERROR;
    }

    return IMU_SUCCESS;
}

/**
 * @brief Function configures IMU as shown in https://github.com/DFRobot/DFRobot_BMX160/blob/master/DFRobot_BMX160.cpp
 *
 * @param[in] bmx160_t* imu, imu struct to initialize
*/
static imu_error_t bmx160_init_mag(bmx160_t *imu) {

    uint8_t i;
    /**
     * Following array is based on the magnetometer setup from dfRobot's bmx160 driver.
     * This command sequence can be found in meathod DFRobot_BMX160::setMagnConf() in DFRobot_BMX160.cpp.
     *
     * A similar example with more explanation is avaliable in device datasheet in Table 16 on page 25.
    */
    uint8_t cmd_array[12][2] = {{0x80, BMX160_MAGN_IF_0_ADDR}, {0x01, BMX160_MAGN_IF_3_ADDR},
        {0x4B, BMX160_MAGN_IF_2_ADDR}, {0x04, BMX160_MAGN_IF_3_ADDR},
        {0x51, BMX160_MAGN_IF_2_ADDR}, {0x0E, BMX160_MAGN_IF_3_ADDR},
        {0x52, BMX160_MAGN_IF_2_ADDR}, {0x02, BMX160_MAGN_IF_3_ADDR},
        {0x4C, BMX160_MAGN_IF_2_ADDR}, {0x42, BMX160_MAGN_IF_1_ADDR},
        {0x08, BMX160_MAGN_CONFIG_ADDR}, {0x03, BMX160_MAGN_IF_0_ADDR}
    };

    /*Put magnetometer into setup mode*/
    if (bmx160_write_reg(imu, cmd_array[0][1], 1, &cmd_array[0][0]) == IMU_ERROR) {
        return IMU_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    for (i = 1; i < 12; i++) {
        if ((bmx160_write_reg(imu, cmd_array[i][1], 1, &cmd_array[i][0])) == IMU_ERROR) {
            LOG_ADCS_IMU__ERROR_DURING_INIT_MAG(i);
            return IMU_ERROR;
        }
    }

    if (bmx160_set_power_mode(imu, BMX160_MAGN_NORMAL_MODE) == IMU_ERROR) {
        return IMU_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(50));
    return IMU_SUCCESS;

}

/**
 * @brief Function reads mag_man_op flag
 *
 * @param bmx160_t* imu: imu to read status bits of
 *
 * Function returns either IMU_SUCCESS if mag_man_op is 0, otherwise returns IMU_ERROR

*/
static imu_error_t bmx160_get_mag_iface_status(bmx160_t *imu) {
    uint8_t buff = 0;

    if (bmx160_read_reg(imu, BMX160_STATUS_ADDR, 1, &buff) == IMU_ERROR) {
        LOG_ADCS_IMU__ERROR_READING_MAG_MAN_OP();
        return IMU_ERROR;
    }

    if ((buff & (0b00000100)) == 0) {
        return IMU_SUCCESS;
    }

    return IMU_ERROR;
}

/**
 * @brief Function reads the magnetometer trimming data located in the magnetometer auxilliary interface
 *
 * @param: bmx160_t* imu, imu struct to initialize
*/
static imu_error_t bmx160_read_trimming_data(bmx160_t *imu) {

    uint8_t i;
    uint8_t j;
    /**
     * The following array contains the command sequence necessary to put the magnetometer into setup mode and bring it out again.
     * This is necessary in order to read the trimming data values out of it's NV memory.
     *
     * Documentation for magnetometer setup mode can be found in device datasheet on Page 23, section 2.4.3.1
    */
    uint8_t cmd_array[6][2] = {{0x80, BMX160_MAGN_IF_0_ADDR}, {0x02, BMX160_MAGN_IF_3_ADDR},
        {0x4C, BMX160_MAGN_IF_2_ADDR}, {0x42, BMX160_MAGN_IF_1_ADDR},
        {0x08, BMX160_MAGN_CONFIG_ADDR}, {0x03, BMX160_MAGN_IF_0_ADDR}
    };

    /**
     * This array contains addresses in magnetometer NV memory which hold device trimming data programmed at the factory.
     * The documentation for this is limited to a single PDF file posted by some Bosch engineer in response to a forum post...
     *
     * See https://drive.google.com/drive/u/0/folders/12OMjm2C4exUvoXbodJH9_WS-AOeu9xN8 on the aleasat drive for more (vague) info on where this comes from.
    */
    uint8_t mag_read_array[] = {0x5D, 0x5E, 0x64, 0x65, 0x71, 0x70, 0x6A, 0x6B, 0x68, 0x68, 0x6E, 0x6F, 0x62, 0x63, 0x6C, 0x6D};
    uint8_t write_addr = BMX160_MAGN_IF_1_ADDR;
    uint8_t read_addr = BMX160_MAG_DATA_ADDR;


    /*Put magnetometer into setup mode*/
    if (bmx160_write_reg(imu, cmd_array[0][1], 1, &cmd_array[0][0]) == IMU_ERROR) {
        return IMU_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    /*Collect trimming data if device is being powered up*/
    if ((imu->is_active) == FALSE) {

        for (i = 0; i < 16; i++) {

            if (bmx160_write_reg(imu, write_addr, 1, &mag_read_array[i]) == IMU_ERROR) {
                return IMU_ERROR;
            }

            vTaskDelay(pdMS_TO_TICKS(100));

            for (j = 0; j <= BMX160_POLL_TIMEOUT; j++) {
                if (bmx160_get_mag_iface_status(imu) == IMU_SUCCESS) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                    break;
                }
            }

            if (bmx160_read_reg(imu, read_addr, 1, &mag_read_array[i]) == IMU_ERROR) {
                return IMU_ERROR;
            }

            vTaskDelay(pdMS_TO_TICKS(10));
        }

        imu->trimming_data.dig_x1 = (int8_t)mag_read_array[0];
        imu->trimming_data.dig_y1 = (int8_t)mag_read_array[1];
        imu->trimming_data.dig_x2 = (int8_t)mag_read_array[2];
        imu->trimming_data.dig_y2 = (int8_t)mag_read_array[3];
        imu->trimming_data.dig_xy1 = (uint8_t)mag_read_array[4];
        imu->trimming_data.dig_xy2 = (int8_t)mag_read_array[5];

        imu->trimming_data.dig_z1 = (uint16_t)((mag_read_array[7] << 8) | mag_read_array[6]);
        imu->trimming_data.dig_z2 = (int16_t)((mag_read_array[9] << 8) | mag_read_array[8]);
        imu->trimming_data.dig_z3 = (int16_t)((mag_read_array[11] << 8) | mag_read_array[10]);
        imu->trimming_data.dig_z4 = (int16_t)((mag_read_array[13] << 8) | mag_read_array[12]);
        imu->trimming_data.dig_xyz1 = (uint16_t)((mag_read_array[15] << 8) | mag_read_array[14]);
    }

    vTaskDelay(pdMS_TO_TICKS(50));

    for (i = 1; i < 6; i++) {
        if ((bmx160_write_reg(imu, cmd_array[i][1], 1, &cmd_array[i][0])) == IMU_ERROR) {
            LOG_ADCS_IMU__ERROR_INIT_MAG_IN_READ_TRIMMING(i);
            return IMU_ERROR;
        }
    }

    if (bmx160_set_power_mode(imu, BMX160_MAGN_NORMAL_MODE) == IMU_ERROR) {
        return IMU_ERROR;
    }

    vTaskDelay(pdMS_TO_TICKS(50));
    return IMU_SUCCESS;

}

/**
 * @brief Reads the CHIPID register of specified IMU, return IMU_SUCCESS upon completion and writes value to buffer
 *
 * @param[in] bmx160_t* imu: Imu to read ID
 *
*/
static imu_error_t bmx160_whoami(bmx160_t *imu, uint8_t *buff) {

    if (bmx160_read_reg(imu, BMX160_CHIP_ID_ADDR, 1, buff) == IMU_ERROR) {
        LOG_ADCS_IMU__ERROR_READING_CHIPID();
        return IMU_ERROR;
    }

    return IMU_SUCCESS;
}

/**
 * @brief Data status read function for imu, gyro, accelerometer
 *
 * @param[in] bmx160_t *imu: imu to read status bits of
 *
 * Function returns either IMU_SUCCESS if all status bits are data ready. Otherwise IMU_ERROR is returned
 *
 * TODO: Potentially add induvidual sensor feedback instead of global sucess or failure (i.e. gyro_ready, accel_ready etc.)
*/

static imu_error_t bmx160_get_data_status(bmx160_t *imu) {
    uint8_t buff = 0;

    if (bmx160_read_reg(imu, BMX160_STATUS_ADDR, 1, &buff) == IMU_ERROR) {
        LOG_ADCS_IMU__ERROR_READING_DATA_READY();
        return IMU_ERROR;
    } else if ((buff & 0b11100000) != (0b11100000)) {
        LOG_ADCS_IMU__ERROR_DATA_READY(buff);
        return IMU_ERROR;
    } else {
        return IMU_SUCCESS;
    }
}


/**
 * @brief Initializes IMU for initial use
 *
 * @param[in] bmx160_t* imu: IMU to initialize
 * @param[in] uint8_t gyro_range: Range for gyro to be initialized to
 * @param[in] uint8_t accel_range: Range for accelerometer to be initialized to
 * @param[in] uint8_t gyro_odr: Output data rate for gyro
 * @param[in] uint8_t gyro_bw: Gyroscope bandwidth
 * @param[in] uint8_t accel_odr: Output data rate for accelerometer
 * @param[in] uint8_t accel_bw: accelerometer bandwidth
 * @param[in] uint8_t mag_odr: Output data rate for magnetometer
 * @param[in] uint8_t mag_bw: Bandwidth for magnetometer
 * Steps:
 *      1: Set all devices out of suspend mode to normal mode
 *      2: Confirm PMU status
 *      3: Read whoami
 *      4: Set sensor range, bandwidth and odr
*/
static imu_error_t bmx160_init(bmx160_t *imu, uint8_t gyro_range, uint8_t accel_range, uint8_t gyro_odr, uint8_t gyro_bw,
                               uint8_t accel_odr, uint8_t accel_bw) {

    uint8_t buff;
    imu_error_t result = IMU_SUCCESS;

    do {

        /*Do soft reset*/
        if (bmx160_soft_reset(imu) == IMU_ERROR) {
            result = IMU_ERROR;
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));

        /*Set accel power to normal mode */
        if (bmx160_set_power_mode(imu, BMX160_ACCEL_NORMAL_MODE) == IMU_ERROR) {
            result = IMU_ERROR;
            break;
        }

        /*Set gyro power to normal mode */
        if (bmx160_set_power_mode(imu, BMX160_GYRO_NORMAL_MODE) == IMU_ERROR) {
            result = IMU_ERROR;
            break;
        }

        /*Set mag power to normal mode */
        if (bmx160_set_power_mode(imu, BMX160_MAGN_NORMAL_MODE) == IMU_ERROR) {
            result = IMU_ERROR;
            break;
        }

        /* Need to fully initialize mag BEFORE reading NV for some reason, thx Bosch*/
        vTaskDelay(pdMS_TO_TICKS(500));

        if (bmx160_init_mag(imu) == IMU_ERROR) {
            result = IMU_ERROR;
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(500));

        if (bmx160_read_trimming_data(imu) == IMU_ERROR) {
            result = IMU_ERROR;
            break;
        }

        /* Now we need to do it again... */
        vTaskDelay(pdMS_TO_TICKS(500));

        if (bmx160_init_mag(imu) == IMU_ERROR) {
            result = IMU_ERROR;
            break;
        }

        vTaskDelay((500));

        if (bmx160_read_reg(imu, BMX160_PMU_STATUS, 1, &buff) == IMU_ERROR) {
            LOG_ADCS_IMU__ERROR_READING_PMU_STATUS();
            result = IMU_ERROR; //get power status
            break;
        }

        if ((buff) != 21) {
            LOG_ADCS_IMU__PMU_STATUS_WRONG(buff);
            result = IMU_ERROR; //Power status is incorrect (something went wrong)
            //21 = 00010101 -> Expected status of PMU_STATUS register
            break;
        }

        if (bmx160_whoami(imu, &buff) == IMU_ERROR) {
            result = IMU_ERROR;
            break;
        }

        if ((buff) != 216) {
            LOG_ADCS_IMU__WRONG_CHIPID(buff);
            result = IMU_ERROR;
            break;
        }

        /*Set gyro data range*/
        if (bmx160_set_gyro_range(imu, gyro_range) == IMU_ERROR) {
            result = IMU_ERROR;
            break;
        }

        /*Set accel data range*/
        if (bmx160_set_accel_range(imu, accel_range) == IMU_ERROR) {
            result = IMU_ERROR;
            break;
        }

        uint8_t gyro_conf = gyro_odr | (uint8_t)(gyro_bw << 4);
        uint8_t accel_conf = accel_odr | (uint8_t)(accel_bw << 4);

        /*write gyro bandwidth and output data rate*/
        if (bmx160_write_reg(imu, BMX160_GYRO_CONFIG_ADDR, 1, &gyro_conf) == IMU_ERROR) {
            LOG_ADCS_IMU__ERROR_WRITING_GYRO_CONFIG();
            result = IMU_ERROR;
            break;
        }

        /*write accel bandwidth and output data rate*/
        if (bmx160_write_reg(imu, BMX160_ACCEL_CONFIG_ADDR, 1, &accel_conf) == IMU_ERROR) {
            LOG_ADCS_IMU__ERROR_WRITING_ACCEL_CONFIG();
            result = IMU_ERROR;
            break;
        }

        imu -> gyro_settings.odr = gyro_odr;
        imu -> gyro_settings.bw = gyro_bw;
        imu -> accel_settings.odr = accel_odr;
        imu -> accel_settings.bw = accel_bw;
    } while (0);

    if (result == IMU_SUCCESS) {
        imu -> is_active = TRUE;
    } else {
        imu -> is_active = FALSE;
    }

    return result;
}
