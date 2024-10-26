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
 * @file board_defaults.h
 * @brief setting up the board defaults.
 */

#ifndef _BOARD_DEFAULTS_H
#define _BOARD_DEFAULTS_H

//TODO: remove?
#include "radio.h"

#ifdef CUSTOM_BOARD_INIT
#include "board.h"
#endif

#ifndef F_CLK
#define F_CLK 27000000
#endif

// TODO: Workaround SDCC lack of long long literal support
#ifndef FWT_FREQUENCY
#define FWT_FREQUENCY 36
#endif

#ifndef CONFIG_CAPABLE_RF_RX
#define CONFIG_CAPABLE_RF_RX 1
#endif

#ifndef CONFIG_CAPABLE_RF_TX
#define CONFIG_CAPABLE_RF_TX 1
#endif

// By default, enable UART1, enable SPI
#ifndef UART1_ENABLED
#define UART1_ENABLED 1
#endif
#ifndef SPI_ENABLED
#define SPI_ENABLED 1
#endif

// Default baud rates are 115200
#ifndef CONFIG_UART1_BAUD
#define CONFIG_UART1_BAUD 24
#endif
#ifndef CONFIG_SPI_BAUD
#define CONFIG_SPI_BAUD 24
#endif
// Use 14 here for 460800
#ifndef CONFIG_UART1_GCR
#define CONFIG_UART1_GCR 12
#endif
#ifndef CONFIG_SPI_GCR
#define CONFIG_SPI_GCR 12
#endif

#ifndef CONFIG_UART1_USE_FLOW_CTRL
#define CONFIG_UART1_USE_FLOW_CTRL 1
#endif
#ifndef CONFIG_UART1_FLOW_PIN
#define CONFIG_UART1_FLOW_PIN P0_3
#endif

// Defaults: flush active transaction, no flow control,
// 8 bits, no parity, 1 high stop bit, 1 low start bit
#ifndef CONFIG_UART1_UCR
#define CONFIG_UART1_UCR ((1<<7) | (1<<1))
#endif

#ifndef RADIO_MODE_DEFAULT_RX
#define RADIO_MODE_DEFAULT_RX rf_437_3mhz_gfsk_9600__sw_crc__whiten__no_fec
#endif

#ifndef RADIO_MODE_DEFAULT_TX
#define RADIO_MODE_DEFAULT_TX rf_437_3mhz_gfsk_9600__sw_crc__whiten__no_fec
#endif

#ifndef RADIO_MODE_DEFAULT
#define RADIO_MODE_DEFAULT rf_437_3mhz_gfsk_9600__sw_crc__whiten__no_fec
#endif

#ifndef RADIO_MODE_RANGING_RX
#define RADIO_MODE_RANGING_RX rf_437_3mhz_gfsk_9600__sw_crc__whiten__no_fec__ranging
#endif

#ifndef RADIO_MODE_RANGING_TX
#define RADIO_MODE_RANGING_TX rf_437_3mhz_gfsk_9600__sw_crc__whiten__no_fec__ranging
#endif

// Delay between RX_DONE and TX start (in ms by default)
// TODO: I thought this should be 2 but it works as 3
// maybe a bug somewhere
#ifndef RF_PRECISE_TIMING_DELAY
#define RF_PRECISE_TIMING_DELAY 3
#endif

// Automatically reboot unless commanded otherwise
// by default every 10 minutes
// Override this to 0 to disable the auto reboot
#ifndef AUTO_REBOOT_SECONDS
#define AUTO_REBOOT_SECONDS 600
#endif


// Cap the number of seconds the reboot counter
// can be extended. Override to 0 to disable this cap.
// The default is 1 week.
#ifndef AUTO_REBOOT_MAX
#define AUTO_REBOOT_MAX 604800
#endif


// By default, we don't enable any of the ADC inputs
// Keep in mind that setting any of these bits
// will override the port function and PDIR setting!
// TODO: enable unused ports as long as they are
// externally pulled down on the reference board
#ifndef ADCCFG_CONFIG
#define ADCCFG_CONFIG 0b11000100
#endif

// This is the default delay (in main loop counts)
// before trying to boot the application image
// when a command is received in the bootloader.
// This should be high enough so that the bootloader doesn't
// reset between commands but low enough that it doesn't
// exceed the watchdog timer
#ifndef COMMAND_WATCHDOG_DELAY
#define COMMAND_WATCHDOG_DELAY 45000
#endif

// Overrideable defaults for simple RF channel customization:
// IF freq of 395507.8125 Hz
#ifndef RF_FSCTRL1
#define RF_FSCTRL1 0x0F
#endif
#ifndef RF_FSCTRL0
#define RF_FSCTRL0 0x00
#endif
// base freq of 437.299805 Mhz
#ifndef RF_FREQ2
#define RF_FREQ2 0x10
#endif
#ifndef RF_FREQ1
#define RF_FREQ1 0x32
#endif
#ifndef RF_FREQ0
#define RF_FREQ0 0x40
#endif
// The defaults here are the CC1110 hard defaults
#ifndef RF_SYNC_WORD1
#define RF_SYNC_WORD1 0xD3
#endif
#ifndef RF_SYNC_WORD0
#define RF_SYNC_WORD0 0x91
#endif
// Channel bandwidth 105.468kHz
#ifndef RF_CHAN_BW_E
#define RF_CHAN_BW_E 0x3
#endif
#ifndef RF_CHAN_BW_M
#define RF_CHAN_BW_M 0x0
#endif
// Channel bandwidth 105.468kHz
#ifndef RF_CHAN_BW_RANGING_E
#define RF_CHAN_BW_RANGING_E 0x3
#endif
#ifndef RF_CHAN_BW_RANGING_M
#define RF_CHAN_BW_RANGING_M 0x0
#endif
// 9604 baud
#ifndef RF_DRATE_E
#define RF_DRATE_E   0x8
#endif
#ifndef RF_DRATE_M
#define RF_DRATE_M  0x75
#endif
// 9604 baud
#ifndef RF_DRATE_RANGING_E
#define RF_DRATE_RANGING_E   0x8
#endif
#ifndef RF_DRATE_RANGING_M
#define RF_DRATE_RANGING_M  0x75
#endif
// delta_f_channel = 363.372kHz
#ifndef RF_CHANSPC_E
#define RF_CHANSPC_E 0x3
#endif
#ifndef RF_CHANSPC_M
#define RF_CHANSPC_M 0xB9
#endif
#ifndef RF_CHANSPC_RANGING_E
#define RF_CHANSPC_RANGING_E 0x3
#endif
#ifndef RF_CHANSPC_RANGING_M
#define RF_CHANSPC_RANGING_M 0xB9
#endif
// 4.943848 kHz difference between symbols
#ifndef RF_DEVIATN_E
#define RF_DEVIATN_E 0x1
#endif
#ifndef RF_DEVIATN_M
#define RF_DEVIATN_M 0x4
#endif
// 9.887696 kHz difference between symbols
#ifndef RF_DEVIATN_RANGING_E
#define RF_DEVIATN_RANGING_E 0x2
#endif
#ifndef RF_DEVIATN_RANGING_M
#define RF_DEVIATN_RANGING_M 0x4
#endif
// From SmartRF studio
#ifndef RF_FSCAL3_CONFIG
#define RF_FSCAL3_CONFIG 0xE9
#endif
#ifndef RF_FSCAL2_CONFIG
#define RF_FSCAL2_CONFIG 0x2A
#endif
#ifndef RF_FSCAL1_CONFIG
#define RF_FSCAL1_CONFIG 0x0
#endif
#ifndef RF_FSCAL0_CONFIG
#define RF_FSCAL0_CONFIG 0x1F
#endif
#ifndef RF_TEST0_CONFIG
#define RF_TEST0_CONFIG  0x9
#endif
// PA_CONFIG of 0xC0 is recommended by the datasheet (pg 207)
// for 33mA output at 433MHz. It is the highest setting listed in
// the datasheet.
#ifndef RF_PA_CONFIG
#define RF_PA_CONFIG     0xC0
#endif

#ifndef MAX_RX_TICKS
// Default of 5 seconds
#define MAX_RX_TICKS 50
#endif

// These are the default radio modes
#ifndef BOARD_RF_SETTINGS
typedef enum {
  rf_437_3mhz_gfsk_9600__sw_crc__whiten__no_fec = 0,
  rf_437_3mhz_gfsk_9600__sw_crc__whiten__no_fec__ranging = 1,
} lst_rf_mode_e;
#endif

#define ANT_DEPLOY_ENABLE  0b00000000
#define ANT_DEPLOY_DISABLE 0b00000001
#define DEFAULT_ANTENNA_DEPLOY ANT_DEPLOY_DISABLE

#define PA_ENABLE  0b00000010
#define PA_DISABLE 0b00000000
#define DEFAULT_PA_ENABLE PA_DISABLE

#define SELECT_ANT_SOLAR  0b00000100
#define SELECT_ANT_HEADER 0b00000000
#define DEFAULT_ANTENNA_SEL SELECT_ANT_SOLAR

#define SELECT_TRX_RF_PATH 0b00000000
#define SELECT_PA_RF_PATH  0b00001000
#define DEFAULT_RADIO_PATH SELECT_TRX_RF_PATH

#define AUTHENTICATION_ENABLED_ON_BOOT 1

extern volatile uint8_t alea_rffe;

void board_defaults_init(void);
uint8_t board_apply_radio_settings(uint8_t mode);

#endif
