/**
 * @file cam_ov5642.c
 * @brief Driver for OV5642 image sensor
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cam_ov5642.h"
#include "cam_ov5642_defs.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

// TMS570
#include "tms_i2c.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Fixed I2C address of the OV5642 sensor
 * 
 * The SCCB (0x78) is given as an 8-bit address so must be converted to a 7-bit
 * I2C address.
 */
#define I2C_ADDR             (0x78U >> 1U)

/**
 * @brief Timeout to acquire the I2C mutex
 */
#define I2C_MUTEX_TIMEOUT_MS 500U

/**
 * @brief Wait time period after resetting the OV5642 sensor
 */
#define STARTUP_DELAY_MS     100U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static ov5642_err_t configure_jpeg(void);
static ov5642_err_t read_sensor_chip_id(uint16_t *chip_id);

static i2c_err_t write_sensor_regs(const ov5642_reg_data_t *reg_data, uint32_t reg_data_len);

static i2c_err_t write_sensor_reg(uint16_t addr, uint8_t data);
static i2c_err_t read_sensor_reg(uint16_t addr, uint8_t *data);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Reset and initialize the OV5642 sensor
 * 
 * @return Status code:
 *            - OV5642_SUCCESS if the initialization is successful
 *            - OV5642_ERR_I2C if an I2C error occurs
 *            - OV5642_ERR_CHIPID if the CHIP ID does not match the expected value
 */
ov5642_err_t ov5642_init(void) {
    ov5642_err_t err = OV5642_SUCCESS;

    do {
        // Reset the sensor
        err = ov5642_soft_reset();
        if (err != OV5642_SUCCESS) {
            break;
        }

        // Wait for reset to finish
        vTaskDelay(pdMS_TO_TICKS(STARTUP_DELAY_MS));

        // Check chip id to confirm camera sensor is responsive
        uint16_t chip_id = 0;
        err = read_sensor_chip_id(&chip_id);
        if (err != OV5642_SUCCESS) {
            break;
        } else {
            if (chip_id != OV5642_CHIPID) {
                err = OV5642_ERR_CHIPID;
                break;
            }
        }
    } while (0);

    return err;
}

/**
 * @brief Reset the OV5642 sensor through software
 * 
 * @return Status code:
 *            - OV5642_SUCCESS if the reset is successful
 *            - OV5642_ERR_I2C if an I2C error occurs
 */
ov5642_err_t ov5642_soft_reset(void) {
    ov5642_err_t err = OV5642_SUCCESS;

    i2c_err_t i2c_err = write_sensor_reg(OV5642_REG_SC_SYS_0, OV5642_REG_SC_SYS_0_SOFT_RESET);
    if (i2c_err != I2C_SUCCESS) {
        err = OV5642_ERR_I2C;
    }

    return err;
}

/**
 * @brief Configure the image format used by the OV5642 sensor
 * 
 * @param[in] format Image format to configure
 * 
 * @return Status code:
 *            - OV5642_SUCCESS if the configuration is successful
 *            - OV5642_ERR_I2C if an I2C error occurs
 *            - OV5642_ERR_INVALID_ARGS if an invalid format is provided
 */
ov5642_err_t ov5642_set_format(ov5642_format_t format) {
    ov5642_err_t err = OV5642_SUCCESS;

    switch (format) {
        case OV5642_FMT_JPEG:
            err = configure_jpeg();
            break;
        // TODO RAW
        default:
            err = OV5642_ERR_INVALID_ARGS;
            break;
    }

    return err;
}

/**
 * @brief Configure the JPEG image resolution used by the OV5642 sensor
 * 
 * @param[in] jpeg_size JPEG size (resolution) to configure
 * 
 * @return Status code:
 *            - OV5642_SUCCESS if the configuration is successful
 *            - OV5642_ERR_I2C if an I2C error occurs
 *            - OV5642_ERR_INVALID_ARGS if an invalid jpeg_size is provided
 */
ov5642_err_t ov5642_set_jpeg_size(ov5642_jpeg_size_t jpeg_size) {
    ov5642_err_t err = OV5642_SUCCESS;

    i2c_err_t i2c_err;

    switch (jpeg_size) {
        case OV5642_JPEG_SIZE_320X240:
            i2c_err = write_sensor_regs(ov5642_reg_data_320x240, LEN(ov5642_reg_data_320x240));
            if (i2c_err != I2C_SUCCESS) {
                err = OV5642_ERR_I2C;
            }
            break;
        case OV5642_JPEG_SIZE_2592X1944:
            i2c_err = write_sensor_regs(ov5642_reg_data_2592x1944, LEN(ov5642_reg_data_2592x1944));
            if (i2c_err != I2C_SUCCESS) {
                err = OV5642_ERR_I2C;
            }
            break;
        // TODO other sizes
        default:
            err = OV5642_ERR_INVALID_ARGS;
            break;
    }

    return err;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Configure the OV5642 sensor to capture a JPEG image
 * 
 * @return Status code:
 *            - OV5642_SUCCESS if the configuration is successful
 *            - OV5642_ERR_I2C if an I2C error occurs
 */
static ov5642_err_t configure_jpeg(void) {
    ov5642_err_t err = OV5642_SUCCESS;

    do {
        i2c_err_t i2c_err = write_sensor_regs(ov5642_reg_data_qvga_preview, LEN(ov5642_reg_data_qvga_preview));
        if (i2c_err != I2C_SUCCESS) {
            err = OV5642_ERR_I2C;
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(200));

        i2c_err = write_sensor_regs(ov5642_reg_data_jpeg_capture_qsxga, LEN(ov5642_reg_data_jpeg_capture_qsxga));
        if (i2c_err != I2C_SUCCESS) {
            err = OV5642_ERR_I2C;
            break;
        }

        err = ov5642_set_jpeg_size(OV5642_JPEG_SIZE_320X240);
        if (err != OV5642_SUCCESS) {
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));

        static const ov5642_reg_data_t final_regs[6] = {
            {OV5642_REG_TIMING_CONTROL_18,   0xa8},
            {OV5642_REG_ARRAY_CONTROL_01,    0x10},
            {OV5642_REG_TIMING_HS_1,         0xb0},
            {OV5642_REG_COMPRESSION_CTRL_07, 0x08},
            {OV5642_REG_LENC_BRV_SCALE_HIGH, 0x00},
            {OV5642_REG_ISP_CONTROL_00,      0xFF},
        };
        i2c_err = write_sensor_regs(final_regs, LEN(final_regs));
        if (i2c_err != I2C_SUCCESS) {
            err = OV5642_ERR_I2C;
            break;
        }
    } while (0);

    return err;
}

/**
 * @brief Read the 16-bit CHIP ID from the OV5642 sensor
 * 
 * @param[out] chip_id Pointer to where the CHIP ID will be stored
 * 
 * @return Status code:
 *            - OV5642_SUCCESS if the read is successful
 *            - OV5642_ERR_I2C if an I2C error occurs
 */
static ov5642_err_t read_sensor_chip_id(uint16_t *chip_id) {
    ov5642_err_t err = OV5642_SUCCESS;

    do {
        i2c_err_t i2c_err;
        uint8_t rx = 0;

        *chip_id = 0;

        i2c_err = read_sensor_reg(OV5642_REG_CHIP_ID_HIGH_BYTE, &rx);
        if (i2c_err != I2C_SUCCESS) {
            err = OV5642_ERR_I2C;
            break;
        }
        *chip_id |= (uint16_t)((uint16_t)rx << 8U);

        i2c_err = read_sensor_reg(OV5642_REG_CHIP_ID_LOW_BYTE, &rx);
        if (i2c_err != I2C_SUCCESS) {
            err = OV5642_ERR_I2C;
            break;
        }
        *chip_id |= rx;
    } while (0);

    return err;
}

/**
 * @brief Write multiple register values to the OV5642 in sequence.
 * 
 * If any one register write fails, the sequence is aborted.
 * 
 * @param[in] reg_data     Array of register addresses and values to write. Cannot be NULL.
 * @param[in] reg_data_len Number of entries in the reg_data array
 * 
 * @return Result code from the first failing I2C operation or I2C_SUCCESS if all operations succeed
 */
static i2c_err_t write_sensor_regs(const ov5642_reg_data_t *reg_data, uint32_t reg_data_len) {
    i2c_err_t err = I2C_SUCCESS;
    for (uint32_t i = 0; i < reg_data_len; i++) {
        err = write_sensor_reg(reg_data[i].addr, reg_data[i].data);
        if (err != I2C_SUCCESS) {
            break;
        }
    }
    return err;
}

/**
 * @brief Write a single register value to the OV5642 over I2C
 * 
 * @param[in]  addr Register address
 * @param[out] data Value to write
 * 
 * @return Result code from the I2C operation
 */
static i2c_err_t write_sensor_reg(uint16_t addr, uint8_t data) {
    uint8_t addr_bytes[2] = {
        (addr >> 8U) & 0xFF,
        (addr >> 0U) & 0xFF,
    };

    i2c_err_t status = tms_i2c_write(I2C_ADDR, sizeof(addr_bytes), addr_bytes, 1, &data, true, I2C_MUTEX_TIMEOUT_MS);
    return status;
}

/**
 * @brief Read a single register value from the OV5642 over I2C
 * 
 * @param[in]  addr Register address
 * @param[out] data Pointer to where the received register value will be stored. Cannot be NULL.
 * 
 * @return Result code from the I2C operation
 */
static i2c_err_t read_sensor_reg(uint16_t addr, uint8_t *data) {
    uint8_t addr_bytes[2] = {
        (addr >> 8U) & 0xFF,
        (addr >> 0U) & 0xFF,
    };

    i2c_err_t status = tms_i2c_read(I2C_ADDR, sizeof(addr_bytes), addr_bytes, 1, data, true, I2C_MUTEX_TIMEOUT_MS);
    return status;
}
