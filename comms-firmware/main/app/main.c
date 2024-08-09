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

#include <stdint.h>
#include <cc1110.h>
#include "board_defaults.h"
#include "adc.h"
#include "aes.h"
#include "clock.h"
#include "commands.h"
#include "dma.h"
#include "input_handlers.h"
#include "interrupts.h"
#include "schedule.h"
#include "uart1.h"
#include "spi.h"
#include "radio.h"
#include "telemetry.h"
#include "timers.h"
#include "watchdog.h"
#include "antenna.h"
#include "routing.h"
#include "radio_commands.h"
#include "flash_vars.h"
// User specified board setup
#ifdef CUSTOM_BOARD_INIT
#include "board.h"
#else
#ifndef NO_CUSTOM_BOARD_INIT
#warning "No board specific board.h/board.c was specified"
#endif
#endif


#ifndef GIT_REV
#define GIT_REV
#endif

#define TO_STR(x) #x
#define BOOT_STRING(git_rev) TO_STR(OpenLST git_rev)

// Save the boot string in named variable
static const __code uint8_t boot_str[] = BOOT_STRING(GIT_REV);

// don't make this too long or it could get stuck in a watchdog reset loop
#define TIMEOUT 100000

uint32_t timeout = TIMEOUT;

static void initialize(void) {
    // Set up the watchdog reset timer for about 1 second
    WATCHDOG_ENABLE;
    WATCHDOG_CLEAR;

    clock_init();
    timers_init();
    dma_init();
    hwid_table_init();
    telemetry_init();
    #ifdef CUSTOM_BOARD_INIT
    board_init();  // User specified init functions
    #endif
    // TODO: wait for ccboot to finish whatever it's sending in case we just
    // did a bootload
    #if UART1_ENABLED == 1
    uart1_init();
    #endif
    #if SPI_ENABLED == 1
    spi_init();
    #endif


    INTERRUPT_GLOBAL_ENABLE;

    adc_init();
    schedule_init();
    aes_init();
    radio_init();
    board_apply_radio_settings(radio_mode_rx);
    #if CONFIG_CAPABLE_RF_RX == 1
    radio_listen();
    #endif
}

void main(void) {
    initialize();

    #if BOARD_HAS_LED == 1
    board_led_set(0);
    #endif

    // Print the git revision out over the UART
    send_command_uart1(hwid_table[DEST_LOCAL_DEBUG].start, radio_msg_start, boot_str, sizeof(boot_str));
    // Print the git revision out over SPI
    send_command_spi(hwid_table[DEST_OBC].start, radio_msg_start, boot_str, sizeof(boot_str));

    while (1) {
        WATCHDOG_CLEAR;
        schedule_handle_events();

        #if UART1_ENABLED == 1
        input_handle_uart1_rx();
        #endif
        #if SPI_ENABLED == 1
        input_handle_spi_rx();
        #endif

        input_handle_rf_rx();

        handle_antenna_deployment();

        if (reboot_scheduled == true) {
            // Print reboot message over the UART
            send_command_uart1(hwid_table[DEST_LOCAL_DEBUG].start, radio_msg_rebooting, (void *) 0, 0);
            // Print reboot message over SPI
            send_command_spi(hwid_table[DEST_OBC].start, radio_msg_rebooting, (void *) 0, 0);
            watchdog_reboot_now();
        }

        // if we have forwarded a packet via the radio and all our buffers are empty,
        // send status msg indicating we are ready for more data to forward
        if (notify_radio_forwarded_from_uart1 == true) {
            if (uart1_buffers_empty()) {
                send_command_uart1(hwid_table[DEST_LOCAL_DEBUG].start, radio_msg_radio_forward_ready, 0, 0);
                notify_radio_forwarded_from_uart1 = false;
            }
        }
        if (notify_radio_forwarded_from_spi == true) {
            if (spi_buffers_empty()) {
                send_command_spi(hwid_table[DEST_OBC].start, radio_msg_radio_forward_ready, 0, 0);
                notify_radio_forwarded_from_spi = false;
            }
        }
    }
}
