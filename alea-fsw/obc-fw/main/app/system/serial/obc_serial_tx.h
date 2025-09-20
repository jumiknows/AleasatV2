/**
 * @file obc_serial_tx.h
 * @brief OBC serial tx driver
 */

#ifndef OBC_SERIAL_TX_H_
#define OBC_SERIAL_TX_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_serial_defs.h"
#include "obc_watchdog.h"
#include "obc_tasks_ids_gen.h"

// Utils
#include "io_stream.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    obc_task_id_t task_id;
    obc_serial_datagram_type_t datagram_type;
    MessageBufferHandle_t msg_buf;
    uint8_t *data_buf;
    uint32_t data_buf_len;
} obc_serial_tx_task_params_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void obc_serial_tx_pre_init(void);
void obc_serial_tx_create_task(obc_task_id_t task_id, obc_serial_tx_task_params_t *params, obc_watchdog_action_t watchdog_action);

#endif // OBC_SERIAL_TX_H_
