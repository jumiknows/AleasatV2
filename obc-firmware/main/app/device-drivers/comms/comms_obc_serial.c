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

// OBC
#include "obc_serial_tx.h"
#include "obc_serial_rx.h"

// Utils
#include "buffered_io.h"
#include "io_stream.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define COMMS_OBC_SERIAL_TIMEOUT_MS 500

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static comms_dev_err_type_t comms_obc_serial_tx(const uint16_t* tx_buffer, uint16_t num_bytes);
static comms_dev_err_type_t comms_obc_serial_rx(uint16_t* rx_buffer, uint16_t buffer_len);

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

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize comms device
 */
void comms_obc_serial_init(void) {
    // Nothing to do
}

/**
 * @brief Get a driver handle for this device
 *
 * @return comms_dev_hdl_t opaque driver interface handle
 */
comms_dev_handle_t comms_obc_serial_get_handle(void) {
    return (comms_dev_handle_t)&cdev;
}

/**
 * @brief Invoke a registered callback function
 */
void comms_obc_serial_invoke_cb(void) {
    comms_dev_cb_func_t cb_func = cdev.cb;
    void* param = cdev.cb_param;

    if (cb_func != NULL) {
        cb_func(false, param); // false because this is not called from an ISR context
    }
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
static comms_dev_err_type_t comms_obc_serial_tx(const uint16_t* tx_buffer, uint16_t msg_len) {
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
static comms_dev_err_type_t comms_obc_serial_rx(uint16_t* rx_buffer, uint16_t buffer_len) {
    if (buffer_len < OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE) {
        return COMMS_DEV_ERR_INVALID_ARG;
    }

    uint8_t *rx_buf_u8 = (uint8_t *)&rx_buffer[0];

    uint32_t bytes_read = buffered_block_istream_read_block(&obc_serial_comms_in, rx_buf_u8, buffer_len, pdMS_TO_TICKS(COMMS_OBC_SERIAL_TIMEOUT_MS), NULL);
    if (bytes_read < COMMS_MIN_PKT_SIZE_BYTES) {
        return COMMS_DEV_ERR_BUS_FAILURE;
    }

    return COMMS_DEV_SUCCESS;
}
