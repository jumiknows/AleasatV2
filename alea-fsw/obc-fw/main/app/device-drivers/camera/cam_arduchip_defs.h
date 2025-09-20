/**
 * @file cam_arduchip_defs.h
 * @brief Definitions for the ArduChip (the processor on ArduCAM modules)
 *
 * Reference Documentation: https://www.arducam.com/docs/spi-cameras-for-arduino/hardware/arducam-chip/
 */

#ifndef CAM_ARDUCHIP_DEFS_H_
#define CAM_ARDUCHIP_DEFS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_utils.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Mask for write operations to register addresses
 * (bit 7 must be set for write and cleared for read)
 */
#define ARDUCHIP_ADDR_WRITE UINT8_BIT(7)

/***************************** REGISTER ADDRESSES *****************************/

#define ARDUCHIP_REG_TEST          0x00
#define ARDUCHIP_REG_CAPTURE_CTRL  0x01
#define ARDUCHIP_REG_SENSOR_TIMING 0x03
#define ARDUCHIP_REG_FIFO_CTRL     0x04
#define ARDUCHIP_REG_RESET         0x07
#define ARDUCHIP_REG_BURST_FIFO_RD 0x3C
#define ARDUCHIP_REG_FW_VER        0x40
#define ARDUCHIP_REG_STATUS        0x41
#define ARDUCHIP_REG_FIFO_SIZE_0   0x42
#define ARDUCHIP_REG_FIFO_SIZE_1   0x43
#define ARDUCHIP_REG_FIFO_SIZE_2   0x44

/****************************** REGISTER VALUES *******************************/

#define ARDUCHIP_FW_VER_E 0x73 // (Mini 5MP Plus - UC-367 Rev. E)
#define ARDUCHIP_FW_VER_F 0x80 // (Mini 5MP Plus - UC-268 Rev. F)

#define ARDUCHIP_REG_RESET_RESET                     UINT8_BIT(7)

#define ARDUCHIP_REG_SENSOR_TIMING_HSYNC_ACTIVE_LOW  UINT8_BIT(0)
#define ARDUCHIP_REG_SENSOR_TIMING_VSYNC_ACTIVE_LOW  UINT8_BIT(1)
#define ARDUCHIP_REG_SENSOR_TIMING_PCLK_REVERSE      UINT8_BIT(3)

#define ARDUCHIP_REG_FIFO_CTRL_CLEAR_WR_DONE         UINT8_BIT(0)
#define ARDUCHIP_REG_FIFO_CTRL_START_CAPTURE         UINT8_BIT(1)
#define ARDUCHIP_REG_FIFO_CTRL_RESET_RD_PTR          UINT8_BIT(4)

#define ARDUCHIP_REG_STATUS_VSYNC_PIN                UINT8_BIT(0)
#define ARDUCHIP_REG_STATUS_EXT_TRIG                 UINT8_BIT(1)
#define ARDUCHIP_REG_STATUS_WR_FIFO_DONE             UINT8_BIT(3)

#endif // CAM_ARDUCHIP_DEFS_H_
