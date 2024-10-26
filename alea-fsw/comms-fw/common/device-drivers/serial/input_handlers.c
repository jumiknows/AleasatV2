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

// Input handlers for command processing and forwarding
#ifdef BOOTLOADER
#pragma codeseg APP_UPDATER
#endif
#include "board_defaults.h"
#include "commands.h"
#include "flash_vars.h"
#include "hwid.h"
#include "input_handlers.h"
#include "uart.h"
#include "uart1.h"
#include "spi.h"
#include "radio.h"

#ifndef BOOTLOADER
#include "routing.h"
#include "radio_commands.h"
#include <stdbool.h>
#endif

// @brief Buffer in RAM for incoming command
static __xdata command_buffer_t buffer;
// @brief Buffer in RAM for outgoing command
static __xdata command_buffer_t reply;

// TODO: good idea?
// Overridable in case zero-length messages,
// messages with no command opcode, no seqnum
// are desired. Also in case we want to include
// an extra required field


/**
 * @brief Minimum size of the expected UART messages, in bytes.
 * Those smaller than this will be ignored.
 */
#ifndef MIN_UART_MSG_SIZE
#define MIN_UART_MSG_SIZE  sizeof(command_header_t)
#endif
/**
 * @brief Minimum size of the expected RF messages, in bytes.
 * Those smaller than this will be ignored.
 */
#ifndef MIN_RADIO_MSG_SIZE
#define MIN_RADIO_MSG_SIZE sizeof(command_header_t)
#endif


#ifndef BOOTLOADER

volatile bool notify_radio_forwarded_from_uart1 = false;
volatile bool notify_radio_forwarded_from_spi = false;

/**
 * @brief Handle forwarding messages for destinations other than us
 */
static void input_handle_msg_forward(uint8_t len, interface_id_t src_iface) {
    dest_t src;
    dest_t dest;
    routing_action_t action;
    interface_id_t dest_iface;

    src = get_dest_from_hwid(buffer.cmd.header.src_hwid);
    dest = get_dest_from_hwid(buffer.cmd.header.dest_hwid);
    if ((src == DEST_INVALID) || (dest == DEST_INVALID)) {
        return;
    }
    action = routing_table[src][dest];
    if (action == ROUTE_FORWARD) {
        dest_iface = dest_interface_table[dest];
        if (dest_iface == INTERFACE_SPI) {
            spi_send_message(buffer.msg_bytes, len);
        }
        else if (dest_iface == INTERFACE_UART) {
            uart1_send_message(buffer.msg_bytes, len);
        }
        else if (dest_iface == INTERFACE_RADIO) {
            radio_send_packet(&buffer.cmd, len, RF_TIMING_NOW);
            if (src_iface == INTERFACE_UART) {
                notify_radio_forwarded_from_uart1 = true;
            }
            else if (src_iface == INTERFACE_SPI) {
                notify_radio_forwarded_from_spi = true;
            }
        }
    }
}
#endif /* BOOTLOADER */

#if UART1_ENABLED == 1
void input_handle_uart1_rx(void) {
    uint8_t len;
    uint8_t reply_len;

    len = uart1_get_message(buffer.msg_bytes);
    if (len < MIN_UART_MSG_SIZE) {
        // no messages (0) or if message too short just drop it
        return;
    }

    // See if this message is addressed to us
    if (buffer.cmd.header.dest_hwid == hwid_flash) {
        // If it is for us, pass the message off to the command handler
        reply_len = commands_handle_command(&buffer.cmd, len, &reply.cmd);
        if (reply_len) {
            uart1_send_message(reply.msg_bytes, reply_len);
        }
    }
    else {
        #ifndef BOOTLOADER
        input_handle_msg_forward(len, INTERFACE_UART);
        #endif
    }
    return;
}
#endif

#if SPI_ENABLED == 1
void input_handle_spi_rx(void) {
    uint8_t len;
    uint8_t reply_len;
    len = spi_get_message(buffer.msg_bytes);
    if (len < MIN_UART_MSG_SIZE) {
        // no messages (0) or if message too short just drop it
        return;
    }

    // See if this message is addressed to us
    if (buffer.cmd.header.dest_hwid == hwid_flash) {
        // If it is for us, pass the message off to the command handler
        reply_len = commands_handle_command(&buffer.cmd, len, &reply.cmd);
        if (reply_len) {
            spi_send_message(reply.msg_bytes, reply_len);
        }
    }
    else {
        #ifndef BOOTLOADER
        input_handle_msg_forward(len, INTERFACE_SPI);
        #endif
    }
    return;
}
#endif

void input_handle_rf_rx(void) {
    uint8_t len;
    uint8_t reply_len;

    len = radio_get_message(&buffer.cmd);
    // if (len == 0) { // no messages
    if (len < MIN_RADIO_MSG_SIZE) {
        // no messages (0) or if message too short just drop it
        return;
    }
    // Start listening again now that we've copied the message over
    radio_listen();

    // See if this message is addressed to us
    if (buffer.cmd.header.dest_hwid == hwid_flash) {
        // If it is for us, pass the message off to the command handler
        reply_len = commands_handle_command(&buffer.cmd, len, &reply.cmd);
        if (reply_len) {
            radio_send_packet(&reply.cmd, reply_len, RF_TIMING_NOW);
        }
    }
    else {
        #ifndef BOOTLOADER
        input_handle_msg_forward(len, INTERFACE_RADIO);
        #endif
    }
    return;
}
