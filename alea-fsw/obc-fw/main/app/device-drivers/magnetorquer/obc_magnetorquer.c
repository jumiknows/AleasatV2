/*
 * @brief Hardware drivers for initializing, starting, stopping, configuring direction and
 * changing the duty cycle of the magnetorquers.
 *
 * As of May 7, 2021, these functions are only tested on Launchpad platform
 *
 *  Created on: Jun 6, 2020
 *      Author: Caleb Gimpel
 */

#include "obc_magnetorquer.h"

/**
 * @brief Starts the magnetorquer PWM generation, based on the direction configured
 *   pwm_a = FORWARD
 *   pwm_b = BACKWARD
 * @param *mag: Pointer to the magnetorquer struct
 */
void mag_start(mag_t *mag) {
    if (mag->dir == FORWARD) {
        pwmStop(MAG_HET, mag->pwm_b);
        pwmStart(MAG_HET, mag->pwm_a);
    } else {
        pwmStop(MAG_HET, mag->pwm_a);
        pwmStart(MAG_HET, mag->pwm_b);
    }
}

/**
 * @brief Suspends the magnetorquer by stopping PWM generation.
 * @param *mag: Pointer to the magnetorquer struct
 */
void mag_stop(mag_t *mag) {
    pwmStop(MAG_HET, mag->pwm_a);
    pwmStop(MAG_HET, mag->pwm_b);
}

/**
 * @brief Configures the direction of the magnetorquer and starts PWM generation
 * @param *mag: Pointer to the magnetorquer struct
 * @param dir: Direction enum FORWARD|BACKWARD
 */
void mag_set_dir(mag_t *mag, mag_direction_t dir) {
    mag->dir = dir;
}

/**
 * @brief Sets the duty of the magnetorquer.
 * Setting duty does not start PWM generation by default.
 * @param *mag: Pointer to the magnetorquer struct
 * @param pwmDuty: PWM Duty Cycle
 */
void mag_set_duty(mag_t *mag, uint32_t pwm_duty) {
    pwmSetDuty(MAG_HET, mag->pwm_a, pwm_duty);
    pwmSetDuty(MAG_HET, mag->pwm_b, pwm_duty);
}

/**
 * @brief Enables power to all magnetorquers.
 *
*/
void mag_power_en(void) {
    obc_gpio_write(MAG_PWR_EN_PORT, MAG_PWR_EN_PIN, 1);
}

/**
 * @brief Disables power to all magnetorquers.
 *
*/
void mag_power_dis(void) {
    obc_gpio_write(MAG_PWR_EN_PORT, MAG_PWR_EN_PIN, 0);
}

/**
 * @brief Sets magnetorquer low power mode.
 *
 * @param mag: Magnetorquer to set.
 * @param sleep: Sleep mode to set to, options are SLEEP_ON|SLEEP_OFF
 *
*/
void mag_set_sleep(mag_t *mag, mag_sleep_t sleep) {
    obc_gpio_write(MAG_SLEEP_PORT, mag->sleep_pin, sleep);
}


/**
 * @brief Initializes the magnetorquer structs.
 * Sets the default direction as FORWARD and disables all PWM generation.
 */

mag_t magnetorquer_1 = {
    .pwm_a  = MAG1A_PWM_REG,
    .pwm_b  = MAG1B_PWM_REG,
    .dir    = FORWARD,
    .sleep  = SLEEP_ON,
    .sleep_pin  = MAG1_SLEEP_PIN
};

mag_t magnetorquer_2 = {
    .pwm_a  = MAG2A_PWM_REG,
    .pwm_b  = MAG2B_PWM_REG,
    .dir    = FORWARD,
    .sleep  = SLEEP_ON,
    .sleep_pin  = MAG2_SLEEP_PIN
};

mag_t magnetorquer_3 = {
    .pwm_a  = MAG3A_PWM_REG,
    .pwm_b  = MAG3B_PWM_REG,
    .dir    = FORWARD,
    .sleep  = SLEEP_ON,
    .sleep_pin  = MAG3_SLEEP_PIN
};
