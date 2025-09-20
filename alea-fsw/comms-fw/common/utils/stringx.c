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

/*
Lightweight versions of some of the standard string library functions.

By taking advantage of the CC111x's unified memory space, these can be
smaller and typically faster than the sdcc versions which use generic pointers.

They are limited to operating on buffers <= 255 bytes long.
 */
#ifdef BOOTLOADER
#pragma codeseg APP_UPDATER
#endif
#include "./stringx.h"

/**
 * @brief Copies memory area from source to destination
 * @param[in] dst The memory address destination in XDATA to copy data too
 * @param[in] src The memory address location to copy
 * @param[in] account The length memory block (byte addressed) to copy
 */
void memcpyx (char __xdata *dst, const char __xdata *src, unsigned char acount) {
    // copy from lower addresses to higher addresses
    while (acount--) {
        *dst++ = *src++;
    }
}

/**
 * @brief Clear/set memory area
 * @param[in] dst The memory address destination in XDATA to clear/set
 * @param[in] value The value to be set/ copied
 * @param[in] account The Length of memory block (byte addressed) to clear/set
 */
void memsetx (char __xdata *dst, unsigned char value , unsigned char acount) {
    // copy from lower addresses to higher addresses
    while (acount--) {
        *dst++ = value;
    }
}

/**
 * @brief Compare memory areas A and B in constant time (used for to compare the in-flash signature)
 *
 * @param[in] a The memory address A in XDATA to compare
 * @param[in] b The memory address B in XDATA to compare
 * @param[in] n The Length of memory block (Byte addressed) to compare
 * @return 0 if all bits in A and B are equal, 1 if a bit is not equal
 */
__bit memcmpx_ct (const char __xdata *a, const char __xdata *b, unsigned char n) {
    char acc = 0;
    while(n--) {
        /* Any nonequal bytes will accumulate some 1 bits into acc. */
        acc |= *a++ ^ *b++;
    }
    return acc;
}

/**
 * @brief Compare memory areas A and B
 *
 * @param[in] a The memory address A in XDATA to compare
 * @param[in] b The memory address B in XDATA to compare
 * @param[in] n The Length of memory block (byte addressed) to compare
 * @return 0 or 1 depending on if bit a at address A is equal to bit b at address B
 */

// When a variable is declared as a bit, it is allocated into the bit addressable memory of the 8051
// The 16 bytes in DATA memory space on addresses 0x20 to 0x2F are bit addressable (pg 46/249) datasheet
// __bit memcmpx (const char __xdata *a, const char __xdata *b, unsigned char n) {
//     while (n--) {
//         if (*a++ != *b++) return 1;
//     }
//     return 0;
// }

/**
 * @brief Function return the length of a string
 *
 * @param[in] dst The memory address destination in XDATA
 * @param[in] src The memory address source in XDATA
 * @return String length (up to 256 Bytes)
 */
// uint8_t strcpylenx(__xdata char *dst, __xdata char *src) {
//     uint8_t l = 0;
//     while (dst[l] = src[l]) {
//         l++;
//     }
//     return l;
// }
