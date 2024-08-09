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
 * @file bootloader_commands.h
 * @brief Header file for bootloader commands
 * @author Timothy Wriglesworth
 * @date May 21, 2021
 */

// Command message opcodes
#ifndef _MESSAGES_H
#define _MESSAGES_H

#include <stdint.h>
#include "hwid.h"
#include "flash.h"
#include "signature.h"

/**
 * @brief List of opcode messages for bootloader commands types
 */
typedef enum {
    // Older versions of the bootloader used
    // separate command codes for serial connections
    // and radio connections.
    // Both are included here for compatiblity
    bootloader_msg_ping        = 0x00,
    bootloader_msg_erase       = 0x0c,
    bootloader_msg_write_page  = 0x02,
    bootloader_msg_ack         = 0x01,
    bootloader_msg_nack        = 0x0f,
    bootloader_msg_start       = 0x0e
} bootloader_msg_no;

/**
 * @brief Use 128 byte chunks when doing writes of
 * 1 page for new app firmware
 */
typedef struct {
    uint8_t flash_page;
    uint8_t page_data[FLASH_WRITE_PAGE_SIZE];
} msg_bootloader_write_page_t;

/**
 * @brief Flag to say bootloader ping received
 */
#define BOOTLOADER_ACK_MSG_PONG   0

/**
 * @brief Flag to say bootloader erase flash
 * command received
 */
#define BOOTLOADER_ACK_MSG_ERASED 1

typedef struct {

    // Bootloader acknowledgment reply
    // of bootloader ping command
    uint8_t ack_message;
} msg_bootloader_ack_t;

/**
 * @brief The msg data format that
 * is used to interact with the command
 * handler for the bootloader application
 */
typedef union {
    // Consists of a page number and the corresponding page data
    msg_bootloader_write_page_t write_page;
    // Bootloader acknowledgement
    msg_bootloader_ack_t ack;
    // Array containing keys
    signature_key_t load_keys[SIGNATURE_KEY_COUNT];
    // Hardware identifier
    hwid_t hwid;

    // TODO Figure out.
    uint8_t data[1];
} msg_data_t;

#endif
