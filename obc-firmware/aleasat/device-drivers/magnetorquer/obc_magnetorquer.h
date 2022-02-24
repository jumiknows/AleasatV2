/*
 * obc_magnetorquer.h
 *
 *  Created on: Jun 6, 2020
 *      Author: Caleb Gimpel
 */

#ifndef OBC_MAGNETORQUER_H_
#define OBC_MAGNETORQUER_H_
#include "het.h"
#include "obc_hardwaredefs.h"

typedef enum {
	FORWARD = 0,
	BACKWARD = 1
} mag_direction_t;

typedef enum {
	SLEEP_ON = 0,
	SLEEP_OFF = 1
} mag_sleep_t;

typedef struct {
	uint32_t pwm_a;
	uint32_t pwm_b;
	mag_direction_t dir;
	mag_sleep_t sleep;
	uint32_t sleep_pin;
} mag_t;

/**
 * @brief Magnetorquer PWM channel
 */
void mag_start(mag_t *mag);
void mag_stop(mag_t *mag);
void mag_set_dir(mag_t *mag, mag_direction_t dir);
void mag_set_duty(mag_t *mag, uint32_t pwm_duty);
void mag_power_en(void);
void mag_power_dis(void);
void mag_set_sleep(mag_t *mag, mag_sleep_t sleep);


extern mag_t magnetorquer_1;
extern mag_t magnetorquer_2;
extern mag_t magnetorquer_3;


#endif /* OBC_MAGNETORQUER_H_ */
