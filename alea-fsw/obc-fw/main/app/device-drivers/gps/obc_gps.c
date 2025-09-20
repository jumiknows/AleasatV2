/**
 * @file obc_gps.c
 * @brief Venus838LPx-T-L-1738D01 GPS module driver
 * @see  https://drive.google.com/file/d/1yHMMh7UOVmpdzGp5utmuKuDXSUNol7ZH/view?usp=sharing
 *
 * TODO(ALEA-165) confirm if we have the L or D version for flatsat.
 *
 * Message Format of a GPS message.
 *
 * Start of Sequence:  2 bytes (0xA0, 0xA1).
 * Payload Length:     2 bytes.
 * Message ID:         1 Byte.
 * Message Body: up to 65535 Bytes.
 * Checksum:           1 Byte.
 * End of sequence:    2 Bytes. (0x0D, 0x0A).
 *
 * A Message Body may further consist of 2 sub-fields, Sub-Message ID (Sub ID)  and Sub-Message Body.
 * Sub-Message ID:     1 Byte.
 * Sub-Message Body:   65534 Bytes.
 *
 * The checksum is the 8-bit exclusive OR of only the payload bytes which start from Message IS until the checksum byte.
 *
 * The message is transferred in big-endian format. The high order byte is transmitted first.
 */

/**
 * Message Flow
 * The GPS can recieve messages from a host. In this case the OBC.
 * Upon receiving a successful input message, the GPS will first ACK the request and then reply to the message.
 * 1. GPS <- MSG -- OBC
 * 2. GPS -- ACK -> OBC
 * 3. GPS -- RPL -> OBC
 *
 * Upon receiving an uncessful input message, the GPS will NACK the request.
 * 1. GPS <- MSG --- OBC
 * 2. GPS -- NACK -> OBC
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_gps.h"
#include "gps_internals.h"

// OBC
#include "obc_serial_tx.h"
#include "gps_serial_rx.h"
#include "obc_time.h"

// Logger
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// Utils
#include "io_stream.h"

// std lib
#include <stdint.h>
#include <stdbool.h>

// HALCoGen
#include "sci.h"

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

int16_t gps_send_and_get_reply(const uint8_t *payload, uint32_t payload_len, bool expect_extended_reply, gps_packet_t *reply_pkt);
gps_err_t handle_gps_response(int32_t reply_len, gps_packet_t *packet);
void send_gps_datagram(const uint8_t *payload, uint8_t payload_len);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static gps_state_t gps_state;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void gps_pre_init(void) {
    static StaticSemaphore_t sci_mutex_buf = {0};
    gps_state.xGpsMutex                    = xSemaphoreCreateMutexStatic(&sci_mutex_buf);

    gps_state.xGpsTaskToNotify      = NULL;
    gps_state.expect_extended_reply = false;
    gps_state.success               = false;
    gps_state.rx_buf_len            = 0;
}

/**
 * @brief parses and handles the GPS control message.
 *
 * This can get called up to twice per command. A reply is one of two:
 * - An extended Reply that contains an ACK and then a second control message of variable length.
 * - A single ACK/NACK. If a command that expects an extended reply recieves a NACK  then there will be no second packet.
 *
 * This method knows if it should expect an extended reply though the GPS State.
 *
 * @param[in] buff: a char sequence representing a command. Must conform to the GPS msg format.
 * @param[in] length: length size of the buffer.
 *
 * @returns gps_err_t
 */
gps_err_t gps_handle_control_msg(uint8_t *buf, uint16_t length) {
    if (!is_pkts_checksum_valid(buf, length) || length > MAX_GPS_LEN_BYTES) {
        gps_state.expect_extended_reply = false;
        xTaskNotifyGive(gps_state.xGpsTaskToNotify);
        LOG_GPS__INVALID_CHECKSUM_CONTROL_MSG();
        return GPS_BAD_RESPONSE;
    }

    // If we got here pkt is fairly valid, so lets notify the thread waiting on the reply.
    /**
     * Wait! We just need one more....
     *  ______     ______     ______   __     ______
     * /\  ___\   /\  __ \   /\  == \ /\ \   /\  ___\
     * \ \ \____  \ \ \/\ \  \ \  _-/ \ \ \  \ \  __\
     *  \ \_____\  \ \_____\  \ \_\    \ \_\  \ \_____\
     *   \/_____/   \/_____/   \/_/     \/_/   \/_____/
     */

    if (gps_state.expect_extended_reply == true) {
        // We should check to see if its an ACK/NACK.
        gps_sys_msg_output_t packet_type = get_packet_type(buf, length);

        switch (packet_type) {
        case GPS_SYS_MSG_ACK:
            // Were going to ingnore the ACK and not append it to the buffer.
            gps_state.expect_extended_reply = false;
            break;

        case GPS_SYS_MSG_NACK:
            // We not a nack when were expecting an extended response.
            gps_state.expect_extended_reply = false;
            gps_state.success               = false;
            xTaskNotifyGive(gps_state.xGpsTaskToNotify);
            return GPS_BAD_RESPONSE;

        default:
            break;
        }
    } else {
        memcpy(gps_state.rx_buf, buf, length);
        gps_state.rx_buf_len = length;
        gps_state.success    = true;
        xTaskNotifyGive(gps_state.xGpsTaskToNotify);
    }

    return GPS_SUCCESS;
}

// Command Implementation

/**
 * @brief Sends the GPS message which will reset and restart the GNSS receiver.
 *
 * There are three types of start types:
 * HOT_START: Its when the GPS device remembers its last calculated postion
 *            and the satellites in view, the almanac used, the UTC time and
 *            tries to lock into the same satellites to lock a position.
 *
 * WARM_START: Its when the GPS device remembers its last calculated position,
 *             almanac used, and UTC time, but not which satellites were in view.
 *
 * COLD_START: Its when the GPS device dumps all the information, and attempts
 *             to locate satellites and the ncalculates a GPS lock.
 */
gps_err_t gps_restart_receiver(gps_restart_modes_t restart_mode) {
    // We need to get the time time on the satellite.
    real_time_t curr_time = {0};
    rtc_err_t err         = rtc_get_current_time(&curr_time);

    if (err != RTC_SUCCESS) {
        return GPS_RTC_ERROR;
    }

    uint8_t year0  = ((curr_time.year + START_CENTURY) >> (8 * 0)) & 0xff;
    uint8_t year1  = ((curr_time.year + START_CENTURY) >> (8 * 1)) & 0xff;
    uint8_t month  = curr_time.month & 0xff;
    uint8_t day    = curr_time.month & 0xff;
    uint8_t hour   = curr_time.hour & 0xff;
    uint8_t minute = curr_time.minute & 0xff;
    uint8_t second = curr_time.second & 0xff;
    int8_t lat0    = (GPS_HENN_LAT >> (8 * 0)) & 0xff;
    int8_t lat1    = (GPS_HENN_LAT >> (8 * 1)) & 0xff;
    int8_t lon0    = (GPS_HENN_LON >> (8 * 0)) & 0xff;
    int8_t lon1    = (GPS_HENN_LON >> (8 * 1)) & 0xff;
    int8_t alt0    = (GPS_HENN_ALT >> (8 * 0)) & 0xff;
    int8_t alt1    = (GPS_HENN_ALT >> (8 * 1)) & 0xff;

    uint8_t payload[15] = {GPS_SYS_CMD_RESART, restart_mode, year0, year1, month, day, hour, minute, second, lat0, lat1, lon0, lon1, alt0, alt1};

    // TODO CREATE FUNC TO ADJUST ORCATIME TO NORMAL TIME.
    uint8_t reply_buf[2]   = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 2};
    memset(reply_pkt.payload, 0, sizeof(reply_pkt.payload));
    int16_t reply_len = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), false, &reply_pkt);
    return handle_gps_response(reply_len, &reply_pkt);
}

/**
 * @brief Queries the software version of the GPS.
 *
 * Will get the software version.
 * Kernel Version: X1,Y1,Z1
 * ODM Version: X1,Y1,Z1
 * Revision: YY,MM,DD
 * @see Page #14 for Query. Page #134 for Reply.
 *
 * @param[out] sw_version
 * @return gps_err_t
 */
gps_err_t gps_query_software_version(uint8_t *sw_version) {
    uint8_t payload[2] = {
        GPS_SYS_CMD_QUERY_SW_VER,
        0x01, // System Code
    };
    uint8_t reply_buf[14]  = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 14};
    int16_t reply_len      = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), true, &reply_pkt);
    gps_err_t status       = handle_gps_response(reply_len, &reply_pkt);

    if (status != GPS_SUCCESS) {
        return status;
    }

    if (reply_pkt.len < 13) {
        return GPS_BAD_RESPONSE;
    }

    // Interpretation on page 134.
    sw_version[0] = reply_pkt.payload[2];  // Kernel.
    sw_version[1] = reply_pkt.payload[3];  // Kernel.
    sw_version[2] = reply_pkt.payload[4];  // Kernel.
    sw_version[3] = reply_pkt.payload[5];  // ODM.
    sw_version[4] = reply_pkt.payload[6];  // ODM.
    sw_version[5] = reply_pkt.payload[7];  // ODM.
    sw_version[6] = reply_pkt.payload[10]; // Rev.
    sw_version[7] = reply_pkt.payload[11]; // Rev.
    sw_version[8] = reply_pkt.payload[12]; // Rev.

    return GPS_SUCCESS;
}

/**
 * @brief Set Factory Defaults
 *
 * @see Query on page #16, Reply is an ACK.
 */
gps_err_t gps_set_factory_settings(void) {
    uint8_t payload[2] = {
        GPS_SYS_CMD_SET_FACTORY_DEFAULT, 0x01 // System Code
    };
    uint8_t reply_buf[2]   = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 2};
    memset(reply_pkt.payload, 0, 0);
    int16_t reply_len = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), false, &reply_pkt);
    return handle_gps_response(reply_len, &reply_pkt);
}

/**
 * @brief Returns the CRC of the GPS software version.
 *
 * @see Page #15 for Query, Page #135 for Reply.
 * @return gps_err_t
 */
gps_err_t gps_query_crc_info(uint16_t *sw_crc) {
    uint8_t payload[2]     = {GPS_SYS_CMD_QUERY_SW_CRC, 0x01};
    uint8_t reply_buf[4]   = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 4};
    int16_t reply_len      = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), true, &reply_pkt);
    gps_err_t status       = handle_gps_response(reply_len, &reply_pkt);

    if (status != GPS_SUCCESS) {
        return status;
    }

    if (reply_pkt.len < 2) {
        return GPS_BAD_RESPONSE;
    }

    *sw_crc = (uint16_t)((uint16_t)reply_pkt.payload[0] << 8) | (uint16_t)reply_pkt.payload[1];
    return GPS_SUCCESS;
}

/**
 * @brief Configure the Serial COM port baud rate.
 * @see Query in page #17
 *
 * @param[in] baud_rate
 * @return gps_err_t
 */
gps_err_t gps_configure_serial_port(gps_config_serial_baud_t baud_rate) {
    uint8_t payload[4]     = {GPS_SYS_CMD_SET_SERIAL_PORT, 0x00, baud_rate, 0x01};
    uint8_t reply_buf[2]   = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 2};
    int16_t reply_len      = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), false, &reply_pkt);
    return handle_gps_response(reply_len, &reply_pkt);
}

/**
 * @brief Configure NMEA message configuration.
 * @see Query in page #18, Reply ACK.
 *
 * Each argument is a message interval [0,255], 0 means disabling the message. Time is in seconds.
 *
 * @param gga
 * @param gsa
 * @param gsv
 * @param gll
 * @param rmc
 * @param vtg
 * @param zda
 * @return gps_err_t
 */
gps_err_t gps_configure_nmea_messages(uint8_t gga, uint8_t gsa, uint8_t gsv, uint8_t gll, uint8_t rmc, uint8_t vtg, uint8_t zda) {
    uint8_t payload[9]     = {GPS_SYS_CMD_SET_NMEA, gga, gsa, gsv, gll, rmc, vtg, zda, 0x01};
    uint8_t reply_buf[2]   = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 2};
    int16_t reply_len      = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), false, &reply_pkt);
    return handle_gps_response(reply_len, &reply_pkt);
}

/**
 * @brief Query NMEA message configuration.
 * @see Query Page #69 nice. Reply #95.
 */
gps_err_t gps_query_nmea_messages(uint8_t *gga, uint8_t *gsa, uint8_t *gsv, uint8_t *gll, uint8_t *rmc, uint8_t *vtg, uint8_t *zda) {
    // GPS is in a bad state after this.
    uint8_t payload[2]     = {GPS_SYS_CMD_QUERY_NMEA, 0x03};
    uint8_t reply_buf[14]  = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 14};
    int16_t reply_len      = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), true, &reply_pkt);
    gps_err_t status       = handle_gps_response(reply_len, &reply_pkt);

    if (status != GPS_SUCCESS) {
        return status;
    }

    if (reply_pkt.len < 12) {
        return GPS_BAD_RESPONSE;
    }

    *gga = reply_pkt.payload[1];
    *gsa = reply_pkt.payload[2];
    *gsv = reply_pkt.payload[3];
    *gll = reply_pkt.payload[4];
    *rmc = reply_pkt.payload[5];
    *vtg = reply_pkt.payload[6];
    *zda = reply_pkt.payload[7];

    return GPS_SUCCESS;
}

/**
 * @brief GPS configure power mode.
 * @see Query Page #21, Reply is ACK.
 *
 * @param[in] power_save, 0x00 mans power save is disabled.
 * @return gps_err_t
 */
gps_err_t gps_configure_power_mode(gps_write_method_t gps_write_method, bool power_save) {
    uint8_t pwr_mode = 0x00;

    if (power_save == true) {
        pwr_mode = 0x01;
    }

    uint8_t payload[3]     = {GPS_SYS_CMD_SET_PWR_MODE, pwr_mode, (uint8_t)gps_write_method};
    uint8_t reply_buf[2]   = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 2};
    int16_t reply_len      = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), false, &reply_pkt);
    return handle_gps_response(reply_len, &reply_pkt);
}

/**
 * @brief Query GPS power status.
 * @see Query on page #25, Reply on page #154.
 *
 * @param[out] power_save
 * @return gps_err_t
 */
gps_err_t gps_query_power_mode(bool *power_save) {
    uint8_t payload[1]     = {GPS_SYS_CMD_QUERY_PWR_MODE};
    uint8_t reply_buf[4]   = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 4};
    int16_t reply_len      = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), true, &reply_pkt);
    gps_err_t status       = handle_gps_response(reply_len, &reply_pkt);

    if (status == GPS_SUCCESS) {
        *power_save = (reply_pkt.payload[0] == (uint8_t)0x01);
    }

    return status;
}

/**
 * @brief
 * @note From page 22 of application doc "value with 4~10 should work with
 *       baud rate 38400 or higher, value with 20 should work with baud rate 115200 or
 *       higher, value with 40, 50 should work with 230400".
 * @see page #138
 *
 * @param[in] pos_update_rate
 * @return gps_err_t
 */
gps_err_t gps_configure_system_postion_rate(gps_pos_update_rate_t pos_update_rate) {
    // Validate the settings.

    // TODO(ALEA-165): Add switch  after  adding the capabilities to change the OBC's GPS baud rate.
    // switch (pos_update_rate) {
    //     case GPS_POS_UPDATE_1HZ:
    //     case GPS_POS_UPDATE_2HZ:
    //         // Assume all baud rates are good before 4Hz.
    //         break;
    //     case GPS_POS_UPDATE_4HZ:
    //     case GPS_POS_UPDATE_8HZ:
    //     case GPS_POS_UPDATE_10HZ:
    //         if (GPS_BAUD_RATE < GPS_38400_BAUD) {
    //             return GPS_BAD_ARGUMENTS;
    //         }
    //         break;
    //     case GPS_POS_UPDATE_20HZ:
    //     case GPS_POS_UPDATE_25HZ:
    //         // I'm totally guessing which is the lowest acceptable caud rate for 25Hz.
    //         if (GPS_BAUD_RATE < GPS_115200_BAUD) {
    //             return GPS_BAD_ARGUMENTS;
    //         }
    //         break;
    //     case GPS_POS_UPDATE_40HZ:
    //     case GPS_POS_UPDATE_50HZ:
    //         if (GPS_BAUD_RATE < GPS_230400_BAUD) {
    //             return GPS_BAD_ARGUMENTS;
    //         }
    //         break;
    //     default:
    //         // NADA!
    //         break;
    // }

    uint8_t payload[3]     = {GPS_SYS_CMD_SET_POS_UPDATE_RATE, pos_update_rate, 0x01};
    uint8_t reply_buf[10]  = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 10};
    int16_t reply_len      = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), false, &reply_pkt);
    return handle_gps_response(reply_len, &reply_pkt);
}

/**
 * @brief Query position update rate.
 *
 * @see Query page #23. Reply page 138.
 *
 * @param[out] pos_update_rate: returns the position update rate of the GNSS system.
 */
gps_err_t gps_query_system_postion_rate(uint8_t *pos_update_rate) {
    uint8_t payload[1] = {GPS_SYS_CMD_QUERY_POS_UPDATE_RATE};

    uint8_t reply_buf[2]   = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 2};
    int16_t reply_len      = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), true, &reply_pkt);
    gps_err_t status       = handle_gps_response(reply_len, &reply_pkt);

    if (status == GPS_SUCCESS) {
        *pos_update_rate = reply_pkt.payload[0];
    }

    return status;
}

/**
 * @brief Configure binary navifation data message interval.
 * @see Msg on page #24. Reply is Ack.
 * @param[in] gps_write_method: update rate in seconds. [0:255]
 *                          0 is disabled.
 * @param[in] nav_msg_interval: update rate in seconds. [0:255]
 *                          0 is disabled.
 * @return gps_err_t
 */
gps_err_t gps_configure_nav_message_interval(gps_write_method_t gps_write_method, uint8_t nav_msg_interval) {
    uint8_t payload[3]     = {GPS_SYS_CMD_SET_NAV_DATA_MSG_INTER, nav_msg_interval, (int)gps_write_method};
    uint8_t reply_buf[2]   = {0};
    gps_packet_t reply_pkt = {false, 0, 0, (uint8_t *) &reply_buf, 2};
    int16_t reply_len      = gps_send_and_get_reply((uint8_t *)payload, sizeof(payload), false, &reply_pkt);
    return handle_gps_response(reply_len, &reply_pkt);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

gps_err_t handle_gps_response(int32_t reply_len, gps_packet_t *packet) {
    if (reply_len == -1) {
        return GPS_MSG_TIMEOUT;
    }

    if (reply_len == GPS_MUTEX_TIMEOUT) {
        return GPS_MUTEX_TIMEOUT;
    }

    if (packet->ack == false) {
        return GPS_RECV_NACK;
    }

    return GPS_SUCCESS;
}

/**
 * @brief Sends a message to the GPS over UART. Then blocks untill it gets a reply.
 *
 * @see https://www.freertos.org/RTOS_Task_Notification_As_Binary_Semaphore.html
 *
 * @param send_data
 * @param send_data_size
 * @param recv_data
 * @return int16_t: returns the length of the recv data.
 *                  or a  gps_parsing_err_t.
 */
int16_t gps_send_and_get_reply(const uint8_t *payload, uint32_t payload_len, bool expect_extended_reply, gps_packet_t *reply_pkt) {
    const TickType_t xBlockTime = pdMS_TO_TICKS(GPS_REPLY_TIMEOUT_MS);
    int16_t return_val          = GPS_INTERNAL_ERROR;

    if (xSemaphoreTake(gps_state.xGpsMutex, xBlockTime) == pdPASS) {
        gps_state.expect_extended_reply = expect_extended_reply;
        gps_state.rx_buf_len            = 0;
        gps_state.xGpsTaskToNotify      = xTaskGetCurrentTaskHandle();
        gps_state.success               = false;
        send_gps_datagram(payload, payload_len);
        uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (ulNotificationValue == 1) {
            if (gps_state.success) {
                gps_parsing_err_t gps_err = parse_control_packet((uint8_t *)&gps_state.rx_buf, gps_state.rx_buf_len, reply_pkt);

                if (gps_err == GPS_PARSE_OK) {
                    return_val = reply_pkt->len;
                } else {
                    xSemaphoreGive(gps_state.xGpsMutex);
                    return gps_err;
                }
            }
        }

        xSemaphoreGive(gps_state.xGpsMutex);
    } else {
        return GPS_MUTEX_TIMEOUT;
    }

    return return_val;
}

void send_gps_datagram(const uint8_t *payload, uint8_t payload_len) {
    uint8_t header[2] = {
        GPS_MSG_START_SEQ_0,
        GPS_MSG_START_SEQ_1,
    };
    uint8_t tail[2] = {
        GPS_MSG_END_SEQ_0,
        GPS_MSG_END_SEQ_1,
    };
    uint8_t len[2] = {
        0x00,
        payload_len,
    };

    uint8_t checksum = calculate_ctrl_msg_checksum((uint8_t *)payload, payload_len);

    // Send header
    sciSend(UART_GPS, sizeof(header), header);

    // Send length
    sciSend(UART_GPS, sizeof(len), len);

    // Send payload
    // Disable MISRA warning about the cast to non-const
    sciSend(UART_GPS, payload_len, (uint8_t *)payload); // HALCoGen doesn't declare the data argument as const despite the fact that it does not modify it

    // Send CRC
    sciSend(UART_GPS, 1, (uint8_t *)&checksum);

    // Send Tail
    sciSend(UART_GPS, sizeof(tail), tail);
}
