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

// Watchdog timer setup and control
#ifdef BOOTLOADER
#pragma codeseg APP_UPDATER
#endif
#include <cc1110.h>


// This attribute __naked tells the compiler that the function is
// an embedded assembly function
void watchdog_reboot_now(void) __naked {
  // Minimum period
  WDCTL = 0xAB;
  // Clear
  WDCTL = 0x5B;
  // exactly what watchdogs are for
  while(1);
}
