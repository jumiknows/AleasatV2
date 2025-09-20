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
 * @file timers.h
 * @brief Header file for the system timers
 * @author Timothy Wriglesworth
 * @date May 14, 2021
 */

#ifndef _TIMERS_H
#define _TIMERS_H

#include <cc1110.h>
#include <stdint.h>

// These are provided in assembly to prevent the compiler
// from reordering them
/**
 * @brief These are provided in assembly to prevent the compiler
 * from reordering them
 */
#define TIMER_INTERRUPTS_ENABLE __asm \
    setb _T1IE \
    __endasm

/**
 * @brief Constant for 1e9/27e6 = 37 seconds
 *
 * @warning Changing F_CLK will give a different period
 */
#define TIMER_INTERRUPTS_DISABLE __asm \
    clr _T1IE \
    __endasm


/**
 * @brief 1ms period
 *
 * @warning Changing F_CLK will give a different period
 */
#define T1_PERIOD (F_CLK / 1000)
#define T1_TICK (1000000000 / F_CLK)
/**
 * @brief 100ms counter variable
 */
#define TIMER_COUNT_PERIOD 100

/**
 * @brief Time given in seconds and nanoseconds
 */
typedef struct {
    uint32_t seconds;
    uint32_t nanoseconds;
} timespec_t;

void timers_init(void);
void timers_get_time(__xdata timespec_t *t);
uint32_t timers_get_seconds(void);
void timers_set_time(const __xdata timespec_t *t);
void timers_add_time(__xdata timespec_t *t1, __xdata timespec_t *t2);
void timers_subtract_time(__xdata timespec_t *t1, __xdata timespec_t *t2);
void timers_watch_for_RF(void);
void timers_trigger_for_RF(void);

void t1_isr(void)  __interrupt (T1_VECTOR) __using (1);

extern volatile __bit rtc_set;
extern volatile __data uint32_t uptime;
extern volatile __data uint16_t timer_count_ms;

#endif
