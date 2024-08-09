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
 * @file crc16.c
 * @brief Routine to run (CRC-16) error checking operations
 * @author Timothy Wriglesworth
 * @date May 13, 2021
 */

#ifdef BOOTLOADER
// #pragma is a compiler directive: "The name to be used for the
// code segment, default CSEG. This is useful if you need to tell the
// compiler to put the code in a special segment so you can later on
// tell the linker to put this segment in  a special place in memory."
// pg. 27/97 SDCC Compiler User Guide. Use APP_UPDATER in this case as an
// identifier
#pragma codeseg APP_UPDATER
#endif
#include <cc1110.h>
#include "crc16.h"

/**
 * @brief Routine which calculates CRC-16 on input data
 *
 * The LFSR can be seeded by writing to the
 * RNDL register twice. Each time the RNDL
 * register is written, the 8 LSB of the LFSR is
 * copied to the 8 MSB and the 8 LSBs are
 * replaced with the new data byte that was
 * written to RNDL.
 *
 * @param[in] data The data in XDATA
 * @param[in] len The length of the above data
 * @return 2 byte CRC16 value
 */
uint16_t crc16(__xdata uint8_t *data, uint16_t len) {
  // The polynomial used for the LFSR random number generator
  // is that of the CRC16 used as well. They are dual purposed
  // (see page 147/249 of the CC1110 datasheet).

  // Using 0xFFFF as seed value for the LFSR will give the
  //    CRC-16 polynomial used by the radio
  // Random Number Generator Data Low Byte is RNDL
  RNDL = 0xFF;
  RNDL = 0xFF;
  while (len--)
    RNDH = *data++;

  // When used for CRC calculations, reading this
  // register RNDH returns the 8 MSBs of the CRC result, now we
  // return the upper and lower bytes of the CRC-16 result
  return RNDH << 8 | RNDL;
}
