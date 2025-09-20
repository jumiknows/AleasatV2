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
 * @file updater.c
 * @brief Routines to update application firmware
 * @author Timothy Wriglesworth
 * @date May 21, 2021
 */

// #pragma is a compiler directive: "The name to be used for the
// code segment, default CSEG. This is useful if you need to tell the
// compiler to put the code in a special segment so you can later on
// tell the linker to put this segment in  a special place in memory."
// pg. 27/97 SDCC Compiler User Guide. Use APP_UPDATER in this case
// as an identifier
#pragma codeseg APP_UPDATER
#include <stdint.h>
#include <cc1110.h>
#include "board_defaults.h"
#include "input_handlers.h"
#include "interrupts.h"
#include "uart1.h"
#include "spi.h"
#include "radio.h"
#include "watchdog.h"
#include "bootloader_commands.h"

#ifndef GIT_REV
#define GIT_REV
#endif

/**
 * @brief The following 2 defines will append the git revision
 * number of the bootloader image
 */

// Append token x
#define TO_STR(x) #x


// Build.mk will populate token "git_rev"
// Append string OpenLST BL and 'token git_rev'
// Use #define's to save code space
#define BOOT_STRING(git_rev) TO_STR(OpenLST BL git_rev)

// Save the boot string in named variable
static const __code uint8_t boot_str[] = BOOT_STRING(GIT_REV);

// Global constant timeout used for watchdog reboots
uint16_t timeout;

/**
 * @brief Initialze external communication periphals
 *
 * @pre board_init
 */
static void initialize(void) {
    IN_BOOTLOADER_FLAG = 1;
    WATCHDOG_CLEAR;

    // The clock and DMA descriptors were already
    // set in bootloader_main
    // board_init has already been called as well

    // Now we enable the UARTs and radio to receive
    // updates
    #if UART1_ENABLED == 1
    uart1_init();
    #endif
    #if SPI_ENABLED == 1
    spi_init();
    #endif

    INTERRUPT_GLOBAL_ENABLE;
    radio_init();

    #if CONFIG_CAPABLE_RF_RX == 1
    radio_listen();
    #endif
}


/**
 * @brief Accept data over the UART periphal
 * or radio link to send to the input handler
 */
void updater(void) {
    initialize();

    #if BOARD_HAS_LED == 1
    // Red LED should be on while updating
    board_led_set(1);
    #endif

    // Print the git revision out over the UART
    send_command_uart1(HWID_LOCAL_START, bootloader_msg_start, boot_str, sizeof(boot_str));
    // Print the git revision out over SPI
    send_command_spi(HWID_OBC_START, bootloader_msg_start, boot_str, sizeof(boot_str));

    // Accept data over the UART or radio
    // while watchdog control loop is not going
    // to force a reboot
    timeout = COMMAND_WATCHDOG_DELAY;
    // Once timeout is 0, we return
    // (its a void so don't return a value, but
    // we still return to caller). Careful, timeout
    // is not always COMMAND_WATCHDOG_DELAY.
    // In commands.c 'bootloader_msg_write_page' it can be
    // set to APP_FLASH_TIMEOUT. APP_FLASH_TIMEOUT is much
    // less delay than COMMAND_WATCHDOG_DELAY. This
    // ensures if there is a faster reboot after a valid write
    // page operation
    while (--timeout) {
        #if SPI_ENABLED == 1
        input_handle_spi_rx();
        #endif
        #if UART1_ENABLED == 1
        input_handle_uart1_rx();
        #endif
        input_handle_rf_rx();
    }
}
