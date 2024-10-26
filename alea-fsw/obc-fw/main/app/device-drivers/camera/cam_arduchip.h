/**
 * @file cam_arduchip.h
 * @brief Driver for the ArduChip (the processor on ArduCAM modules)
 */

#ifndef CAM_ARDUCHIP_H_
#define CAM_ARDUCHIP_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_hardwaredefs.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief The number of bytes that are transferred on each burst read operation
 *
 * This is controlled by the MIBSPI buffer length settings. For each burst read
 * a single byte is consumed for the regiser address and the remaining bytes can
 * be used to read data.
 *
 * Each word is 16 bits (2 byte).
 */
#define ARDUCHIP_BURST_READ_SIZE ((CAMERA_TG_BURST_WORDS * 2U) - 1U)

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Error codes that can be returned by ArduChip driver functions
 */
typedef enum {
    ARDUCHIP_SUCCESS          = 0,
    ARDUCHIP_ERR_INVALID_ARGS = -1,
    ARDUCHIP_ERR_MIBSPI       = -2,
    ARDUCHIP_ERR_FW_VER       = -3,
    ARDUCHIP_ERR_TEST_FAIL    = -4,
} arduchip_err_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

arduchip_err_t arduchip_init(void);
arduchip_err_t arduchip_reset(void);

arduchip_err_t arduchip_configure_sensor_timing(bool hsync_active_low, bool vsync_active_low, bool pclk_reverse);
arduchip_err_t arduchip_set_num_frames(uint8_t num_frames);
arduchip_err_t arduchip_fifo_start_capture(void);

arduchip_err_t arduchip_is_capture_done(bool *done);
arduchip_err_t arduchip_read_fifo_len(uint32_t *fifo_len);
arduchip_err_t arduchip_read_fifo_burst(uint8_t *data);
arduchip_err_t arduchip_fifo_clear_wr_done(void);

#endif // CAM_ARDUCHIP_H_
