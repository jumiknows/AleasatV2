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

/**
 * Source: https://stackoverflow.com/questions/807244/c-compiler-asserts-how-to-implement
 * 
 * A compile time assertion check.
 *
 * Validate at compile time that the predicate is true without
 * generating code. This can be used at any point in a source file
 * where typedef is legal.
 *
 * On success, compilation proceeds normally.
 *
 * On failure, attempts to typedef an array type of negative size. The
 * offending line will look like
 *      typedef assertion_failed_file_h_42[-1]
 * where file is the content of the second parameter which should
 * typically be related in some obvious way to the containing file
 * name, 42 is the line number in the file on which the assertion
 * appears, and -1 is the result of a calculation based on the
 * predicate failing.
 *
 * @param predicate The predicate to test. It must evaluate to
 * something that can be coerced to a normal C boolean.
 *
 * @param file A sequence of legal identifier characters that should
 * uniquely identify the source file in which this condition appears.
 */
#define CASSERT(predicate, file) _impl_CASSERT_LINE(predicate,__LINE__,file)

#define _impl_PASTE(a,b) a##b
#define _impl_CASSERT_LINE(predicate, line, file) \
    typedef char _impl_PASTE(assertion_failed_##file##_,line)[2*!!(predicate)-1];

#define RUNTIME_ASSERT( x ) configASSERT(x) /* Use FreeRTOS assertion */

// Arrays
#define LEN(array)       (sizeof((array)) / sizeof((array)[0]))

// Math
#define MIN(X, Y)        (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)        (((X) > (Y)) ? (X) : (Y))

// Timing
#define SYSTEM_TIME_US() portCPU_CLOCK_US() /* Use FreeRTOS CPU clock */

// CPU
#define CPU_SOFT_RESET()      vPrivilegedCPUReset()

// Data

/**
 * @brief Extract byte i from x where i = 0 is the least significant byte
 */
#define GET_BYTE(x, i)  (uint8_t)(((x) >> ((i) * 8U)) & 0xFF)

/**
 * @brief Construct a uint8_t with the nth bit set (all other bits cleared)
 */
#define UINT8_BIT(n)    ((uint8_t)((uint8_t)1U << (n)))

/**
 * @brief Construct a uint32_t with the nth bit set (all other bits cleared)
 */
#define UINT32_BIT(n)   ((uint32_t)((uint32_t)1U << (n)))

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// String Manipulation
int32_t cseq_to_num(char* seq);

// Timing
void busy_wait(uint32_t ticks_to_wait);
void obc_delay_us(uint32_t us);

#endif // OBC_UTILS_H
