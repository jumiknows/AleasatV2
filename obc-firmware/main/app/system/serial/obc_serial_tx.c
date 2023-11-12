/**
 * @file obc_serial_tx.c
 * @brief OBC serial tx driver
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_serial_tx.h"

// OBC Serial
#include "obc_serial.h"

// OBC
#include "obc_watchdog.h"
#include "obc_rtos.h"
#include "obc_hardwaredefs.h"

// Utils
#include "rtos_stream.h"
#include "io_stream.h"
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

// HALCoGen
#include "sci.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define COMMS_MSG_BUFFER_SIZE  (OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE * 2U)
#define LOGS_MSG_BUFFER_SIZE   255U

#define SCI_MUTEX_TIMEOUT_MS   500U

#define OBC_SERIAL_TX_POLL_PERIOD_MS 1000U

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
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void obc_serial_tx_task(void *pvParameters);
static void send_datagram(obc_serial_datagram_type_t type, const uint8_t *data, uint8_t data_len);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

// FreeRTOS objects
static MessageBufferHandle_t comms_msg_buffer = NULL;
static MessageBufferHandle_t logs_msg_buffer  = NULL;
static SemaphoreHandle_t sci_mutex            = NULL;

// Streams
static rtos_msg_ostream_t comms_msg_stream = {
    .msg_buf = NULL, // Assigned in obc_serial_tx_create_infra
    .mutex   = NULL,
};

static rtos_msg_ostream_t logs_msg_stream = {
    .msg_buf = NULL, // Assigned in obc_serial_tx_create_infra
    .mutex   = NULL,
};

// Task Parameters
static uint8_t comms_data_buf[OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE];
static obc_serial_tx_task_params_t comms_task_params = {
    .task_id = OBC_TASK_ID_OBC_SERIAL_TX_COMMS,
    .datagram_type = OBC_SERIAL_DATAGRAM_COMMS,
    .msg_buf = NULL, // Assigned in obc_serial_tx_create_infra
    .data_buf = comms_data_buf,
    .data_buf_len = sizeof(comms_data_buf),
};

static uint8_t logs_data_buf[OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE];
static obc_serial_tx_task_params_t logs_task_params = {
    .task_id = OBC_TASK_ID_OBC_SERIAL_TX_LOGS,
    .datagram_type = OBC_SERIAL_DATAGRAM_LOG,
    .msg_buf = NULL, // Assigned in obc_serial_tx_create_infra
    .data_buf = logs_data_buf,
    .data_buf_len = sizeof(logs_data_buf),
};

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

const io_ostream_t obc_serial_comms_out = {
    .handle = &comms_msg_stream,
    .write = &rtos_stream_write_msg,
    .flush = NULL,
};

const io_ostream_t obc_serial_logs_out = {
    .handle = &logs_msg_stream,
    .write = &rtos_stream_write_msg,
    .flush = NULL,
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize FreeRTOS data structures for the OBC serial TX
 */
void obc_serial_tx_create_infra(void) {
    // Message buffer for COMMS
    static StaticStreamBuffer_t comms_msg_buffer_buf                   = { 0 };
    static uint8_t comms_msg_buffer_storage[COMMS_MSG_BUFFER_SIZE + 1] = { 0 }; // See https://www.freertos.org/xMessageBufferCreateStatic.html for explanation of + 1

    // Message buffer for LOGS
    static StaticStreamBuffer_t logs_msg_buffer_buf                  = { 0 };
    static uint8_t logs_msg_buffer_storage[LOGS_MSG_BUFFER_SIZE + 1] = { 0 }; // See https://www.freertos.org/xMessageBufferCreateStatic.html for explanation of + 1

    // Mutex for SCI peripheral
    static StaticSemaphore_t sci_mutex_buf = { 0 };

    // Initialize FreeRTOS objects
    comms_msg_buffer = xMessageBufferCreateStatic(COMMS_MSG_BUFFER_SIZE, comms_msg_buffer_storage, &comms_msg_buffer_buf);
    logs_msg_buffer = xMessageBufferCreateStatic(LOGS_MSG_BUFFER_SIZE, logs_msg_buffer_storage, &logs_msg_buffer_buf);
    sci_mutex = xSemaphoreCreateMutexStatic(&sci_mutex_buf);

    comms_msg_stream.msg_buf = comms_msg_buffer;
    logs_msg_stream.msg_buf  = logs_msg_buffer;

    comms_task_params.msg_buf = comms_msg_buffer;
    logs_task_params.msg_buf  = logs_msg_buffer;
}

/**
 * @brief Configure and enable interrupts for the OBC serial TX
 */
void obc_serial_tx_init_irq(void) {
    // Nothing to do
}

/**
 * @brief Start the OBC serial TX servicing task
 */
void obc_serial_tx_start_task(void) {
    obc_rtos_create_task(OBC_TASK_ID_OBC_SERIAL_TX_COMMS, &obc_serial_tx_task, &comms_task_params, OBC_WATCHDOG_ACTION_ALLOW);
    obc_rtos_create_task(OBC_TASK_ID_OBC_SERIAL_TX_LOGS, &obc_serial_tx_task, &logs_task_params, OBC_WATCHDOG_ACTION_ALLOW);
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
