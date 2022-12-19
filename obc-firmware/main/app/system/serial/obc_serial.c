/**
 * @file obc_serial.c
 * @brief OBC serial driver
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_serial.h"

// OBC Serial
#include "obc_serial_rx.h"
#include "obc_serial_tx.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize FreeRTOS data structures for the OBC serial
 */
void obc_serial_create_infra(void) {
    obc_serial_tx_create_infra();
    obc_serial_rx_create_infra();
}

/**
 * @brief Configure and enable interrupts for the OBC serial
 */
void obc_serial_init_irqs(void) {
    obc_serial_tx_init_irq();
    obc_serial_rx_init_irq();
}

/**
 * @brief Start the OBC serial servicing tasks
 */
void obc_serial_start_tasks(void) {
    obc_serial_tx_start_task();
    obc_serial_rx_start_task();
}
