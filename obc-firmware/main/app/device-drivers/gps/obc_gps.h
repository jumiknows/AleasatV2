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
#include "tms_uart.h"
#include "obc_rtc.h"

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/**
 * GPS receiver configuration functions. They send configuration commands to the
 * GPS receiver in SkyTraq binary format, and check for the corresponding ack or
 * nack response.
 */

gps_err_t gps_restart_receiver(gps_restart_modes_t restart_mode);

#endif /* OBC_GPS_H_ */
