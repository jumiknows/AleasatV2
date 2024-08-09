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
 * @file telemetry.h
 * @brief Header file for telemetry routines
 * @author Timothy Wriglesworth
 * @date May 13, 2021
 */

#ifndef _TELEMETRY_H
#define _TELEMETRY_H
#include <stdint.h>
#include "adc.h"

/**
 * @brief Radio telemetry struct defintion
 */
typedef struct {
    uint8_t reserved;
    // Uptime in seconds since the last reboot
    uint32_t uptime;
    // Recieved messages on SPI
    uint32_t spi_rx_count;
    // Recieved messages on UART1
    uint32_t uart1_rx_count;
    // Recv mode, see board_defaults.h
    uint8_t rx_mode;
    // Tx mode, see board_defaults.h
    uint8_t tx_mode;
    // ADCS Channel
    int16_t adc[ADC_NUM_CHANNELS];
    // Previous packets received signal
    // strength indicator
    int8_t last_rssi;
    // Link quality indicator
    uint8_t last_lqi;
    // Previous packets freq offset estimate.
    int8_t last_freqest;
    // Number of packets transmitted
    uint32_t packets_sent;
    // Carrier sense (number of times)
    // a signal above 0dBm abs threshold
    // has been detected
    uint32_t cs_count;
    // Number of good received packets
    uint32_t packets_good;
    // Number of packets with failed CRC-16
    uint32_t packets_rejected_checksum;
    uint32_t packets_rejected_authent;
    uint32_t packets_rejected_other;
    uint32_t reserved0;
    uint32_t reserved1;
    uint32_t custom0;
    uint32_t custom1;

} telemetry_t;

extern __xdata telemetry_t telemetry;

void telemetry_init(void);
void update_telemetry(void);

#endif
