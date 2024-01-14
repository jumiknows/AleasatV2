/**
 * @file log_utils.h
 * @brief General Logging defines / typdefs / functions
*/

#ifndef LOG_UTILS_H
#define LOG_UTILS_H

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// System
#include "logger.h"

// StdLib
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// Logging
#define DEBUG_LINE()     debug_line(__FILE__, __LINE__)

static void debug_line(const char *file, uint16_t line) {
    const char *filename = strrchr(file, '/');
    filename = (filename != NULL) ? (filename + 1) : file;
    LOG_PRINTF__PRINT_DEBUG((uint8_t *)filename, strlen(filename), line);
}


#endif // LOG_UTILS_H
