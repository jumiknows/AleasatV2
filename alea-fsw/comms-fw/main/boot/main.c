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
 * @file main.c
 * @brief Main program for bootloader
 * @author Timothy Wriglesworth
 * @date May 21, 2021
 */

// #pragma is a compiler directive: "The name to be used for the
// code segment, default CSEG. This is useful if you need to tell the
// compiler to put the code in a special segment so you can later on
// tell the linker to put this segment in  a special place in memory."
// pg. 27/97 SDCC Compiler User Guide. Use HOME in this case as an
// identifier
#pragma codeseg HOME
#include <cc1110.h>
#include "board_defaults.h"
#include "clock.h"
#include "commands.h"
#include "dma.h"
#include "flash.h"
#include "interrupts.h"
#include "signature.h"
#include "updater.h"
#include "watchdog.h"

// User specified board setup
#ifdef CUSTOM_BOARD_INIT
#include "board.h"
#else
#ifndef NO_CUSTOM_BOARD_INIT
#warning "No board specific board.h/board.c was specified"
#endif
#endif

/**
 * @brief Bootloader main control routine
 *
 * Assembly routine "start.asm" runs at boot and
 * jumps to main here in bootloader_main.
 *
 * Periphals required for the bootloader application
 * are initialized, the application image is verified
 * for data integrity and then the application program
 * is run
 */
void bootloader_main(void) {
    // Disable global Interrupts
    // If we rebooted from the main application,
    // we don't want to receive any interrupts until we
    // are ready to handle them
    INTERRUPT_GLOBAL_DISABLE;

    // Set up the watchdog reset timer for about 1 second
    WATCHDOG_ENABLE;
    WATCHDOG_CLEAR;
    // Disable interrupt enables for when we reenable
    // interrupts globally
    IEN0 = IEN1 = IEN2 = 0;

    clock_init();
    dma_init();

    #ifdef CUSTOM_BOARD_INIT
    // User specified init functions
    // These correspond to radio type.
    board_init();
    #endif

    // Listen over the radio for updates
    IN_BOOTLOADER_FLAG = 1;
    updater();

    // Try to boot the application
    if (signature_app_valid()) {

        // Disable interrupt enables for when we reenable
        // interrupts locally.
        INTERRUPT_GLOBAL_DISABLE;
        IEN0 = IEN1 = IEN2 = 0;

        // Mark the bootloader flag so that the ISR trampoline
        // in start.asm jumps to the application ISRs
        IN_BOOTLOADER_FLAG = 0;

        // Jump to user code at FLASH_APP_START 0x0400
        __asm
        ljmp #FLASH_APP_START
        __endasm;
    }
}
