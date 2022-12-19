/**
 * @file obc_serial_rx.c
 * @brief OBC serial rx driver
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_serial_rx.h"

// OBC Serial
#include "obc_serial.h"

// OBC
#include "obc_watchdog.h"
#include "obc_rtos.h"
#include "obc_task_info.h"
#include "obc_hardwaredefs.h"
#include "obc_misra.h"

// Utils
#include "rtos_stream.h"
#include "buffered_io.h"

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

/**
 * @brief Data buffer to store up to 2 datagrams
 */
#define COMMS_MSG_BUFFER_SIZE           (OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE * 2U)

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
static MessageBufferHandle_t comms_msg_buffer  = NULL;

// Data
static uint8_t rx_byte;

// Stream
static rtos_msg_istream_t msg_stream = {
    .msg_buf = NULL, // Assigned in obc_serial_rx_create_infra
};

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

const buffered_block_istream_t obc_serial_comms_in = {
    .handle         = &msg_stream,
    .max_block_size = OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE,
    .read_block     = &rtos_stream_read_msg,
};

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

    // Message buffer for comms
    static StaticMessageBuffer_t comms_msg_buffer_buf                  = { 0 };
    static uint8_t comms_msg_buffer_storage[COMMS_MSG_BUFFER_SIZE + 1] = { 0 }; // See https://www.freertos.org/xMessageBufferCreateStatic.html for explanation of + 1

    phy_stream_buffer = xStreamBufferCreateStatic(PHY_STREAM_BUFFER_SIZE, PHY_STREAM_BUFFER_DEF_TRIG_LVL, phy_stream_buffer_storage, &phy_stream_buffer_buf);
    comms_msg_buffer = xMessageBufferCreateStatic(COMMS_MSG_BUFFER_SIZE, comms_msg_buffer_storage, &comms_msg_buffer_buf);

    msg_stream.msg_buf = comms_msg_buffer;
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
 * @brief Start the OBC serial RX servicing task
 */
void obc_serial_rx_start_task(void) {
    static StaticTask_t task_buffer                          = { 0 };
    static StackType_t task_stack[SERIAL_RX_TASK_STACK_SIZE] = { 0 };

    task_create_static(&obc_serial_rx_task, "obc_serial_rx", SERIAL_RX_TASK_STACK_SIZE, NULL, SERIAL_RX_TASK_PRIORITY, task_stack, &task_buffer);
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
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    static uint8_t data_buf[OBC_SERIAL_FRAME_MAX_DATA_SIZE] = { 0 };

    obc_serial_rx_state_t state = OBC_SERIAL_RX_STATE_SYNC_0;
    uint8_t data_len = 0;
    uint8_t crc_buf[2] = { 0 };

    while (1) {
        // Parse a serial frame to extract/validate the data field (which contains a datagram)
        switch (state) {
            case OBC_SERIAL_RX_STATE_SYNC_0:
                set_task_status(wd_task_id, task_asleep);
                xStreamBufferSetTriggerLevel(phy_stream_buffer, 1);
                if (xStreamBufferReceive(phy_stream_buffer, data_buf, 1, portMAX_DELAY) == 1) {
                    set_task_status(wd_task_id, task_alive);
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
                    // TODO ALEA-842 check CRC
                    // TODO ALEA-843 log error if false returned from handle_datagram
                    handle_datagram(data_buf, data_len, portMAX_DELAY);
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
    switch (type) {
        case OBC_SERIAL_DATAGRAM_COMMS:
            success = (xMessageBufferSend(comms_msg_buffer, &datagram_buf[1], data_len, timeout_ticks) == data_len);
            break;

        default:
            success = false;
            break;
    }
    return success;
}
