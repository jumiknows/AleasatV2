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
 * @file compiler_utils.h
 * @brief Configuration file for sanity checks in case we override flash.h settings
 * @author Timothy Wriglesworth
 * @date May 13, 2021
 */

#ifndef _COMPILER_UTILS_H
#define _COMPILER_UTILS_H

/**
 * @brief Compliation macro used as a sanity check in compilation
 * ## is a pre-processor macro in C. It is used to concatenate 2 tokens into one token.
 */
#define STATIC_ASSERT(id, exp) \
  typedef char _static_assert_ ## id[(exp) ? 1 : -1]

#endif