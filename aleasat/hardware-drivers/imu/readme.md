# ALEASAT IMU DRIVERS

This driver allows the TMS570 based OBC/Launchpad platforms interface with the Bosch-Sensortec BMX160 IMU over I2C.
The OBC contains two IMU's for redundancy, only one IMU is to be enabled and used at once via GPIO.

IMU contains 3 axis for magnetometer, gyroscope and accelerometer. By default, magnetometer units are in uT, gyro units are in degrees/second and accelerometer units are in g's.

## Avaliable API:

bmx160_soft_reset -> Perform software reset on device \
bmx160_set_low_power -> Set a device to lowest power consumption mode \
bmx160_wake_up -> Wake device out of low power consumption mode \
bmx160_enable_imu -> Enable IMU on OBC hardware (enable power gate MOSFET to device) \
bmx160_disable_imu -> Disable IMU on OBC hardware (disable power gate MOSFET to device) \
bmx160_set_power_mode -> Set induvidual sensor power mode (i.e magnetometer, accelerometer, gyro) \
bmx160_set_gyro_range -> Set gyro sensitivity range, options found in imu_bmx160_registers.h \
bmx160_set_accel_range -> Set accelerometer sensitivity range, options found in imu_bmx160_registers.h \
bmx160_get_data -> Read IMU data

## Important notes:

 - IMU data is returned in bmx160_data_t datapackets which can be found in imu_bmx160.h
 - All API functions return imu_error_t enumerated type