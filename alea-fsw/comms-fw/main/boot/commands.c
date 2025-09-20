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
 * @file commands.c
 * @brief File containing bootloader commands to be operated on based on given opcode command
 *
 * To make it clear how there can be 2 commands.c files. Remember that this is the commands.c for the
 * bootloader and it lives in a different area of flash then that of the radio commands.c.
 * The bootloader application can be running with no application fw image, and still needs to be able
 * to faciliate and act on commands (like a flash new application image command).
 *
 * @author Timothy Wriglesworth
 * @date May 21, 2021
 */

// #pragma is a compiler directive: "The name to be used for the
// code segment, default CSEG. This is useful if you need to tell the
// compiler to put the code in a special segment so you can later on
// tell the linker to put this segment in  a special place in memory."
// pg. 27/97 SDCC Compiler User Guide. Use APP_UPDATER in this case as an
// identifier
#pragma codeseg APP_UPDATER
#include "board_defaults.h"
#include "bootloader_commands.h"
#include "cc1110_regs.h"
#include "commands.h"
#include "flash.h"
#include "flash_vars.h"
#include "hwid.h"
#include "radio.h"
#include "watchdog.h"
#include "signature.h"
#include "stringx.h"

/**
 * @brief MAX_CMD_DATA corresponds to a full 255 of
 * availble pages to write.
 * FLASH_WRITE_PAGE_SIZE * MAX_PAGES = 32640 bytes
 * Remember we only have 32640 bytes of flash
 * (32640/1024) = (256/8) = [0,255]
 * Where 1024 is FLASH_PAGE_SIZE
 */
#define MAX_PAGES      255

/**
 * @brief APP_FLASH_TIMEOUT corresponds
 * when to reboot the bootloader during an
 * bootloader_msg_write_page command. It is a
 * lower value from the watchdog and measured
 * in main loop counts
 */
#define APP_FLASH_TIMEOUT 1000

// Global Timeout
extern uint16_t timeout;

/**
 * @brief Bootloader command handler
 *
 * The parameter "len" is unused in the bootloader, but DO NOT remove it. It's function
 * is shared using commands.h and "len" is used in the application radio/commands.c
 * Instead of re-writing the same function prototypes where one contains len and the other
 * doesn't, we keep things simple and just not use it in the bootloader.
 *
 * @param[in] cmd The buffer containing the command to handle
 * @param[in] len. UNUSED in bootloader command handler
 * @param[out] reply The buffer with the result of the cmd
 * @return length of the reply buffer, or 0 for no reply.
 */
uint8_t commands_handle_command(const __xdata command_t *cmd, uint8_t len, __xdata command_t *reply) {
    uint8_t reply_length;
    uint8_t flash_err;
    __xdata msg_data_t *cmd_data;
    __xdata msg_data_t *reply_data;
    __bit reset_timeout = 0;

    len;  // To avoid compiler warnings about this being unused

    if (get_resp(cmd->header.seqnum_and_resp) != 0) {
        // Comms doesn't ever expect responses
        return 0;
    }

    cmd_data = (__xdata msg_data_t *) cmd->data;
    reply_data = (__xdata msg_data_t *) reply->data;

    // Initialize the reply header
    reply->header.seqnum_and_resp = cmd->header.seqnum_and_resp | 0x8000;  // Set seqnum to be equal and set response bit
    reply->header.dest_hwid = cmd->header.src_hwid;
    reply->header.src_hwid = hwid_flash;

    // Fallthrough case - use "nack" as the default response
    reply->header.cmd_num = common_msg_nack;
    reply_length = sizeof(reply->header);

    switch (cmd->header.cmd_num) {
        // The bootloader can send acks similar to the default radio
        case common_msg_ack:
            reply->header.cmd_num = common_msg_ack;
            break;

        // The bootloader can send nacks similar to the default radio.    `
        case common_msg_nack:
            reply->header.cmd_num = common_msg_nack;
            break;

        // The bootloader accepts a basic ping with an ack reply
        // and can be used to delay a reboot of the bootloader.
        case bootloader_msg_ping:
            WATCHDOG_CLEAR;
            reset_timeout = 1;
            reply->header.cmd_num = bootloader_msg_ack;
            reply_data->ack.ack_message = BOOTLOADER_ACK_MSG_PONG;
            reply_length += sizeof(reply_data->ack.ack_message);
            break;

        // Erases the CC1110's flash memory
        case bootloader_msg_erase:
            WATCHDOG_CLEAR;
            reset_timeout = 1;
            flash_erase_app();
            reply->header.cmd_num = bootloader_msg_ack;
            reply_data->ack.ack_message = BOOTLOADER_ACK_MSG_ERASED;
            reply_length += sizeof(reply_data->ack.ack_message);
            break;

        // Writes a page to the CC1110's flash memory.
        // If the write fails it returns a nack
        // Writing page 255 with no data means all pages have been written
        case bootloader_msg_write_page:
            WATCHDOG_CLEAR;

            // If we have reached the last possible page to write (MAX_PAGES) and the signature
            // of all pages numbers [0,255] is valid, we APP_FLASH_TIMEOUT to boot the new application
            if (cmd_data->write_page.flash_page == MAX_PAGES) {
                if (len == sizeof(cmd->header) + sizeof(cmd_data->write_page.flash_page) && signature_app_valid()) {
                    timeout = APP_FLASH_TIMEOUT;
                    flash_err = FLASH_WRITE_OK;
                } else {
                    flash_err = FLASH_WRITE_ERR_UNKNOWN;
                }
            } else {
                reset_timeout = 1;
                flash_err = flash_write_app_page(cmd_data->write_page.flash_page, cmd_data->write_page.page_data);
            }

            if (flash_err != FLASH_WRITE_OK) {
                reply->header.cmd_num = bootloader_msg_nack;
            } else {
                reply->header.cmd_num = bootloader_msg_ack;
                reply_data->ack.ack_message = cmd_data->write_page.flash_page;
                reply_length += sizeof(reply_data->ack.ack_message);
            }
            break;
    }
    if (reset_timeout) {
        timeout = COMMAND_WATCHDOG_DELAY;
    }
    return reply_length;
}
