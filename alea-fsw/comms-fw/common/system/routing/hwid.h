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
 * @file hwid.h
 * @brief Configuration file for Hardware ID
 * @author Timothy Wriglesworth
 * @date May 13, 2021
 */

#ifndef HWID_H
#define HWID_H

#include <stdint.h>
#include "flash_constants.h"

// TODO: turn into table in code memory at specific location in flash

#define HWID_OBC_START (0x7000)
#define HWID_OBC_END   (0x701F)

#define HWID_GS1_START (0x8000)
#define HWID_GS1_END   (0x801F)

#define HWID_GS2_START (0x8020)
#define HWID_GS2_END   (0x803F)

#define HWID_ARO_START (0xE000)
#define HWID_ARO_END   (0xEFFF)

/**
 * @brief HWID_LOCAL is the HWID for the local debug connection on UART1
 */
#define HWID_LOCAL_START (0xFFFF)
#define HWID_LOCAL_END   (0xFFFF)

/**
 * @brief Hardware ID.
 */
typedef uint16_t hwid_t;

/**
 * @brief Store the FLASH_HWID in code memory (flash) at address 0x03FE
 */
__code __at (FLASH_HWID) hwid_t hwid_flash;

#endif  // HWID_H
