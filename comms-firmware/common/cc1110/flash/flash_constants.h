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
 * @file flash_constants.h
 * @brief Memory map for reserved storage sectors
 *
 * Memory map:
 * 0x0000 - 0x03FF - Bootloader
 * 0x0400 - 0x6BFF - Application
 * 0x6C00 - 06FFF - Persistent storage
 *
 * @See pg 45/249 of datasheet to see memory managament
 *
 * @author Timothy Wriglesworth
 * @date May 13, 2021
 */

#ifndef _FLASH_CONSTANTS_H
#define _FLASH_CONSTANTS_H

/**
 * @brief The CC1110 uses 1024 byte (512 word) pages
 * when performing erases
 */
#define FLASH_PAGE_SIZE 1024

/**
 * @brief We use 128 byte chunks when doing writes of
 * new app firmware
 */
#define FLASH_WRITE_PAGE_SIZE 128

/**
 * @brief Address marker for the end of the bootloader block.
 */
#define FLASH_BOOTLOADER_END 0x03FF

/**
 * @brief Signature key storage (3 16 byte keys)
 */
#define FLASH_SIGNATURE_KEYS 0x03CC
/**
 * @brief Address marker for the two reserved bytes.
 */
#define FLASH_RESERVED       0x03FC

/**
 * @brief The memoery address of radio HWID
 */
#define FLASH_HWID           0x03FE

/**
 * @brief Start address of application
 */
#define FLASH_APP_START      0x0400

/**
 * @brief End address of application
 */
#define FLASH_APP_END        0x6BFF

/**
 * @brief Store the application signature at this location
 */
#define FLASH_APP_SIGNATURE  0x6BF0

/**
 * @brief Store the CRC at this location
 */
#define FLASH_APP_CRC        0x6BFE

/**
 * @brief Start address of storage memory
 */
#define FLASH_STORAGE_START  0x6C00

/**
 * @brief End address of storage memory
 */
#define FLASH_STORAGE_END    0x6FFF

/**
 * @brief Store code for firmware updating here
 */
#define FLASH_UPDATER_START  0x7000

/**
 * @brief Store the size of the flash here
 */
#define FLASH_SIZE           0x8000

#endif
