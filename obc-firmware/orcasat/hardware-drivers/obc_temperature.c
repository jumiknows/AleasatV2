/*
 * obc_temp.c
 *
 *  Created on: Feb 4, 2019
 *      Author: Melvin Matthews
 */

#include "obc_temperature.h"
#include "sys_common.h"
#include "temp_stts75.h"
#include "tms_i2c.h"
#include "logger.h"
#include "gio.h"
#include "obc_rtos.h"

// TODO(ALEA-512): Rework driver

#define LAUNCHPAD_MOCK_TEMP 20 // temperature sensor is not available on launchpad

/**
 * @brief The main temperature reading function compatible with FreeRTOS
 *
 * @param [out] temp: pointer to store temperature in Celsius
 * @return temp_err_t: TEMP_SUCCESS if the temperature was read successfully, error code otherwise
 */
temp_err_t read_temp(int16_t* temp) {
#ifdef PLATFORM_LAUNCHPAD_1224
    *temp = LAUNCHPAD_MOCK_TEMP;
    return TEMP_SUCCESS;
#endif /* PLATFORM_LAUNCHPAD_1224 */
}

/**
 * @brief Executes the temperature sensor's self-test.
 *
 * @return temp_err_t: TEMP_SUCCESS if the self-test passed
 */
void self_test_temperature(void) {
#ifdef PLATFORM_LAUNCHPAD_1224
    log_str(ERROR, TEMP_LOG, true, "Mocked temp sensor check passed.");
#endif /* PLATFORM_LAUNCHPAD_1224 */
}

/**
 * @brief Hard reset of temperature sensor
 */
void reset_temp(void) {
#ifdef PLATFORM_LAUNCHPAD_1224
    return I2C_SUCCESS;
#endif /* PLATFORM_LAUNCHPAD_1224 */
}
