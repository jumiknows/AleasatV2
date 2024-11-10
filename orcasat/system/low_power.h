/*
 * low_power.h
 *
 *  Created on: Nov 4, 2018
 *      Author: Richard
 */

#ifndef LOW_POWER_H_
#define LOW_POWER_H_

void vApplicationIdleHook(void);
void enter_idle_sleep(void);

void idle_sleep_on(void);
void idle_sleep_off(void);

#endif /* LOW_POWER_H_ */
