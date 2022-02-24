/*
 * obc_utils.h
 *
 *  Created on: April 3, 2017
 *      Author: steven
 */

#ifndef OBC_UTILS_H
#define OBC_UTILS_H

#include "sys_common.h"

/**
 * To work with arrays easier.
 */
#define LEN(array)                  (sizeof((array)) / sizeof((array)[0]))
#define FOR_EACH(it, array)         for ((it) = (array); (it) < &(array)[LEN(array)]; (it)++)
#define IS_OUT_OF_BOUNDS(it, array) ((it) >= &(array)[LEN(array)])
#define MIN(X, Y)                   (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)                   (((X) > (Y)) ? (X) : (Y))

/* Misc. Conversion */
char* itoa2(int32_t num, char* buffer, int32_t base, int32_t itr); // Integer to ASCII string
char* utoa2(uint32_t num, char* buffer, int32_t base,
            int32_t itr);                      // Unsigned integer to ASCII string
void clear_buffer(char* buf, uint32_t length); // Fill buffer with null characters

/* Misc. */
void busy_wait(uint32_t ticks_to_wait); // Quick n' dirty busy wait
void obc_delay_us(uint32_t us); //1 us/tick delay function
void restart_software(void);            // software reset

/* Math */
uint32_t power(uint32_t b, uint32_t e);
uint32_t cseq_to_num(char* seq);

#endif /* OBC_UTILS_H */
