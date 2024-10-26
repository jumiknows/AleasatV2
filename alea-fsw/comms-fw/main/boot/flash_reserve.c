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
 * @file flash_reserve.c
 * @brief Compiler/Linker directives to reserve regions of flash
 *
 * If we use too much space in the "HOME" region (see the #pragma directive in main.c)
 * the bootloader code will overlap with the designated storage areas in flash sector 0.
 * Code memory regions are defined here so that the linker will detect this overlap.
 * NOTE that these are only linker warnings and compilation will proceed regardless
 *
 * @author Timothy Wriglesworth
 * @date May 21, 2021
 */

#include <stdint.h>
#include "flash_vars.h"
#include "flash_constants.h"
#include "hwid.h"

/**
 * @brief The following are the reserved regions for the bootloader
 *
 * After compilation, memory locations need to be linked, and these __code directives
 * tell the linker to use CODE memory space (flash) and then the important flash address sectors
 * are explictly linked. i.e. the Linker doesn't arbitrarily chose- we tell it exactly what we want.
 * These address's come come from flash_constants.h
 */
__code __at (FLASH_SIGNATURE_KEYS) uint8_t  flash_keys[FLASH_RESERVED - FLASH_SIGNATURE_KEYS] = {0};
__code __at (FLASH_RESERVED)       uint16_t reserved_flash = 0;
__code __at (FLASH_HWID)           hwid_t   hwid_flash     = HWID;

/**
 * @brief These are reserved regions for the application
 */
__code __at (FLASH_APP_START)      uint8_t  flash_app[FLASH_STORAGE_START - FLASH_APP_START] = {0};
__code __at (FLASH_STORAGE_START)  uint8_t  flash_storage[FLASH_UPDATER_START - FLASH_STORAGE_START] = {0};
