/**
 * @file gps_serial_rx.c
 * @brief GPS serial rx driver
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "gps_serial_rx.h"

// OBC
#include "obc_watchdog.h"
#include "obc_rtos.h"
#include "obc_hardwaredefs.h"
#include "obc_gps.h"
#include "gps_defs.h"
#include "logger.h"

// Utils
#include "rtos_stream.h"
#include "buffered_io.h"

// Logger
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// HALCoGen
#include "sci.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define GPS_STREAM_BUFFER_SIZE         255U
#define GPS_STREAM_BUFFER_DEF_TRIG_LVL 1U

#define GPS_SERIAL_FRAME_MAX_DATA_SIZE    ((uint8_t)234U)
#define GPS_SERIAL_DATAGRAM_MAX_DATA_SIZE ((uint8_t)233U)

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    GPS_SERIAL_RX_STATE_START_SYNC_0,
    GPS_SERIAL_RX_STATE_START_SYNC_1,
    GPS_SERIAL_RX_STATE_LENGTH,
    GPS_SERIAL_RX_STATE_LENGTH_0,
    GPS_SERIAL_RX_STATE_LENGTH_1,
    GPS_SERIAL_RX_STATE_MSG_ID,
    GPS_SERIAL_RX_STATE_DATA,
    GPS_SERIAL_RX_STATE_CRC,
    GPS_SERIAL_RX_STATE_END_SYNC_0,
    GPS_SERIAL_RX_STATE_END_SYNC_1,
    GPS_NMEA_MSG,
} gps_serial_rx_state_t;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void gps_serial_rx_task(void *pvParameters);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

// FreeRTOS objects
static StreamBufferHandle_t gps_stream_buffer = NULL;

// Data
static uint8_t gps_rx_byte;

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize FreeRTOS data structures for the OBC serial RX
 */
void gps_serial_rx_pre_init(void) {
    // Stream buffer for physical serial port
    static StaticStreamBuffer_t gps_stream_buffer_buf                    = {0};
    static uint8_t gps_stream_buffer_storage[GPS_STREAM_BUFFER_SIZE + 1] = {0}; // See https://www.freertos.org/xStreamBufferCreateStatic.html for explanation of + 1

    gps_stream_buffer = xStreamBufferCreateStatic(GPS_STREAM_BUFFER_SIZE, GPS_STREAM_BUFFER_DEF_TRIG_LVL, gps_stream_buffer_storage,
                        &gps_stream_buffer_buf);

    obc_rtos_create_task(OBC_TASK_ID_GPS_SERIAL_RX, &gps_serial_rx_task, NULL, OBC_WATCHDOG_ACTION_ALLOW);
}

/**
 * @brief Enable interrupts in post-init section
 */
void gps_serial_rx_post_init(void) {
    gps_serial_rx_init_irq();
}

/**
 * @brief Configure and enable interrupts for the OBC serial RX
 */
void gps_serial_rx_init_irq(void) {
    sciEnableNotification(UART_GPS, SCI_RX_INT);

    // This HALCoGen SCI driver has a built-in buffering mechanism so that multiple bytes can be
    // collected before a call to sciNotification. Since we need to pass the data from the ISR to
    // a task anyway (to avoid dropping bytes while processing a message), we'll receive one byte
    // at a time.
    //
    // When sciNotification fires, rx_byte will contain the new byte.
    // We will have to call sciReceive again to receive the next byte.
    sciReceive(UART_GPS, 1, &gps_rx_byte);
}

/**
 * @brief ISR handler for RX events on the OBC serial port
 *
 * @param pxHigherPriorityTaskWoken Pointer to store flag that a higher priority task was woken
 */
void gps_serial_rx_isr(BaseType_t *pxHigherPriorityTaskWoken) {
    xStreamBufferSendFromISR(gps_stream_buffer, &gps_rx_byte, 1, pxHigherPriorityTaskWoken);
    sciReceive(UART_GPS, 1, &gps_rx_byte);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Handle GPS and dispatch GPS messages.
 *
 * The GPS can generate one of two messages.
 *
 * 1. NMEA sentences which begin with '$' (GPS_NMEA_MSG_START) and ends with the GPS_MSG_END_SEQ_0,GPS_MSG_END_SEQ_1.
 * 2. GPS replies which are composed of up to two packets. Both compliying with the 2 start/stop bits.
 * a) ACK/Nack pkt (see page 136) with ID 0x83/0x84 (GPS_SYS_MSG_ACK/_NACK) and the ID of the ACK'd sent msg.
 * b) Reply pkt.
 * If a) is a NACK packet then b will not follow.
 */
static void gps_serial_rx_task(void *pvParameters) {
    TickType_t timeout = pdMS_TO_TICKS(GPS_REPLY_TIMEOUT_MS);
    static uint8_t data_buf[GPS_SERIAL_FRAME_MAX_DATA_SIZE] = {0};
    uint8_t rx_char                                         = 0;
    uint8_t buf_idx                                         = 0;
    uint16_t payload_len                                    = 0;

    gps_serial_rx_state_t state = GPS_SERIAL_RX_STATE_START_SYNC_0;

    while (1) {
        obc_watchdog_pet(OBC_TASK_ID_GPS_SERIAL_RX);

        switch (state) {
        case GPS_NMEA_MSG:
            xStreamBufferSetTriggerLevel(gps_stream_buffer, 1);

            if (xStreamBufferReceive(gps_stream_buffer, &rx_char, 1, timeout) == 1) {
                if (buf_idx >= sizeof(data_buf)) {
                    LOG_GPS__NMEA_PARSER_OVERFLOW();
                    state = GPS_SERIAL_RX_STATE_START_SYNC_0;
                    break;
                }

                data_buf[buf_idx] = rx_char;

                if (rx_char == GPS_MSG_END_SEQ_1) {
                    // We need to check for the sequence
                    if (buf_idx > 0) {
                        if (data_buf[buf_idx - 1] == GPS_MSG_END_SEQ_0) {
                            // TODO(ALEA-165): send this to NMEA parser.
                            state = GPS_SERIAL_RX_STATE_START_SYNC_0;
                        }
                    }
                }

                buf_idx++;
            }

            break;

        case GPS_SERIAL_RX_STATE_START_SYNC_0:
            buf_idx     = 0;
            rx_char     = 0;
            payload_len = 0;
            xStreamBufferSetTriggerLevel(gps_stream_buffer, 1);

            if (xStreamBufferReceive(gps_stream_buffer, &rx_char, 1, timeout) == 1) {
                // Check if its NMEA or GPS msg.
                if (rx_char == GPS_NMEA_MSG_START) {
                    state = GPS_NMEA_MSG;
                } else if (rx_char == GPS_MSG_START_SEQ_0) {
                    state = GPS_SERIAL_RX_STATE_START_SYNC_1;
                }

                data_buf[buf_idx] = rx_char;
                buf_idx++;
            }

            break;

        case GPS_SERIAL_RX_STATE_START_SYNC_1:
            xStreamBufferSetTriggerLevel(gps_stream_buffer, 1);

            if (xStreamBufferReceive(gps_stream_buffer, &rx_char, 1, timeout) == 1) {
                if (rx_char == GPS_MSG_START_SEQ_1) {
                    state = GPS_SERIAL_RX_STATE_LENGTH;
                } else {
                    state = GPS_SERIAL_RX_STATE_START_SYNC_0;
                }

                data_buf[buf_idx] = rx_char;
                buf_idx++;
            }

            break;

        case GPS_SERIAL_RX_STATE_LENGTH:
            xStreamBufferSetTriggerLevel(gps_stream_buffer, 1);

            if (xStreamBufferReceive(gps_stream_buffer, &rx_char, 1, timeout) == 1) {
                payload_len       = (uint16_t)rx_char << 8;
                data_buf[buf_idx] = rx_char;
                buf_idx++;
            }

            xStreamBufferSetTriggerLevel(gps_stream_buffer, 1);

            if (xStreamBufferReceive(gps_stream_buffer, &rx_char, 1, timeout) == 1) {
                state             = GPS_SERIAL_RX_STATE_LENGTH_1;
                payload_len       = (payload_len | (uint16_t)((uint16_t)rx_char << 0));

                if (payload_len > GPS_SERIAL_FRAME_MAX_DATA_SIZE) {
                    LOG_GPS__PACKET_PARSER_OVERFLOW(payload_len);
                    state = GPS_SERIAL_RX_STATE_START_SYNC_0;
                } else {
                    data_buf[buf_idx] = rx_char;
                    buf_idx++;
                    state = GPS_SERIAL_RX_STATE_DATA;
                }
            }

            break;

        case GPS_SERIAL_RX_STATE_DATA:
            for (int i = 0; i < (payload_len + 3); i++) {
                xStreamBufferSetTriggerLevel(gps_stream_buffer, 1);

                if (xStreamBufferReceive(gps_stream_buffer, &rx_char, 1, timeout) == 1) {
                    data_buf[buf_idx] = rx_char;
                    buf_idx++;
                }
            }

            gps_handle_control_msg((uint8_t *)&data_buf, buf_idx);
            state = GPS_SERIAL_RX_STATE_START_SYNC_0;
            break;

        default:
            state = GPS_SERIAL_RX_STATE_START_SYNC_0;
            break;
        }
    }
}
