/**
 * @file obc_utils.h
 * @brief General OBC utility defines / typedefs / functions
 */

#ifndef OBC_UTILS_H
#define OBC_UTILS_H

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// FreeRTOS
#include "rtos.h"

// HALCoGen
#include "sys_common.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// Arrays
#define LEN(array)                  (sizeof((array)) / sizeof((array)[0]))

// Math
#define MIN(X, Y)                   (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)                   (((X) > (Y)) ? (X) : (Y))

// Timing
#define SYSTEM_TIME_US()            portCPU_CLOCK_US() /* Use FreeRTOS CPU clock */

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// String Manipulation
int32_t cseq_to_num(char* seq);

// Timing
void busy_wait(uint32_t ticks_to_wait);
void obc_delay_us(uint32_t us);

// Math
uint32_t power(uint32_t b, uint32_t e);

// System
void restart_software(void);

#endif // OBC_UTILS_H
