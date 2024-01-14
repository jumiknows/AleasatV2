/**
 * @file cam_arduchip.c
 * @brief Driver for the ArduChip (the processor on ArduCAM modules)
 *
 * Reference Documentation: https://www.arducam.com/docs/spi-cameras-for-arduino/hardware/arducam-chip/
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cam_arduchip.h"
#include "cam_arduchip_defs.h"

// OBC
#include "obc_hardwaredefs.h"

// FreeRTOS
#include "rtos.h"

// TMS570
#include "tms_mibspi.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Timeout for each MIBSPI transaction to complete
 */
#define MIBSPI_TIMEOUT_MS 30U

/**
 * @brief Wait time period after resetting the ArduChip
 */
#define STARTUP_DELAY_MS  100U

/**
 * @brief Value written/read when using the TEST register to verify communication
 * with the ArduChip.
 */
#define TEST_REG_VALUE    0x55

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief MIBSPI transfer group definition for single byte mode
 */
static const mibspi_tg_t transfer_group_single = {
    .reg = CAMERA_MIBSPI_REG,
    .transfer_group = CAMERA_TG_SINGLE,
    .cs_port = CAMERA_CS_PORT,
    .cs_pin = CAMERA_CS_PIN
};

/**
 * @brief MIBSPI transfer group definition for burst mode
 */
static const mibspi_tg_t transfer_group_burst = {
    .reg = CAMERA_MIBSPI_REG,
    .transfer_group = CAMERA_TG_BURST,
    .cs_port = CAMERA_CS_PORT,
    .cs_pin = CAMERA_CS_PIN
};

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static arduchip_err_t read_ardu_fw_ver(uint8_t *fw_ver);
static arduchip_err_t test_reg(uint8_t test_val);

static mibspi_err_t read_ardu_burst(uint8_t addr, uint8_t *data);

static mibspi_err_t write_ardu_reg(uint8_t addr, uint8_t data);
static mibspi_err_t read_ardu_reg(uint8_t addr, uint8_t *data);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Reset and initialize the ArduChip
 *
 * @return Status code:
 *            - ARDUCHIP_SUCCESS if the full initialization procedure is successful
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 *            - ARDUCHIP_ERR_FW_VER if the ArduChip firmware version does not match the expected value
 *            - ARDUCHIP_ERR_TEST_FAIL if the operation of the TEST register fails
 */
arduchip_err_t arduchip_init(void) {
    arduchip_err_t err = ARDUCHIP_SUCCESS;

    // SPI setup
    mibspiEnableGroupNotification(transfer_group_single.reg, transfer_group_single.transfer_group, CAMERA_RX_INT_LEVEL);
    mibspiEnableGroupNotification(transfer_group_burst.reg, transfer_group_burst.transfer_group, CAMERA_RX_INT_LEVEL);

    do {
        // Reset ArduChip
        err = arduchip_reset();

        if (err != ARDUCHIP_SUCCESS) {
            break;
        }

        // Wait for initialization
        vTaskDelay(pdMS_TO_TICKS(STARTUP_DELAY_MS));

        // Check firmware version to confirm ArduChip is responsive
        uint8_t fw_ver = 0;
        err = read_ardu_fw_ver(&fw_ver);

        if (err != ARDUCHIP_SUCCESS) {
            break;
        } else {
            if ((fw_ver != ARDUCHIP_FW_VER_E) && (fw_ver != ARDUCHIP_FW_VER_F)) {
                err = ARDUCHIP_ERR_FW_VER;
                break;
            }
        }

        // Write/read from test register
        err = test_reg(TEST_REG_VALUE);

        if (err != ARDUCHIP_SUCCESS) {
            break;
        }
    } while (0);

    return err;
}

/**
 * @brief Reset the ArduChip through software
 *
 * @return Status code:
 *            - ARDUCHIP_SUCCESS if the reset is successful
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 */
arduchip_err_t arduchip_reset(void) {
    arduchip_err_t err = ARDUCHIP_SUCCESS;

    do {
        mibspi_err_t mibspi_err;

        mibspi_err = write_ardu_reg(ARDUCHIP_REG_RESET, ARDUCHIP_REG_RESET_RESET);

        if (mibspi_err != MIBSPI_NO_ERR) {
            err = ARDUCHIP_ERR_MIBSPI;
        }

        // Wait for reset to finish
        vTaskDelay(pdMS_TO_TICKS(STARTUP_DELAY_MS));

        // Clear reset register
        mibspi_err = write_ardu_reg(ARDUCHIP_REG_RESET, 0);

        if (mibspi_err != MIBSPI_NO_ERR) {
            err = ARDUCHIP_ERR_MIBSPI;
            break;
        }
    } while (0);

    return err;
}

/**
 * @brief Configure the sensor timing behaviour expected by the ArduChip
 *
 * @param[in] hsync_active_low true if the HSYNC signal is active-low
 * @param[in] vsync_active_low true if the VSYNC signal is active-low
 * @param[in] pclk_reverse     true if the PCLK is reversed
 *
 * @return Status code:
 *            - ARDUCHIP_SUCCESS if the configuration is successful
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 */
arduchip_err_t arduchip_configure_sensor_timing(bool hsync_active_low, bool vsync_active_low, bool pclk_reverse) {
    arduchip_err_t err = ARDUCHIP_SUCCESS;

    uint8_t sensor_timing = 0x0;

    if (hsync_active_low) {
        sensor_timing |= ARDUCHIP_REG_SENSOR_TIMING_HSYNC_ACTIVE_LOW;
    }

    if (vsync_active_low) {
        sensor_timing |= ARDUCHIP_REG_SENSOR_TIMING_VSYNC_ACTIVE_LOW;
    }

    if (pclk_reverse) {
        sensor_timing |= ARDUCHIP_REG_SENSOR_TIMING_PCLK_REVERSE;
    }

    mibspi_err_t mibspi_err = write_ardu_reg(ARDUCHIP_REG_SENSOR_TIMING, sensor_timing);

    if (mibspi_err != MIBSPI_NO_ERR) {
        err = ARDUCHIP_ERR_MIBSPI;
    }

    return err;
}

/**
 * @brief Set the number of frames to be captured
 *
 * @param[in] num_frames Number of frames to capture (>= 1)
 *
 * @return Status code:
 *            - ARDUCHIP_SUCCESS if the configuration is successful
 *            - ARDUCHIP_ERR_INVALID_ARGS if an invalid number of frames is specified
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 */
arduchip_err_t arduchip_set_num_frames(uint8_t num_frames) {
    if (num_frames == 0) {
        return ARDUCHIP_ERR_INVALID_ARGS;
    }

    arduchip_err_t err = ARDUCHIP_SUCCESS;

    mibspi_err_t mibspi_err = write_ardu_reg(ARDUCHIP_REG_CAPTURE_CTRL, (num_frames - 1)); // Actual number of frames is register value + 1

    if (mibspi_err != MIBSPI_NO_ERR) {
        err = ARDUCHIP_ERR_MIBSPI;
    }

    return err;
}

/**
 * @brief Trigger an image capture.
 *
 * @return Status code:
 *            - ARDUCHIP_SUCCESS if the capture is successfully started
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 */
arduchip_err_t arduchip_fifo_start_capture(void) {
    arduchip_err_t err = ARDUCHIP_SUCCESS;

    mibspi_err_t mibspi_err = write_ardu_reg(ARDUCHIP_REG_FIFO_CTRL, ARDUCHIP_REG_FIFO_CTRL_START_CAPTURE);

    if (mibspi_err != MIBSPI_NO_ERR) {
        err = ARDUCHIP_ERR_MIBSPI;
    }

    return err;
}

/**
 * @brief Check if an image capture is complete
 *
 * @param[out] done Pointer to where the done status will be stored
 *
 * @return Status code:
 *            - ARDUCHIP_SUCCESS if the check is successful (regardless of if the capture is complete or not)
 *            - ARDUCHIP_ERR_INVALID_ARGS if a null pointer is passed
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 */
arduchip_err_t arduchip_is_capture_done(bool *done) {
    if (done == NULL) {
        return ARDUCHIP_ERR_INVALID_ARGS;
    }

    arduchip_err_t err = ARDUCHIP_SUCCESS;

    uint8_t status = 0;
    mibspi_err_t mibspi_err = read_ardu_reg(ARDUCHIP_REG_STATUS, &status);

    if (mibspi_err != MIBSPI_NO_ERR) {
        err = ARDUCHIP_ERR_MIBSPI;
    } else {
        if ((status & ARDUCHIP_REG_STATUS_WR_FIFO_DONE) != 0) {
            *done = true;
        } else {
            *done = false;
        }
    }

    return err;
}

/**
 * @brief Retrieve the length of the image data FIFO after an image capture
 *
 * @param[out] fifo_len Pointer to where the FIFO length will be stored
 *
 * @return Status code:
 *            - ARDUCHIP_SUCCESS if the length is successfully retrieved
 *            - ARDUCHIP_ERR_INVALID_ARGS if a null pointer is passed
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 */
arduchip_err_t arduchip_read_fifo_len(uint32_t *fifo_len) {
    if (fifo_len == NULL) {
        return ARDUCHIP_ERR_INVALID_ARGS;
    }

    arduchip_err_t err = ARDUCHIP_SUCCESS;

    do {
        mibspi_err_t mibspi_err;
        uint8_t fifo_len_byte = 0;

        *fifo_len = 0;

        // Read fifo_len[7:0]
        mibspi_err = read_ardu_reg(ARDUCHIP_REG_FIFO_SIZE_0, &fifo_len_byte);

        if (mibspi_err != MIBSPI_NO_ERR) {
            err = ARDUCHIP_ERR_MIBSPI;
            break;
        }

        *fifo_len |= ((uint32_t)fifo_len_byte << 0);

        // Read fifo_len[15:8]
        mibspi_err = read_ardu_reg(ARDUCHIP_REG_FIFO_SIZE_1, &fifo_len_byte);

        if (mibspi_err != MIBSPI_NO_ERR) {
            err = ARDUCHIP_ERR_MIBSPI;
            break;
        }

        *fifo_len |= ((uint32_t)fifo_len_byte << 8);

        // Read fifo_len[23:16]
        mibspi_err = read_ardu_reg(ARDUCHIP_REG_FIFO_SIZE_2, &fifo_len_byte);

        if (mibspi_err != MIBSPI_NO_ERR) {
            err = ARDUCHIP_ERR_MIBSPI;
            break;
        }

        *fifo_len |= ((uint32_t)fifo_len_byte << 16);
    } while (0);

    return err;
}

/**
 * @brief Read data from the image data FIFO in a burst.
 *
 * The size of the burst read is given by ARDUCHIP_BURST_READ_SIZE.
 *
 * @param[out] data Buffer to store read data. Must have a size at least as big as ARDUCHIP_BURST_READ_SIZE.
 *                  If NULL is passed, the data will still be read but discarded immediately.
 *
 * @return Status code:
 *            - ARDUCHIP_SUCCESS if the read is successful
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 */
arduchip_err_t arduchip_read_fifo_burst(uint8_t *data) {
    arduchip_err_t err = ARDUCHIP_SUCCESS;

    mibspi_err_t mibspi_err = read_ardu_burst(ARDUCHIP_REG_BURST_FIFO_RD, data);

    if (mibspi_err != MIBSPI_NO_ERR) {
        err = ARDUCHIP_ERR_MIBSPI;
    }

    return err;
}

/**
 * @brief Clear the FIFO write done flag.
 *
 * Call this after transferring an image to allow the next image to be captured
 *
 * @return Status code:
 *            - ARDUCHIP_SUCCESS if the flag is successfully cleared
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 */
arduchip_err_t arduchip_fifo_clear_wr_done(void) {
    arduchip_err_t err = ARDUCHIP_SUCCESS;

    mibspi_err_t mibspi_err = write_ardu_reg(ARDUCHIP_REG_FIFO_CTRL, ARDUCHIP_REG_FIFO_CTRL_CLEAR_WR_DONE);

    if (mibspi_err != MIBSPI_NO_ERR) {
        err = ARDUCHIP_ERR_MIBSPI;
    }

    return err;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Read the firmware version from the ArduChip
 *
 * @param[out] fw_ver Pointer to where the firmware version will be stored. Cannot be NULL.
 *
 * @return Status code:
 *            - ARDUCHIP_SUCCESS if the read is successful
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 */
static arduchip_err_t read_ardu_fw_ver(uint8_t *fw_ver) {
    arduchip_err_t err = ARDUCHIP_SUCCESS;

    mibspi_err_t mibspi_err;

    mibspi_err = read_ardu_reg(ARDUCHIP_REG_FW_VER, fw_ver);

    if (mibspi_err != MIBSPI_NO_ERR) {
        err = ARDUCHIP_ERR_MIBSPI;
    }

    return err;
}

/**
 * @brief Perform a register write/read test using the TEST register.
 *
 * Writes the test_val to the TEST register then reads the same register and confirms the result matches.
 *
 * @param[in] test_val Value to write to the TEST register
 *
 * @return Test result:
 *            - ARDUCHIP_SUCCESS if the test passes
 *            - ARDUCHIP_ERR_TEST_FAIL if the read value does not match test_val
 *            - ARDUCHIP_ERR_MIBSPI if a SPI error occurs
 */
static arduchip_err_t test_reg(uint8_t test_val) {
    arduchip_err_t err = ARDUCHIP_SUCCESS;

    do {
        mibspi_err_t mibspi_err;
        uint8_t test_val_rx = 0;

        // Write to TEST register
        mibspi_err = write_ardu_reg(ARDUCHIP_REG_TEST, test_val);

        if (mibspi_err != MIBSPI_NO_ERR) {
            err = ARDUCHIP_ERR_MIBSPI;
            break;
        }

        // Read from TEST register
        mibspi_err = read_ardu_reg(ARDUCHIP_REG_TEST, &test_val_rx);

        if (mibspi_err != MIBSPI_NO_ERR) {
            err = ARDUCHIP_ERR_MIBSPI;
            break;
        }

        // Check values match
        if (test_val_rx != test_val) {
            err = ARDUCHIP_ERR_TEST_FAIL;
            break;
        }
    } while (0);

    return err;
}

/**
 * @brief Read from the ArduChip over SPI in burst mode
 *
 * @param[in]  addr Register address (the first byte transmitted)
 * @param[out] data Pointer to where the received data will be stored (all data received after the address is transmitted).
 *                  If NULL is passed, the data will still be read but discarded immediately.
 *
 * @return Result code from the SPI operation
 */
static mibspi_err_t read_ardu_burst(uint8_t addr, uint8_t *data) {
    static uint16_t buf[CAMERA_TG_BURST_WORDS] = { 0 };

    memset(buf, 0, sizeof(buf));
    buf[0] = ((uint16_t)addr << 8U);

    // The same buffer is used for tx and rx since we don't need to preserve the tx data
    mibspi_err_t status = tms_mibspi_tx_rx(&transfer_group_burst, buf, buf, MIBSPI_TIMEOUT_MS);

    if ((status == MIBSPI_NO_ERR) && (data != NULL)) {
        uint8_t *buf_u8 = (uint8_t *)buf; // uint8_t is allowed to alias anything
        memcpy(data, (buf_u8 + 1), (sizeof(buf) - 1)); // - 1 to skip the first byte (which was when we were transmitting the register address)
    }

    return status;
}

/**
 * @brief Write a single register value to the ArduChip over SPI
 *
 * @param[in]  addr Register address
 * @param[out] data Value to write
 *
 * @return Result code from the SPI operation
 */
static mibspi_err_t write_ardu_reg(uint8_t addr, uint8_t data) {
    addr = (addr | ARDUCHIP_ADDR_WRITE);
    uint16_t tx = (uint16_t)((uint16_t)addr << 8U) | (uint16_t)data;
    mibspi_err_t status = tms_mibspi_tx(&transfer_group_single, &tx, MIBSPI_TIMEOUT_MS);
    return status;
}

/**
 * @brief Read a single register value from the ArduChip over SPI
 *
 * @param[in]  addr Register address
 * @param[out] data Pointer to where the received register value will be stored. Cannot be NULL.
 *
 * @return Result code from the SPI operation
 */
static mibspi_err_t read_ardu_reg(uint8_t addr, uint8_t *data) {
    uint16_t rx = 0;
    uint16_t tx = ((uint16_t)addr << 8U);
    mibspi_err_t status = tms_mibspi_tx_rx(&transfer_group_single, &tx, &rx, MIBSPI_TIMEOUT_MS);

    if (status == MIBSPI_NO_ERR) {
        *data = (rx & 0xFF);
    }

    return status;
}
