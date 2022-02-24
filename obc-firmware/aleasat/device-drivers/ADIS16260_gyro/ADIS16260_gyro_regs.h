/**
 * @file ADIS16260_gyro_regs.h
 * @brief File contains constants used in ADIS gyro drivers
 */

#ifndef ADIS16260_GYRO_REGS_H_
#define ADIS16260_GYRO_REGS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/
#include <stdint.h>


/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/
/* Device register addresses */
#define ADIS_FLASH_CNT_ADDR 0x00
#define ADIS_POWER_OUT_ADDR 0x02
#define ADIS_RATE_ADDR 0x04
#define ADIS_TEMP_ADDR 0x0C
#define ADIS_ANGL_ADDR 0x0E
#define ADIS_OFFSET_CNT_ADDR 0x14
#define ADIS_SCALE_CNT_ADDR 0x16
#define ADIS_MISC_CNT_ADDR 0x34
#define ADIS_SMPL_PERIOD_ADDR 0x36
#define ADIS_SENS_AVG_ADDR 0x38
#define ADIS_SENS_AVG_ADDR_UPPER 0x39
#define ADIS_SLEEP_CNT_ADDR 0x3A
#define ADIS_DIAG_STAT_ADDR 0x3C
#define ADIS_GLOB_CMD_ADDR 0x3E

#define ADIS_LOTID_1_ADDR 0x52
#define ADIS_LOTID_2_ADDR 0x54
#define ADIS_PROD_ID_ADDR 0x56
#define ADIS_SERIAL_NUM_ADDR 0x58

/* Device bitmasks */
#define ADIS_NEW_DATA ((uint16_t)1U << 15)
#define ADIS_ERROR_ALARM ((uint16_t)1U << 14)
#define ADIS_MAX_SIGNED_12 2047
#define ADIS_SET_WRITE_BIT 0x8000
#define ADIS_DATA_ADDRESS_SHIFT 8

/* Device write constants */
#define ADIS_SOFT_RESET 0x80

/* Other constants */
#define ADIS_DEVICE_ID_WORD_SIZE 4
#define ADIS_GYRO_SCALE_DEG_LSB 0.00048828F
#define ADIS_GYRO_OFFSET_DEG_LSB 0.018315F
#define ADIS_GYRO_DEG_LSB 0.07326F
#define ADIS_TEMP_DEG_LSB 0.1453F
#define ADIS_TEMP_DEFAULT_OFFSET 25.0F
#define ADIS_SLP_TICKS_MS 500
#define ADIS_GYRO_DEFAULT_SCALE 0x0800

/* Signal processing bitmasks */
#define ADIS_RANGE_320 0b100
#define ADIS_RANGE_160 0b010
#define ADIS_RANGE_80 0b001
#define ADIS_BW_330 0b10000000
#define ADIS_BW_50 0b00000000


#endif //ADIS16260_GYRO_REGS_H_
