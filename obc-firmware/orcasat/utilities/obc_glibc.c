/**
 * @file obc_glibc.c
 * @brief Local copies of glibc functions.
 * @date April 15, 2020
 *
 * When adding to this file, please ensure licensing requirements are respected. Generally,
 * this means including the copyright comment block from the original source file.
 */

#include <obc_glibc.h>
#include <string.h>
#include "sys_common.h"

/* Reentrant string tokenizer.  Generic version.
   Copyright (C) 1991-2019 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.
 */

/**
 * @brief Reentrant string tokenize.
 * Regular strtok is not thread safe or reentrant because it saves state within the function.
 * This one requires the user to store the state.
 *
 * Modified from glibc: https://code.woboq.org/userspace/glibc/string/strtok_r.c.html. See license
 * information above, or license file in glibc_license.txt. By the terms of the original license,
 * this source file is licensed under the GNU Lesser General Public License Version 2.1 or later.
 *
 * Parse S into tokens separated by characters in DELIM.
 * If S is NULL, the saved pointer in SAVE_PTR is used as
 * the next starting point.  For example:
 *      char s[] = "-abc-=-def";
 *      char *sp;
 *       x = strtok_r(s, "-", &sp);        // x = "abc", sp = "=-def"
 *       x = strtok_r(NULL, "-=", &sp);    // x = "def", sp = NULL
 *       x = strtok_r(NULL, "=", &sp);     // x = NULL
 *       s = "abc\0-def\0"
 *
 * @param[inout] s The string to parse. Note that the delimiter will be replaced with '\0'.
 * @param[in] delim The delimiter to split by.
 * @param[out] save_ptr The next starting point to use on future iterations.
 * @returns NULL if the final token has been reached. Otherwise, the location of the tokenized
 * component.
 */
char* orca_strtok_r(char* s, const char* delim, char** save_ptr) {
    char* end;

    if (s == NULL) {
        s = *save_ptr;
    }

    if (*s == '\0') {
        *save_ptr = s;
        return NULL;
    }

    /* Scan leading delimiters.  */
    s += strspn(s, delim);
    if (*s == '\0') {
        *save_ptr = s;
        return NULL;
    }

    /* Find the end of the token.  */
    end = s;
    end += strcspn(s, delim);
    if (*end == '\0') {
        *save_ptr = end;
        return s;
    }

    /* Terminate the token and make *SAVE_PTR point past it.  */
    *end      = '\0';
    *save_ptr = end + 1;
    return s;
}
