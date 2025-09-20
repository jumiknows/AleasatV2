/**
 * @file comms_obc_serial.h
 * @brief COMMS device implementation using the OBC serial port
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_obc_serial.h"

// COMMS
#include "comms_defs.h"

// OBC Serial
#include "obc_serial_defs.h"
#include "obc_serial_tx.h"
#include "obc_serial_rx.h"

// Utils
#include "buffered_io.h"
#include "rtos_stream.h"
#include "io_stream.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define COMMS_MSG_BUFFER_SIZE  (OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE * 2U)

#define COMMS_OBC_SERIAL_TIMEOUT_MS 500U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static comms_dev_err_type_t comms_obc_serial_tx(const uint16_t *tx_buffer, uint16_t num_bytes);
static comms_dev_err_type_t comms_obc_serial_rx(uint16_t *rx_buffer, uint16_t buffer_len);

static bool comms_obc_serial_rx_handler(const uint8_t *data, uint8_t data_len, uint32_t timeout_ticks);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief Definition of the comms driver interface for this device
 */
static comms_device_t cdev = {
    .tx = &comms_obc_serial_tx,
    .rx = &comms_obc_serial_rx,
    .cb = NULL,
    .cb_param = NULL
};

// RX

static MessageBufferHandle_t rx_msg_buffer = NULL;

static rtos_msg_istream_t rx_msg_stream = {
    .msg_buf = NULL, // Assigned in obc_serial_tx_create_infra
};

static const buffered_block_istream_t obc_serial_comms_in = {
    .handle         = &rx_msg_stream,
    .max_block_size = OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE,
    .read_block     = &rtos_stream_read_msg,
};

// TX

static MessageBufferHandle_t tx_msg_buffer = NULL;

static rtos_msg_ostream_t tx_msg_stream = {
    .msg_buf = NULL, // Assigned in obc_serial_tx_create_infra
    .mutex   = NULL,
};

static uint8_t tx_data_buf[OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE];
static obc_serial_tx_task_params_t tx_task_params = {
    .task_id       = OBC_TASK_ID_OBC_SERIAL_TX_COMMS,
    .datagram_type = OBC_SERIAL_DATAGRAM_COMMS,
    .msg_buf       = NULL, // Assigned in obc_serial_tx_create_infra
    .data_buf      = tx_data_buf,
    .data_buf_len  = sizeof(tx_data_buf),
};

static const io_ostream_t obc_serial_comms_out = {
    .handle = &tx_msg_stream,
    .write  = &rtos_stream_write_msg,
    .flush  = NULL,
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize FreeRTOS data structures for COMMS OBC serial driver
 */
void comms_obc_serial_create_infra(void) {
    // TX
    static StaticMessageBuffer_t tx_msg_buffer_buf                  = { 0 };
    static uint8_t tx_msg_buffer_storage[COMMS_MSG_BUFFER_SIZE + 1] = { 0 }; // See https://www.freertos.org/xMessageBufferCreateStatic.html for explanation of + 1

    tx_msg_buffer = xMessageBufferCreateStatic(COMMS_MSG_BUFFER_SIZE, tx_msg_buffer_storage, &tx_msg_buffer_buf);
    tx_msg_stream.msg_buf  = tx_msg_buffer;
    tx_task_params.msg_buf = tx_msg_buffer;

    // RX
    static StaticMessageBuffer_t rx_msg_buffer_buf                  = { 0 };
    static uint8_t rx_msg_buffer_storage[COMMS_MSG_BUFFER_SIZE + 1] = { 0 }; // See https://www.freertos.org/xMessageBufferCreateStatic.html for explanation of + 1

    rx_msg_buffer = xMessageBufferCreateStatic(COMMS_MSG_BUFFER_SIZE, rx_msg_buffer_storage, &rx_msg_buffer_buf);
    rx_msg_stream.msg_buf = rx_msg_buffer;
    obc_serial_rx_register_handler(OBC_SERIAL_DATAGRAM_COMMS, &comms_obc_serial_rx_handler);
}

/**
 * @brief Create task for transmitting COMMS packets
 */
void comms_obc_serial_create_task(void) {
    obc_serial_tx_create_task(OBC_TASK_ID_OBC_SERIAL_TX_COMMS, &tx_task_params, OBC_WATCHDOG_ACTION_ALLOW);
}

/**
 * @brief Get a driver handle for this device
 *
 * @return comms_dev_hdl_t opaque driver interface handle
 */
comms_dev_handle_t comms_obc_serial_get_handle(void) {
    return (comms_dev_handle_t)&cdev;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Transmit one packet of data to COMMS (i.e. the OBC serial port)
 *
 * @param[in] tx_buffer pointer to the data to transmit
 * @param[in] msg_len   length of the message (i.e. number of bytes to transmit)
 *
 * @return COMMS_DEV_SUCCESS if no error
 *         COMMS_DEV_BUS_FAILURE otherwise
 */
static comms_dev_err_type_t comms_obc_serial_tx(const uint16_t *tx_buffer, uint16_t msg_len) {
    const uint8_t *tx_buf_u8 = (const uint8_t *)&tx_buffer[0];

    if (msg_len > OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE) {
        return COMMS_DEV_ERR_INVALID_ARG;
    }

    uint32_t bytes_written = io_stream_write(&obc_serial_comms_out, tx_buf_u8, msg_len, pdMS_TO_TICKS(COMMS_OBC_SERIAL_TIMEOUT_MS), NULL);

    if (bytes_written != msg_len) {
        return COMMS_DEV_ERR_BUS_FAILURE;
    }

    return COMMS_DEV_SUCCESS;
}

/**
 * @brief Receive one packet of data from COMMS (i.e. the OBC serial port)
 *
 * @param[out] rx_buffer  pointer to the data to receive, MUST be 256 bytes long
 * @param[in]  buffer_len length of the buffer
 *
 * @return COMMS_DEV_SUCCESS if no error,
 *         COMMS_DEV_BUS_FAILURE otherwise
 */
static comms_dev_err_type_t comms_obc_serial_rx(uint16_t *rx_buffer, uint16_t buffer_len) {
    if (buffer_len < OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE) {
        return COMMS_DEV_ERR_INVALID_ARG;
    }

    uint8_t *rx_buf_u8 = (uint8_t *)&rx_buffer[0];

    uint32_t bytes_read = buffered_block_istream_read_block(&obc_serial_comms_in, rx_buf_u8, buffer_len, pdMS_TO_TICKS(COMMS_OBC_SERIAL_TIMEOUT_MS),
                          NULL);

    if (bytes_read < COMMS_MIN_PKT_SIZE_BYTES) {
        return COMMS_DEV_ERR_BUS_FAILURE;
    }

    return COMMS_DEV_SUCCESS;
}

static bool comms_obc_serial_rx_handler(const uint8_t *data, uint8_t data_len, uint32_t timeout_ticks) {
    bool success = false;

    if (rx_msg_buffer != NULL) {
        uint32_t bytes_sent = xMessageBufferSend(rx_msg_buffer, data, data_len, timeout_ticks);

        if (bytes_sent == data_len) {
            success = true;
            // Notify COMMS device-level driver that a message is available
            // (emulating the behaviour of the RX interrupt line from an actual COMMS board)

            comms_dev_cb_func_t cb_func = cdev.cb;
            void *param = cdev.cb_param;

            if (cb_func != NULL) {
                cb_func(false, param); // false because this is not called from an ISR context
            }
        }
    }

    return success;
}
