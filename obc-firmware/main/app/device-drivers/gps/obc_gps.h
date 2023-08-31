/**
 * @file obc_gps.h
 * @brief Venus838FLPx_DS_v& GPS receiver driver.
 */

#ifndef OBC_GPS_H_
#define OBC_GPS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// GeeePeeEss
#include "gps_defs.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_rtc.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/**
 * GPS receiver configuration functions. They send configuration commands to the
 * GPS receiver in SkyTraq binary format, and check for the corresponding ack or
 * nack response.
 */

void gps_init(void);
void gps_create_infra(void);
gps_err_t gps_handle_control_msg(uint8_t* buf, uint16_t length);
gps_err_t gps_restart_receiver(gps_restart_modes_t restart_mode);
gps_err_t gps_query_software_version(uint8_t* sw_version);
gps_err_t gps_set_factory_settings(void);
gps_err_t gps_query_crc_info(uint16_t*  sw_crc);
gps_err_t gps_configure_serial_port(gps_config_serial_baud_t baud_rate);
gps_err_t gps_configure_nmea_messages(uint8_t gga, uint8_t gsa, uint8_t gsv, uint8_t gll, uint8_t rmc, uint8_t vtg, uint8_t zda);
gps_err_t gps_query_nmea_messages(uint8_t* gga, uint8_t* gsa, uint8_t* gsv, uint8_t* gll, uint8_t* rmc, uint8_t* vtg, uint8_t* zda);
gps_err_t gps_configure_msg_type(gps_msg_type_t msg_type);
gps_err_t gps_configure_power_mode(gps_write_method_t gps_write_method, bool power_save);
gps_err_t gps_query_power_mode(bool* power_save);
gps_err_t gps_configure_system_postion_rate(gps_pos_update_rate_t pos_update_rate);
gps_err_t gps_query_system_postion_rate(uint8_t* pos_update_rate);
gps_err_t gps_configure_nav_message_interval(gps_write_method_t gps_write_method, uint8_t nav_msg_interval);

#endif /* OBC_GPS_H_ */
