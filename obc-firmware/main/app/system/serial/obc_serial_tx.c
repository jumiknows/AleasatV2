/**
 * @file obc_serial_tx.c
 * @brief OBC serial tx driver
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_serial_tx.h"

// OBC Serial
#include "obc_serial_defs.h"

// OBC
#include "obc_watchdog.h"
#include "obc_rtos.h"
#include "obc_hardwaredefs.h"

// FreeRTOS
#include "rtos.h"

// HALCoGen
#include "sci.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define SCI_MUTEX_TIMEOUT_MS         500U

#define OBC_SERIAL_TX_POLL_PERIOD_MS 1000U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void obc_serial_tx_task(void *pvParameters);
static void send_datagram(obc_serial_datagram_type_t type, const uint8_t *data, uint8_t data_len);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static SemaphoreHandle_t sci_mutex = NULL;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize FreeRTOS data structures for the OBC serial TX
 */
void obc_serial_tx_create_infra(void) {
    // Mutex for SCI peripheral
    static StaticSemaphore_t sci_mutex_buf = { 0 };

    sci_mutex = xSemaphoreCreateMutexStatic(&sci_mutex_buf);
}

/**
 * @brief Create a task for transmitting data on the OBC serial
 */
void obc_serial_tx_create_task(obc_task_id_t task_id, obc_serial_tx_task_params_t *params, obc_watchdog_action_t watchdog_action) {
    obc_rtos_create_task(task_id, &obc_serial_tx_task, params, watchdog_action);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief OBC serial TX servicing task
 * 
 * @param pvParameters Task parameters (see obc_rtos)
 */
static void obc_serial_tx_task(void *pvParameters) {
    const obc_serial_tx_task_params_t *params = (const obc_serial_tx_task_params_t *)pvParameters;

    while (1) {
        obc_watchdog_pet(params->task_id);

        uint32_t data_len = xMessageBufferReceive(params->msg_buf, params->data_buf, params->data_buf_len, pdMS_TO_TICKS(OBC_SERIAL_TX_POLL_PERIOD_MS));
        if (data_len > 0) {
            send_datagram(params->datagram_type, params->data_buf, data_len);
        }
    }
}

/**
 * @brief Frame a datagram and send it over the OBC serial
 * 
 * @param[in] type     Type of datagram to send
 * @param[in] data     Pointer to data buffer to send
 * @param[in] data_len Length of the data buffer
 */
static void send_datagram(obc_serial_datagram_type_t type, const uint8_t *data, uint8_t data_len) {
    uint8_t header[4] = {
        OBC_SERIAL_SYNC_0_VALUE,
        OBC_SERIAL_SYNC_1_VALUE,
        (data_len + 1), // + 1 for the type byte
        (uint8_t)type,
    };

    // TODO ALEA-842 actually calculate CRC
    uint8_t crc[2] = { 0x12, 0x34 };

    if (xSemaphoreTake(sci_mutex, pdMS_TO_TICKS(SCI_MUTEX_TIMEOUT_MS))) {
        // Send header
        sciSend(UART_DEBUG, sizeof(header), header);

        // Send data
        sciSend(UART_DEBUG, data_len, (uint8_t *)data); // HALCoGen doesn't declare the data argument as const despite the fact that it does not modify it

        // Send CRC
        sciSend(UART_DEBUG, sizeof(crc), crc);

        xSemaphoreGive(sci_mutex);
    }
}
