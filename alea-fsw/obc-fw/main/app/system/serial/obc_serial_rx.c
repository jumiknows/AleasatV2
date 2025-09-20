/**
 * @file obc_serial_rx.c
 * @brief OBC serial rx driver
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_serial_rx.h"

// OBC Serial
#include "obc_serial_defs.h"

// OBC
#include "obc_watchdog.h"
#include "obc_rtos.h"
#include "obc_hardwaredefs.h"

// Utils
#include "rtos_stream.h"
#include "buffered_io.h"
#include "obc_crc.h"
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// HALCoGen
#include "sci.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define PHY_STREAM_BUFFER_SIZE          255U
#define PHY_STREAM_BUFFER_DEF_TRIG_LVL  1U

#define FRAME_TIMEOUT_MS                500U

#define OBC_SERIAL_RX_POLL_PERIOD_MS    1000U

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    OBC_SERIAL_RX_STATE_SYNC_0,
    OBC_SERIAL_RX_STATE_SYNC_1,
    OBC_SERIAL_RX_STATE_HEADER,
    OBC_SERIAL_RX_STATE_DATA,
    OBC_SERIAL_RX_STATE_CRC,
} obc_serial_rx_state_t;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void obc_serial_rx_task(void *pvParameters);
static bool handle_datagram(const uint8_t *datagram_buf, uint8_t len, uint32_t timeout_ticks);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

// FreeRTOS objects
static StreamBufferHandle_t phy_stream_buffer = NULL;

// Data
static uint8_t rx_byte;

// Handlers
static obc_serial_rx_handler_t rx_handlers[OBC_SERIAL_DATAGRAM_TYPE_COUNT] = { 0 };

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/


/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize FreeRTOS data structures for the OBC serial RX
 */
void obc_serial_rx_create_infra(void) {
    // Stream buffer for physical serial port
    static StaticStreamBuffer_t phy_stream_buffer_buf                    = { 0 };
    static uint8_t phy_stream_buffer_storage[PHY_STREAM_BUFFER_SIZE + 1] = { 0 }; // See https://www.freertos.org/xStreamBufferCreateStatic.html for explanation of + 1

    phy_stream_buffer = xStreamBufferCreateStatic(PHY_STREAM_BUFFER_SIZE, PHY_STREAM_BUFFER_DEF_TRIG_LVL, phy_stream_buffer_storage,
                        &phy_stream_buffer_buf);
}

/**
 * @brief Register a handler for a particular datagram type. This will overwrite a previously registered handler
 * for the same datagram type.
 *
 * When a packet of the given datagram type is received, the handler will be invoked and passed the packet data.
 *
 * @param datagram_type Type of datagram this handler can handle
 * @param handler       Handler to register
 */
void obc_serial_rx_register_handler(obc_serial_datagram_type_t datagram_type, obc_serial_rx_handler_t handler) {
    rx_handlers[datagram_type] = handler;
}

/**
 * @brief Configure and enable interrupts for the OBC serial RX
 */
void obc_serial_rx_init_irq(void) {
    sciEnableNotification(UART_DEBUG, SCI_RX_INT);

    // This HALCoGen SCI driver has a built-in buffering mechanism so that multiple bytes can be
    // collected before a call to sciNotification. Since we need to pass the data from the ISR to
    // a task anyway (to avoid dropping bytes while processing a message), we'll receive one byte
    // at a time.
    //
    // When sciNotification fires, rx_byte will contain the new byte.
    // We will have to call sciReceive again to receive the next byte.
    sciReceive(UART_DEBUG, 1, &rx_byte);
}

/**
 * @brief Create the OBC serial RX servicing task
 */
void obc_serial_rx_create_task(void) {
    obc_rtos_create_task(OBC_TASK_ID_OBC_SERIAL_RX, &obc_serial_rx_task, NULL, OBC_WATCHDOG_ACTION_ALLOW);
}

/**
 * @brief ISR handler for RX events on the OBC serial port
 *
 * @param pxHigherPriorityTaskWoken Pointer to store flag that a higher priority task was woken
 */
void obc_serial_rx_isr(BaseType_t *pxHigherPriorityTaskWoken) {
    xStreamBufferSendFromISR(phy_stream_buffer, &rx_byte, 1, pxHigherPriorityTaskWoken);
    sciReceive(UART_DEBUG, 1, &rx_byte);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief OBC serial RX servicing task
 *
 * @param pvParameters Task parameters (see obc_rtos)
 */
static void obc_serial_rx_task(void *pvParameters) {
    static uint8_t data_buf[OBC_SERIAL_FRAME_MAX_DATA_SIZE] = { 0 };

    obc_serial_rx_state_t state = OBC_SERIAL_RX_STATE_SYNC_0;
    uint8_t data_len = 0;
    uint8_t crc_buf[2] = { 0 };

    while (1) {
        obc_watchdog_pet(OBC_TASK_ID_OBC_SERIAL_RX);

        // Parse a serial frame to extract/validate the data field (which contains a datagram)
        switch (state) {
        case OBC_SERIAL_RX_STATE_SYNC_0:
            xStreamBufferSetTriggerLevel(phy_stream_buffer, 1);

            if (xStreamBufferReceive(phy_stream_buffer, data_buf, 1, pdMS_TO_TICKS(OBC_SERIAL_RX_POLL_PERIOD_MS)) == 1) {
                if (data_buf[0] == OBC_SERIAL_SYNC_0_VALUE) {
                    state = OBC_SERIAL_RX_STATE_SYNC_1;
                }
            }

            break;

        case OBC_SERIAL_RX_STATE_SYNC_1:
            xStreamBufferSetTriggerLevel(phy_stream_buffer, 1);

            if (xStreamBufferReceive(phy_stream_buffer, data_buf, 1, pdMS_TO_TICKS(FRAME_TIMEOUT_MS)) == 1) {
                if (data_buf[0] == OBC_SERIAL_SYNC_1_VALUE) {
                    state = OBC_SERIAL_RX_STATE_HEADER;
                } else {
                    state = OBC_SERIAL_RX_STATE_SYNC_0;
                }
            } else {
                state = OBC_SERIAL_RX_STATE_SYNC_0;
            }

            break;

        case OBC_SERIAL_RX_STATE_HEADER:
            xStreamBufferSetTriggerLevel(phy_stream_buffer, 1);

            if (xStreamBufferReceive(phy_stream_buffer, &data_len, 1, pdMS_TO_TICKS(FRAME_TIMEOUT_MS)) == 1) {
                // Check that data_len is valid
                if ((data_len >= 1) && (data_len <= OBC_SERIAL_FRAME_MAX_DATA_SIZE)) {
                    state = OBC_SERIAL_RX_STATE_DATA;
                } else {
                    // If not, ignore the rest of the frame (i.e. wait for SYNC_0 again)
                    state = OBC_SERIAL_RX_STATE_SYNC_0;
                }
            } else {
                state = OBC_SERIAL_RX_STATE_SYNC_0;
            }

            break;

        case OBC_SERIAL_RX_STATE_DATA:
            xStreamBufferSetTriggerLevel(phy_stream_buffer, data_len);

            if (xStreamBufferReceive(phy_stream_buffer, data_buf, data_len, pdMS_TO_TICKS(FRAME_TIMEOUT_MS)) == data_len) {
                state = OBC_SERIAL_RX_STATE_CRC;
            } else {
                state = OBC_SERIAL_RX_STATE_SYNC_0;
            }

            break;

        case OBC_SERIAL_RX_STATE_CRC:
            xStreamBufferSetTriggerLevel(phy_stream_buffer, 2);

            if (xStreamBufferReceive(phy_stream_buffer, crc_buf, 2, pdMS_TO_TICKS(FRAME_TIMEOUT_MS)) == 2) {
                // TODO ALEA-843 log error if false returned from handle_datagram
                uint16_t rec_crc = crc_buf[1] | (crc_buf[0] << 8);

                if (crc_16_buf(CRC16_SEED, data_buf, data_len) == rec_crc) {
                    handle_datagram(data_buf, data_len, portMAX_DELAY);
                } else {
                    LOG_RX_CRC_CHECK__CRC_MISMATCH();
                }

                state = OBC_SERIAL_RX_STATE_SYNC_0;
            }

            break;

        default:
            state = OBC_SERIAL_RX_STATE_SYNC_0;
            break;
        }
    }
}

/**
 * @brief Handle the reception of a complete datagram.
 *
 * Depending on the type byte, the datagram will be routed to the appropriate message buffer
 *
 * @param[in] datagram_buf Pointer to the datagram buffer
 * @param[in] len          Length of the datagram
 *
 * @return true if the datagram is valid, otherwise false
 */
static bool handle_datagram(const uint8_t *datagram_buf, uint8_t len, uint32_t timeout_ticks) {
    if (len < 2) {
        // Need at least two bytes (one for the datagram type and one of actual data)
        return false;
    }

    obc_serial_datagram_type_t type = (obc_serial_datagram_type_t)datagram_buf[0];
    uint8_t data_len = (len - 1);

    bool success = false;

    if (type < OBC_SERIAL_DATAGRAM_TYPE_COUNT) {
        obc_serial_rx_handler_t handler = rx_handlers[type];

        if (handler != NULL) {
            success = handler(&datagram_buf[1], data_len, timeout_ticks);
        }
    }

    return success;
}
