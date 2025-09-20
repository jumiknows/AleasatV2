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

#ifndef __STRINGX_H__
#define __STRINGX_H__

#include <stdint.h>

/**
 * @brief Macro to copy from destination 'd' to source 's' with length 'n'
 */
#define MEMCPYX(d, s, n) memcpyx((__xdata char *)(d), (__xdata const char *)(s), n)

/**
 * @brief Macro to compare data at destination 'd' to source 's' with length 'n'
 */
#define MEMCMPX(d, s, n) memcmpx((__xdata const char *)(d), (__xdata const char *)(s), n)

void memcpyx  (char __xdata *dst, const char __xdata *src, unsigned char acount);
void memsetx (char __xdata *dst, unsigned char value , unsigned char acount);
__bit memcmpx_ct (const char __xdata *a, const char __xdata *b, unsigned char n);
// __bit memcmpx (const __xdata char *a, const __xdata char *b, unsigned char n);
// uint8_t strcpylenx(__xdata char *dst, __xdata char *src);

/**
 * @brief Macro tool to assign XDATA to a different alias
 */
#define XDATA_ALIAS(p) ((__xdata void *)(0xFF00 | (unsigned char)&p))

#endif
