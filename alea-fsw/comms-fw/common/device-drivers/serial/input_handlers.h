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

#ifndef _INPUT_HANDLERS_H
#define _INPUT_HANDLERS_H

#include <stdbool.h>

#ifndef BOOTLOADER
extern volatile bool notify_radio_forwarded_from_uart1;
extern volatile bool notify_radio_forwarded_from_spi;
#endif /* BOOTLOADER */

#if UART1_ENABLED == 1
void input_handle_uart1_rx(void);
#endif
#if SPI_ENABLED == 1
void input_handle_spi_rx(void);
#endif
void input_handle_rf_rx(void);

#endif
