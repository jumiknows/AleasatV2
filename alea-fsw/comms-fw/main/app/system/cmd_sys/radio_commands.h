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

// Command message opcodes
#ifndef _MESSAGES_H
#define _MESSAGES_H

#include <stdint.h>
#include "timers.h"
#include "telemetry.h"
#include "radiotelem.h"
#include "aes.h"

/**
 * @brief  These opcodes correspond to
 * the CMD field in the packet structure
 */
typedef enum {
    radio_msg_reboot       = 0x12,
    radio_msg_get_time     = 0x13,
    radio_msg_set_time     = 0x14,
    radio_msg_ranging      = 0x15,
    radio_msg_ranging_ack  = 0x16,
    radio_msg_get_telem    = 0x17,
    radio_msg_telem        = 0x18,
    radio_msg_get_callsign = 0x19,
    radio_msg_set_callsign = 0x1a,
    radio_msg_callsign     = 0x1b,
    radio_msg_start        = 0x1e,
    radio_msg_rebooting    = 0x1f,
    alea_radio_msg_get_radiotelem     = 0x21,
    alea_radio_msg_radiotelem         = 0x22,
    alea_radio_msg_set_radiotelem     = 0x23,
    alea_radio_msg_reset_radiotelem   = 0x24,

    radio_msg_set_radio_rx_enabled    = 0x2a,

    radio_msg_radio_forward_ready     = 0x31,

    radio_msg_get_authent             = 0x40,
    radio_msg_authent_status          = 0x41,
    radio_msg_set_authent             = 0x42,
    radio_msg_get_aro_key             = 0x43,
    radio_msg_aro_key                 = 0x44,
    radio_msg_set_aro_key             = 0x45,

    radio_msg_get_main_key            = 0x50,
    radio_msg_main_key                = 0x51,
    radio_update_telem                = 0x58,
    radio_send_test_data              = 0x59,

    // The only command that will be accepted from AROs
    radio_msg_obc_data                = 0x60,
} radio_msg_no;

#define RANGING_ACK_TYPE 1
#define RANGING_ACK_VERSION 1

typedef struct {
    uint8_t ack_type;
    uint8_t ack_version;
} radio_ranging_ack_t;


/**
 * @brief Create a postphone by seconds struct
 */
typedef struct {
    uint32_t postpone_sec;
} reboot_postpone_t;

/**
 * @brief Contains the PAYLOAD MSG data
 * the radio core will respond with
 *
 * The array data[1] gives access to the first
 * and second byte of the data field of this union.
 * Data[1] is only ever used in DMA transactions when
 * looking for fixed length transfers, otherwise use
 * data[0] as the start address for data
 */
typedef union {
    timespec_t time;
    radio_ranging_ack_t ranging_ack;
    reboot_postpone_t reboot_postpone;
    telemetry_t telemetry;
    radiotelem_t radiotelem;
    uint8_t authent_enable;
    uint8_t aes_key[AES_KEY_SIZE];
    uint8_t data[1];
    uint8_t radio_rx_enabled;
} msg_data_t;

/**
 * @brief Contains the callsign
 */
typedef struct radio_callsign{
    char callsign[8];
} radio_callsign_t;

#endif
