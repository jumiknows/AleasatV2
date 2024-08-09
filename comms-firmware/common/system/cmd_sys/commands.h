// OpenLST
// Copyright (C) 2018 Planet Labs Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
/**
 * @file commands.h
 * @brief Configuration file for command handler baseband serial communications
 * @author Timothy Wriglesworth
 * @date May 11, 2021
 */

#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <stdint.h>
#include "hwid.h"
#include "uart.h"

typedef enum {
    common_msg_ack = 0x10,
    common_msg_nack = 0xff,
    common_msg_ascii = 0x11,
    custom_alea_ack = 0xee
} common_msg_no;

/**
 * @brief Correctly-sized container for radio
 * op-code enum value, needed when packing/unpacking data
 */
typedef uint8_t radio_msg_no_t;

/**
 * @brief Command Header prepended to packet payloads
 * See user guide for data fields documentation
 */
typedef struct command_header {
    uint16_t seqnum_and_resp;
    hwid_t dest_hwid;
    hwid_t src_hwid;
    radio_msg_no_t cmd_num;
} command_header_t;

inline uint16_t get_seqnum(uint16_t seqnum_and_resp) {
    return seqnum_and_resp & 0x7FFF;
}

inline __bit get_resp(uint16_t seqnum_and_resp) {
    return seqnum_and_resp >= 0x8000;
}


/*
 * These values are influenced by constants in radio.h
 * Change these defines when adjusting MAC_FIELD_LEN
 */
#define LOCAL_PACKET_MAX_LEN    233
#define LOCAL_PACKET_DATA_MAX_NUM_BYTES    226
#define LOCAL_PACKET_MAX_NUM_BYTES    (1 + LOCAL_PACKET_MAX_LEN)


/**
 * @brief Command message struct, provided only as convenience for accessing uint8_t buffers
 */
typedef struct command {
    command_header_t header;
    uint8_t data[LOCAL_PACKET_DATA_MAX_NUM_BYTES];
} command_t;

/**
 * @brief Store command messages in a correctly sized buffer for read/write access
 */
typedef union command_buffer {
    uint8_t msg_bytes[LOCAL_PACKET_MAX_LEN];
    command_t cmd;
} command_buffer_t;

uint8_t commands_handle_command(const __xdata command_t *cmd, uint8_t len, __xdata command_t *reply);

#endif
