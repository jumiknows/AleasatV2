/**
 * @file imu_bmx160.h
 * @brief Prototypes and typedefs for BMX160 IMU driver
 */

#ifndef IMU_BMX160_H_
#define IMU_BMX160_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

//OBC
#include "imu_bmx160_registers.h"
#include "obc_task_info.h"
#include "logger.h"
#include "obc_gpio.h"
#include "obc_featuredefs.h"

//FreeRTOS
#include "i2c_freertos.h"

//HAL
#include "sys_common.h"


/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief bmx160 sensor status enum
 *
 * Allows device to throw error when reads/writes go wrong
 */
typedef enum {

  IMU_SUCCESS,
  IMU_ERROR

} imu_error_t;


/**
 * @brief bmx160 sensor settings struct
 *
 * The same settings apply for Magnetometer, Accelerometer and Gyro
 *
 * Power mode possible configurations:
 *       Accelerometer: Normal, Low Power
 *       Gyroscope: Normal, Fast Start Up, Suspend
 *       Magnetometer: Suspend, Normal, Low Power
 *
 * Output data possible rates (Hz):
 *      Accelerometer/Magnetometer: 25/32, 25/16, 100, 800, 1600
 *      Gyroscope: 25, 100, 1600, 3200
 */
typedef struct {

  /* power mode */
  uint8_t power_state;

  /* output data rate */
  uint8_t odr;

  /* bandwidth */
  uint8_t bw;

  /* range */
  float32 range;

} bmx160_cfg_t;

/**
 * @brief bmx160 sensor data structure
 *
 * Contains x,y,z 16 bit data values
 */
typedef struct  {
  /* X-axis sensor processed data */
  float32 x_proc;
  /* Y-axis sensor processed data */
  float32 y_proc;
  /* Z-axis sensor processed data */
  float32 z_proc;
  /* X-axis raw sensor data */
  int16_t x_raw;
  /* Y-axis raw sensor data */
  int16_t y_raw;
  /* Z-axis raw sensor data */
  int16_t z_raw;

} bmx160_sensor_data_t;

/**
 * @brief Magnetometer trimming data collected at device initialization time
 */
typedef struct {

  int8_t dig_x1;
  int8_t dig_y1;
  int8_t dig_x2;
  int8_t dig_y2;

  uint8_t dig_xy1;
  int8_t dig_xy2;

  uint16_t dig_z1;
  int16_t dig_z2;
  int16_t dig_z3;
  int16_t dig_z4;

  uint16_t dig_xyz1;

} bmx160_trimming_t;


/**
 * @brief Reponse data packet containing magnetometer, gyroscope and accelerometer data
 */
typedef struct{

  /**
   * Data structs for each sensor type
  */
  bmx160_sensor_data_t mag, gyro, accel;

  /**
   *  Rhall raw sensor data
   */
  int16_t rhall;

} bmx160_data_t;


/**
 * @brief bmx160 complete settings and data struct
 */
typedef struct{

  /**
   * Magnetometer, Gyroscope and Accelerometer settings
   *
   * */
  bmx160_cfg_t gyro_settings, accel_settings, mag_settings;

  /**
   * Device I2C address
   *
   * */
  uint8_t addr;

  /**
   * Device GPIO output port
   *
  */
  gpio_port_t en_port;

  /**
   * Device GPIO output pin
   *
  */
  uint8_t en_pin;

  /**
   * Device master power status
  * */
  bool is_active;

  /**
   * Calibration offset to apply to sensor data
   * !!This field will be set to 1 by default but should be set somwhere in the ADCS system!!
  */
  float32 mag_cal, gyro_cal, accel_cal;

  /*Trimming data for magnetometer adjustment*/
  bmx160_trimming_t trimming_data;

} bmx160_t;


/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

extern bmx160_t bmx160_imu_1;
#if IMU_2_EN
extern bmx160_t bmx160_imu_2;
#endif // IMU_2_EN


/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

imu_error_t bmx160_soft_reset(bmx160_t* imu);
imu_error_t bmx160_set_low_power(bmx160_t* imu);
imu_error_t bmx160_wake_up(bmx160_t* imu);
imu_error_t bmx160_enable_imu(bmx160_t* imu);
imu_error_t bmx160_disable_imu(bmx160_t* imu);
imu_error_t bmx160_set_power_mode(bmx160_t* imu, uint8_t power_mode);
imu_error_t bmx160_set_gyro_range(bmx160_t* imu, uint8_t new_gyro_range);
imu_error_t bmx160_set_accel_range(bmx160_t* imu, uint8_t new_accel_range);
imu_error_t bmx160_get_data(bmx160_t* imu, bmx160_data_t* data_out);


/*IMU_BMX160_H_*/
#endif
