/**
 * @file gps_internals.h
 * @brief internal components of the GPS Driver. All RTOS related components are in obc_gps.h.
 */

#ifndef GPS_INTERNALS_H_
#define GPS_INTERNALS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "gps_defs.h"

// std lib
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

uint8_t calculate_ctrl_msg_checksum(uint8_t *msg, uint8_t size);
bool is_pkts_checksum_valid(uint8_t *msg, uint16_t size);
gps_parsing_err_t parse_control_packet(uint8_t *data, uint8_t data_len, gps_packet_t *packet);
gps_sys_msg_output_t get_packet_type(const uint8_t *msg, const uint16_t data_len);
bool is_ack_pkt(uint8_t *msg, uint16_t data_length);

#endif /* GPS_INTERNALS_H_ */
