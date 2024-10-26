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
 * @file flash.h
 * @brief Header file for flash.c
 * @author Timothy Wriglesworth
 * @date May 21, 2021
 */

#ifndef _FLASH_H
#define _FLASH_H

#include <stdint.h>
#include "flash_constants.h"
#include "hwid.h"
#include "signature.h"

/**
 * @brief Flash operation did not have any issues.
 */
#define FLASH_WRITE_OK             0

/**
 * @brief Error signalling we are trying to write flash in protected area.
 */
#define FLASH_WRITE_ERR_PROTECTED  1

/**
 * @brief Error singaling we are trying to write to an invalid address.
 */
#define FLASH_WRITE_ERR_BAD_ADDR   2

/**
 * @brief Unknown error.
 */
#define FLASH_WRITE_ERR_UNKNOWN      255

void flash_erase_app(void) __critical;
uint8_t flash_write_app_page(uint8_t page, __xdata uint8_t *buf);

#endif
