/**
 * @file cam_arducam.h
 * @brief ArduCAM device driver
 */

#ifndef CAM_ARDUCAM_H_
#define CAM_ARDUCAM_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Camera
#include "cam_arduchip.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define ARDUCAM_READ_SIZE  ARDUCHIP_BURST_READ_SIZE

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    ARDUCAM_SUCCESS           = 0,
    ARDUCAM_ERR_INVALID_ARGS  = -1,
    ARDUCAM_ERR_OV5642        = -2,
    ARDUCAM_ERR_ARDUCHIP      = -3,
    ARDUCAM_ERR_INVALID_STATE = -4,
    ARDUCAM_ERR_TIMEOUT       = -5,
} arducam_err_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

arducam_err_t arducam_init(void);
arducam_err_t arducam_capture(uint32_t timeout_ms);
arducam_err_t arducam_get_img_size(uint32_t *img_size);
arducam_err_t arducam_transfer_img_data(uint8_t *buf, uint32_t buf_len, uint32_t *data_len);

#endif // CAM_ARDUCAM_H_
