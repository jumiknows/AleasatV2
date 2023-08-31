/**
 * @file gps_internals.c
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// GPS
#include "gps_defs.h"

// OBC
#include "obc_hardwaredefs.h"

// std lib
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Calculates the checksum of a GPS message
 *
 * @see Page 4.
 *
 * @param msg GPS message including MSG ID.
 * @param size length of GPS message.
 * @return uint8_t XOR checksum bit.
 */
uint8_t calculate_ctrl_msg_checksum(const uint8_t* msg, const uint8_t size) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < size; i++) {
        checksum = checksum ^ msg[i];
    }
    return checksum;
}

/**
 * @brief Checks that the msg buffer has a valid GPS message.
 *
 * @param[in] msg: pointer to message buffer. If the  packet is malformed it will likely fail since it assumes it follows the GPS control msg structure.
 * @param[in] size: length of message buffer.
 * @return bool: True if checksum is valid.
 */
bool is_pkts_checksum_valid(const uint8_t* msg, const uint16_t size) {
    uint16_t payload_length = (uint16_t)((uint16_t)msg[GPS_CMD_LEN_POS_0] << 8) | msg[GPS_CMD_LEN_POS_1];
    uint8_t packet_checksum = msg[GPS_CMD_PAYLOAD_START + payload_length];
    uint8_t calculated_ctrl_msg_checksum = calculate_ctrl_msg_checksum(msg + GPS_CMD_PAYLOAD_START,  payload_length);
    return calculated_ctrl_msg_checksum == packet_checksum;
}

/**
 * @brief Parse control packet.
 *
 * This could be in the form of a single ACK/NACK packet or in the case of extended replied it could be a variable length reply.
 *
 * @param[in] data
 * @param[in] data_len
 * @param[out] packet
 * @return gps_parsing_err_t
 */
gps_parsing_err_t parse_control_packet(const uint8_t* data, const uint8_t data_len, gps_packet_t* packet) {
    if (data_len < GPS_ACK_PKT_SIZE) {
        packet->id  = 0;
        packet->len = 0;
        packet->ack = false;
        return GPS_PARSE_TOO_SHORT;
    }

    if (data_len == GPS_ACK_PKT_SIZE) {
        // This data should contain an ACK+Reply, if we only have an ACK/NACK then lets check the ID.
        packet->ack = (data[GPS_CMD_ID_POS] == GPS_SYS_MSG_ACK);
        packet->len = 0;
        packet->id  = data[GPS_CMD_ID_POS];
        return GPS_PARSE_OK;
    }

    // If we got here then we probably

    // Extract the packet data from the input array
    uint16_t payload_len = (uint16_t)((uint16_t)data[GPS_CMD_LEN_POS_0] << 8) | data[GPS_CMD_LEN_POS_1];
    if (payload_len > packet->payload_len) {
        packet->ack = false;
        packet->len = 0;
        packet->id  = data[GPS_CMD_ID_POS];
        return GPS_PARSE_MAX_SIZE;
    }

    // -1 Because body_len contains the Message ID.
    packet->ack = true;
    packet->len = payload_len-1;
    packet->id  = data[GPS_CMD_ID_POS];
    // +2 because payload is composed of ID + payload.
    memcpy(packet->payload, data + GPS_CMD_PAYLOAD_START + 1, payload_len);

    return GPS_PARSE_OK;
}

/**
 * @brief Checks if the msg is an ack or nack packet.
 *
 * If the msg is marlphormed it will reply with nack.
 *
 * @param msg
 * @param data_len
 *
 * @return gps_sys_msg_output_t
 */
gps_sys_msg_output_t get_packet_type(const uint8_t* msg, const uint16_t data_len) {
    if (data_len < GPS_ACK_PKT_SIZE) {
        return GPS_SYS_MSG_NACK;
    }
    if (msg[GPS_CMD_ID_POS] == GPS_SYS_MSG_ACK) {
        return GPS_SYS_MSG_ACK;
    }
    if (msg[GPS_CMD_ID_POS] == GPS_SYS_MSG_NACK) {
        return GPS_SYS_MSG_NACK;
    }
    return GPS_SYS_MSG_PAYLOAD;
}


/**
 * @brief Checks if the msg is an ack packet.
 *
 * If the msg is marlphormed it will reply with false
 *
 * @param msg
 * @param data_len
 *
 * @return bool: true if its an ack packet.
 */
bool is_ack_pkt(const uint8_t* msg, const uint16_t data_len) {
    if (data_len < GPS_ACK_PKT_SIZE) {
        return false;
    }
    return msg[GPS_CMD_ID_POS] == GPS_SYS_MSG_ACK;
}
