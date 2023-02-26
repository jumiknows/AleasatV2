/**
 * @file obc_camera.c
 * @brief OBC camera driver
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cam_arducam.h"

#include "cam_ov5642.h"
#include "cam_arduchip.h"

// OBC
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Period to wait between sending messages to the ArduCAM to check if an
 * image capture is complete.
 */
#define CAPTURE_POLL_PERIOD_MS 100

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Internal states of the ArduCAM
 */
typedef enum {
    ARDUCAM_STATE_INIT,
    ARDUCAM_STATE_IDLE,
    ARDUCAM_STATE_IMG_CAPTURED,
    ARDUCAM_STATE_TRANSFER_IN_PROGRESS,
} arducam_state_t;

/**
 * @brief Data associated with the current state of the ArduCAM
 */
typedef struct {
    /**
     * @brief Internal state of the ArduCAM.
     */
    arducam_state_t state;

    /**
     * @brief Original size of the ArduCAM FIFO after the image capture.
     * This field is NOT updated as the image data is transferred from the ArduCAM.
     */
    uint32_t fifo_len;

    /**
     * @brief Number of bytes remaining in the ArduCAM FIFO.
     * This field is updated as image data is transferred from the ArduCAM.
     */
    uint32_t fifo_rem;
} arducam_state_data_t;

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief Single instance of arducam_state_data_t
 */
static arducam_state_data_t state_data = {
    .state               = ARDUCAM_STATE_INIT,
    .fifo_len            = 0,
    .fifo_rem            = 0,
};

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static arducam_err_t wait_for_capture(uint32_t timeout_ms);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize the ArduCAM and prepare it for JPEG image capture
 * 
 * @return Status code:
 *            - ARDUCAM_SUCCESS if the initialization is successful
 *            - ARDUCAM_ERR_ARDUCHIP if an error with the ArduChip occurs
 *            - ARDUCAM_ERR_OV5642 if an error with the OV5642 sensor occurs
 */
arducam_err_t arducam_init(void) {
    arducam_err_t err = ARDUCAM_SUCCESS;

    do {
        arduchip_err_t arduchip_err;
        ov5642_err_t ov5642_err;

        // Initialize ArduChip
        arduchip_err = arduchip_init();
        if (arduchip_err != ARDUCHIP_SUCCESS) {
            err = ARDUCAM_ERR_ARDUCHIP;
            break;
        }

        // Initialize OV5642 image sensor
        ov5642_err = ov5642_init();
        if (ov5642_err != OV5642_SUCCESS) {
            err = ARDUCAM_ERR_OV5642;
            break;
        }

        // Setup default sensor settings for JPEG
        ov5642_err = ov5642_set_format(OV5642_FMT_JPEG);
        if (ov5642_err != OV5642_SUCCESS) {
            err = ARDUCAM_ERR_OV5642;
            break;
        }

        // Tell the ArduChip how the sensor timing signals are configured
        //   - HSYNC is active-high
        //   - VSYNC is active-low
        //   - PCLK is not reversed
        arduchip_err = arduchip_configure_sensor_timing(false, true, false);
        if (arduchip_err != ARDUCHIP_SUCCESS) {
            err = ARDUCAM_ERR_ARDUCHIP;
            break;
        }

        ov5642_err = ov5642_set_jpeg_size(OV5642_JPEG_SIZE_320X240);
        if (ov5642_err != OV5642_SUCCESS) {
            err = ARDUCAM_ERR_OV5642;
            break;
        }

        // Give sensor some time after configuration before preping ArduChip for image capture
        vTaskDelay(pdMS_TO_TICKS(1000));

        arduchip_err = arduchip_fifo_clear_wr_done();
        if (arduchip_err != ARDUCHIP_SUCCESS) {
            err = ARDUCAM_ERR_ARDUCHIP;
            break;
        }

        arduchip_err = arduchip_set_num_frames(1);
        if (arduchip_err != ARDUCHIP_SUCCESS) {
            err = ARDUCAM_ERR_ARDUCHIP;
            break;
        }
    } while (0);

    if (err == ARDUCAM_SUCCESS) {
        state_data.state = ARDUCAM_STATE_IDLE;
    }

    return err;
}

/**
 * @brief Capture an image. The ArduCAM must be IDLE before calling this function.
 * 
 * After triggering an image capture, we have to poll the ArduCAM to determine if the capture
 * is complete. The polling is spaced out by a time period of CAPTURE_POLL_PERIOD_MS.
 * The task calling this function will be suspended during these waiting periods.
 * 
 * @param[in] timeout_ms Timeout for the total amount of time the capture can take
 * 
 * @return Status code:
 *            - ARDUCAM_SUCCESS if the capture is successful
 *            - ARDUCAM_ERR_ARDUCHIP if an error with the ArduChip occurs
 *            - ARDUCAM_ERR_OV5642 if an error with the OV5642 sensor occurs
 *            - ARDUCAM_ERR_INVALID_STATE if the ArduCAM was in an invalid state
 *            - ARDUCAM_ERR_TIMEOUT if the capture times-out
 */
arducam_err_t arducam_capture(uint32_t timeout_ms) {
    if (state_data.state != ARDUCAM_STATE_IDLE) {
        return ARDUCAM_ERR_INVALID_STATE;
    }

    arducam_err_t err = ARDUCAM_SUCCESS;

    do {
        arduchip_err_t arduchip_err = ARDUCHIP_SUCCESS;

        // Clear FIFO flag so we're starting from a fresh state
        arduchip_err = arduchip_fifo_clear_wr_done();
        if (arduchip_err != ARDUCHIP_SUCCESS) {
            err = ARDUCAM_ERR_ARDUCHIP;
            break;
        }

        // Trigger the image
        arduchip_err = arduchip_fifo_start_capture();
        if (arduchip_err != ARDUCHIP_SUCCESS) {
            err = ARDUCAM_ERR_ARDUCHIP;
            break;
        }

        // Wait for the ArduCAM to finish capturing
        err = wait_for_capture(timeout_ms);
        if (err != ARDUCAM_SUCCESS) {
            break;
        }

        // Read and store the fifo length
        arduchip_err = arduchip_read_fifo_len(&(state_data.fifo_len));
        if (arduchip_err == ARDUCHIP_SUCCESS) {
            state_data.fifo_rem = state_data.fifo_len;
        } else {
            state_data.fifo_len = 0;
            state_data.fifo_rem = 0;
            err = ARDUCAM_ERR_ARDUCHIP;
            break;
        }

        // Update state
        state_data.state = ARDUCAM_STATE_IMG_CAPTURED;
    } while (0);

    return err;
}

/**
 * @brief Retrieve the size of the most recently captured image.
 * 
 * This function can only be called after calling arducam_capture(...).
 * 
 * @param[out] img_size Pointer to where the image size will be stored.
 * 
 * @return Status code:
 *            - ARDUCAM_SUCCESS if the image size can be retrieved
 *            - ARDUCAM_ERR_INVALID_STATE if the ArduCAM was in an invalid state
 *            - ARDUCAM_ERR_INVALID_ARGS if the img_size argument is NULL
 */
arducam_err_t arducam_get_img_size(uint32_t *img_size) {
    if (img_size == NULL) {
        return ARDUCAM_ERR_INVALID_ARGS;
    }

    if (!((state_data.state == ARDUCAM_STATE_IMG_CAPTURED) ||
          (state_data.state == ARDUCAM_STATE_TRANSFER_IN_PROGRESS))) {

        return ARDUCAM_ERR_INVALID_STATE;
    }

    *img_size = state_data.fifo_len;
    return ARDUCAM_SUCCESS;
}

/**
 * @brief Transfer image data from the ArduCAM to the provided buffer.
 * 
 * This function can only be called after calling arducam_capture(...).
 * 
 * After all image data has been transferred, the next call to this function will return 0 in
 * the data_len output parameter and return an ARDUCAM_SUCCESS status code. Any subsequent calls
 * to this function (without calling arducam_capture(...)) will cause an ARDUCAM_ERR_INVALID_STATE
 * error to be returned.
 * 
 * @param[out] buf      Buffer where image data will be transferred
 * @param[in]  buf_len  Size of the buffer, must be at least ARDUCAM_READ_SIZE
 * @param[out] data_len Number of bytes actually transferred to buf
 * 
 * @return Status code:
 *            - ARDUCAM_SUCCESS if the image data transfer is successful
 *            - ARDUCAM_ERR_INVALID_ARGS if the provided arguments are invalid
 *            - ARDUCAM_ERR_INVALID_STATE if the ArduCAM was in an invalid state
 *            - ARDUCAM_ERR_ARDUCHIP if an error with the ArduChip occurs
 */
arducam_err_t arducam_transfer_img_data(uint8_t *buf, uint32_t buf_len, uint32_t *data_len) {
    // Check args
    if ((buf == NULL) || (buf_len < ARDUCAM_READ_SIZE) || (data_len == NULL)) {
        return ARDUCAM_ERR_INVALID_ARGS;
    }

    // Update state
    if (state_data.state == ARDUCAM_STATE_IMG_CAPTURED) {
        state_data.state = ARDUCAM_STATE_TRANSFER_IN_PROGRESS;
    }

    // Check state
    if (state_data.state != ARDUCAM_STATE_TRANSFER_IN_PROGRESS) {
        return ARDUCAM_ERR_INVALID_STATE;
    }

    // Check if we've read all the image data
    if (state_data.fifo_rem == 0) {
        state_data.fifo_len = 0;
        state_data.state = ARDUCAM_STATE_IDLE;

        // Indicate to caller that we've finished reading
        *data_len = 0;

        return ARDUCAM_SUCCESS;
    }

    // Read more data
    arducam_err_t err = ARDUCAM_ERR_ARDUCHIP;

    uint32_t bytes_to_read = MIN(ARDUCAM_READ_SIZE, state_data.fifo_rem);
    arduchip_err_t arduchip_err = arduchip_read_fifo_burst(buf);

    if (arduchip_err == ARDUCHIP_SUCCESS) {
        state_data.fifo_rem -= bytes_to_read;
        *data_len = bytes_to_read;

        err = ARDUCAM_SUCCESS;
    }

    return err;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Repeatedly poll the ArduCAM (with a period of CAPTURE_POLL_PERIOD_MS)
 * until the capture is complete or a timeout occurs.
 * 
 * @param[in] timeout_ms Maximum time to wait for the capture
 * 
 * @return Status code:
 *            - ARDUCAM_SUCCESS if the capture is successful
 *            - ARDUCAM_ERR_ARDUCHIP if an error with the ArduChip occurs
 *            - ARDUCAM_ERR_TIMEOUT if the capture times-out
 */
static arducam_err_t wait_for_capture(uint32_t timeout_ms) {
    arducam_err_t err = ARDUCAM_SUCCESS;

    // Timing variables
    uint32_t start_ticks = xTaskGetTickCount();
    uint32_t poll_ticks = start_ticks;

    bool capture_done = false;
    while (!capture_done) {
        vTaskDelayUntil(&poll_ticks, pdMS_TO_TICKS(CAPTURE_POLL_PERIOD_MS));

        // Poll ArduCAM
        arduchip_err_t arduchip_err = arduchip_is_capture_done(&capture_done);
        if (arduchip_err != ARDUCHIP_SUCCESS) {
            err = ARDUCAM_ERR_ARDUCHIP;
            break;
        }

        // Check for timeout
        if (!capture_done) {
            if ((poll_ticks - start_ticks) > pdMS_TO_TICKS(timeout_ms)) {
                err = ARDUCAM_ERR_TIMEOUT;
                break;
            }
        }
    }

    return err;
}
