/*
 * temp_stts75.h
 *
 *  Created on: Feb 4, 2019
 *      Author: Melvin
 */
#ifndef ORCASAT_HARDWARE_DRIVERS_TEMP_STTS75_H_
#define ORCASAT_HARDWARE_DRIVERS_TEMP_STTS75_H_

#include "obc_temperature.h"
#include "sys_common.h"
#include "obc_temperature.h"

/*
 * Registers
 */
#define STTS75_TEMPERATURE   0x00U
#define STTS75_CONFIGURATION 0x01U
#define STTS75_HYSTERESIS    0x02U
#define STTS75_OVERTEMP      0x03U

#define NUM_BYTES_CONFIG 1
#define NUM_BYTES_TEMP   2

/**
 * @brief Configuration registers of the stts75
 *
 */
typedef enum { STTS75_SD = 0x01, STTS75_M = 0x02, STTS75_POL = 0x04, STTS75_FT = 0x18, STTS75_RC = 0x60, STTS75_OSM = 0x80 } stts75_config_reg;

/**
 * @brief configuration offsets
 */
typedef enum { STTS75_OFFSET_SD = 0U, STTS75_OFFSET_M = 1U, STTS75_OFFSET_POL = 2U, STTS75_OFFSET_FT = 3U, STTS75_OFFSET_RC = 5U, STTS75_OFFSET_OSM = 7U } stts75_config_offset;

/**
 * @brief allowable resolutions for temperature in the stts75
 *
 * Ranges from 9-12 bits
 */
typedef enum {
    STTS75_9bit  = 0,
    STTS75_10bit = 1,
    STTS75_11bit = 2,
    STTS75_12bit = 3,
} stts75_resolution_t;

/**
 * @brief number of faults (error conditions) before user gets notified
 *
 * This feature is used to filter noise
 * Temperature sensor must get # of fault results outside of expected range
 * before notifying the user (triggering the INT output)
 */
typedef enum {
    one_fault  = 0,
    two_fault  = 1,
    four_fault = 2,
    six_fault  = 3,
} stts75_fault_t;

/**
 * @brief temp sensor mode
 *
 * continuous conversation -  the temperature is continually updated
 * shutdown - the conversion in progress is completed, then the stts75 goes into low power mode
 * one-shot - converts temp once
 */
typedef enum {
    continuous_conversion = 0,
    shutdown              = 1,
    one_shot              = 2,
} stts75_mode_t;

i2c_err_t stts75_ping(void);

temp_err_t stts75_temp_get(int16_t* val);
temp_err_t stts75_mode_get(stts75_mode_t* val);
temp_err_t stts75_mode_set(stts75_mode_t val);
temp_err_t stts75_resolution_get(stts75_resolution_t* val);
temp_err_t stts75_resolution_set(stts75_resolution_t val);
temp_err_t stts75_fault_get(stts75_fault_t* val);
temp_err_t stts75_fault_set(stts75_fault_t val);
temp_err_t stts75_hysteresis_get(int16_t* val);
temp_err_t stts75_hysteresis_set(int16_t hyst_deg_c);
temp_err_t stts75_overtemp_get(int16_t* overtemp_deg_c);
temp_err_t stts75_overtemp_set(int16_t overtemp_deg_c);

void stts75_self_test(void);
void stts75_check_registers(void);

// for use by temp_stts75_selftest.c
temp_err_t stts75_read_reg(uint8_t reg, uint8_t* data_rcv, uint8_t len);
temp_err_t stts75_read_config_reg(uint8_t* data_rcv);
temp_err_t stts75_write_config_reg(uint8_t reg_value);

#endif /* ORCASAT_HARDWARE_DRIVERS_TEMP_STTS75_H_ */
