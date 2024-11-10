/**
 * @file obc_gps.h
 * @brief OEM7X GPS module driver
 * @author Andres Martinez
 * @date May 13, 2020
 */

#ifndef OBC_GPS_H
#define OBC_GPS_H

#include "obc_uart.h"
#include "oem7x_parse.h"

void gps_create_infra(void);
void gps_reset(void);
void gps_set_power(bool is_on);

gps_req_err_t gps_req_xyz(uint16_t timeout_s, gps_ecef_t* ec);
gps_req_err_t gps_req_time(uint16_t timeout_s, gps_time_t* ts);

gps_req_err_t gps_cmd(const char* req_msg, char* resp_msg);

#endif
