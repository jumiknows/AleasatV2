/**
 * @file obc_glibc.h
 * @brief Local copies of glibc functions.
 * @author Richard Arthurs (see .c file for source attribution).
 * @date April 15, 2020
 */

#ifndef OBC_GLIBC_H
#define OBC_GLIBC_H

char* orca_strtok_r(char* s, const char* delim, char** save_ptr); // Reentrant string tokenize

#endif // OBC_GLIBC_H
