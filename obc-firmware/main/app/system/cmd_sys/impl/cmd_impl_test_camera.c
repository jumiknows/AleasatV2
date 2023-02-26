/**
 * @file cmd_impl_test_camera.c
 * @brief Implementation of test commands for the camera
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// Logger
#include "logger.h"

// Camera
#include "cam_arducam.h"

// Utils
#include "io_stream.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define CAPTURE_TIMEOUT_MS 500U

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

cmd_sys_resp_code_t cmd_impl_TEST_CAM_INIT(const cmd_sys_cmd_t *cmd, cmd_TEST_CAM_INIT_resp_t *resp) {
    log_str(INFO, LOG_CAMERA, "%s", "Initializing");
    arducam_err_t err = arducam_init();
    resp->arducam_err = err;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_err_t cmd_impl_TEST_CAM_CAPTURE(const cmd_sys_cmd_t *cmd, cmd_TEST_CAM_CAPTURE_resp_t *resp, const data_fmt_desc_t *resp_desc, uint32_t resp_len, uint8_t *buf) {
    cmd_sys_resp_code_t resp_code = CMD_SYS_RESP_CODE_ERROR;
    uint8_t img_buf[ARDUCAM_READ_SIZE] = { 0 };

    // Capture image
    log_str(INFO, LOG_CAMERA, "%s", "Capturing image");
    arducam_err_t err = arducam_capture(CAPTURE_TIMEOUT_MS);

    // Check image size
    uint32_t img_size = 0;
    if (err == ARDUCAM_SUCCESS) {
        err = arducam_get_img_size(&img_size);
        if (err == ARDUCAM_SUCCESS) {
            resp_code = CMD_SYS_RESP_CODE_SUCCESS;
        } else {
            img_size = 0;
        }
    }

    // Start response
    cmd_sys_err_t cmd_sys_err = cmd_sys_begin_response(cmd, resp_code, (resp_len + img_size));
    if (cmd_sys_err != CMD_SYS_SUCCESS) return cmd_sys_err;

    // Send fixed fields
    resp->arducam_err = err;
    resp->image_size = img_size;
    cmd_sys_err = cmd_sys_handle_resp_fields(cmd, resp, resp_desc, resp_len, buf);
    if (cmd_sys_err != CMD_SYS_SUCCESS) return cmd_sys_err;

    // Send image data
    log_str(INFO, LOG_CAMERA, "Transferring image (%d KB)", (img_size / 1024));
    if (err == ARDUCAM_SUCCESS) {
        bool done = false;
        while (!done) {
            // Read buffer of image data from ArduCAM
            uint32_t data_len = 0;
            err = arducam_transfer_img_data(img_buf, sizeof(img_buf), &data_len);
            if (err != ARDUCAM_SUCCESS) {
                // TODO fill out rest of response or abort
                break;
            }

            if (data_len > 0) {
                // Send the data to the output stream
                uint32_t bytes_written = io_stream_write(cmd->output, img_buf, data_len, pdMS_TO_TICKS(CMD_SYS_OUTPUT_WRITE_TIMEOUT_MS), NULL);
                if (bytes_written != data_len) {
                    break;
                }
            } else {
                // data_len == 0 indicates all data has been transferred.
                done = true;
            }
        }

        if (!done) {
            // TODO abort response
        }
    }

    cmd_sys_err = cmd_sys_finish_response(cmd);

    return cmd_sys_err;
}
