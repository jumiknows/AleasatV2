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
 * @file signature.h
 * @brief Configuration file signing the application fw
 * @author Timothy Wriglesworth
 * @date May 21, 2021
 */

#ifndef _SIGNATURE_H
#define _SIGNATURE_H

#include <stdint.h>

/**
 * @brief Keys lengths are 16 bytes or 128 bits
 */
#define AES_KEY_SIZE   16

/**
 * @brief Block size is 16 bytes or 128 bits
 */
#define AES_BLOCK_SIZE 16

/**
 * @brief 3 keys can be used for encryption/decryption
 */
#define SIGNATURE_KEY_COUNT 3


/**
 * @brief Array containing signature key
 */
typedef struct {
    uint8_t key[AES_KEY_SIZE];
} signature_key_t;

/**
 * @brief Use bit addressable memory to check for
 * valid application signature
 */
__bit signature_app_valid(void);

#endif
