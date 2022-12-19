/**
 * @file obc_serial.h
 * @brief OBC serial driver
 */

#ifndef OBC_SERIAL_H_
#define OBC_SERIAL_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define OBC_SERIAL_FRAME_MAX_DATA_SIZE    ((uint8_t)234U)
#define OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE ((uint8_t)233U)

#define OBC_SERIAL_SYNC_0_VALUE           ((uint8_t)0xABU)
#define OBC_SERIAL_SYNC_1_VALUE           ((uint8_t)0xCDU)

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    OBC_SERIAL_SUCCESS            = 0,
    OBC_SERIAL_ERR_DATA_TOO_LARGE = 1,
    OBC_SERIAL_ERR_BUF_TOO_SMALL  = 2,
    OBC_SERIAL_TIMEOUT            = 3,
} obc_serial_err_t;

typedef enum {
    OBC_SERIAL_DATAGRAM_LOG   = 0,
    OBC_SERIAL_DATAGRAM_COMMS = 1,
} obc_serial_datagram_type_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void obc_serial_create_infra(void);
void obc_serial_init_irqs(void);
void obc_serial_start_tasks(void);

#endif // OBC_SERIAL_H_
