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

#ifndef _SPI_H
#define _SPI_H

#include <cc1110.h>
#include <stdint.h>

#ifndef SPI_RX_BUFFERS
#define SPI_RX_BUFFERS 1
#endif

void spi_rx_isr() __interrupt (URX0_VECTOR) __using (3); //USART interrupt vector
void spi_init(void);
uint8_t spi_get_message(__xdata uint8_t *buf);
void spi_send_message(const __xdata uint8_t *msg, uint8_t len);

#ifndef BOOTLOADER
__bit spi_buffers_empty(void);
#endif /* BOOTLOADER */

void send_command_spi(hwid_t dest, radio_msg_no_t cmd, const uint8_t *data, uint8_t data_len);

extern volatile __data uint32_t spi_rx_count;

#endif
