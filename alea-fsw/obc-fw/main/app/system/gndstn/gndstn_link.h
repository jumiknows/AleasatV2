/**
 * @file gndstn_link.h
 * @brief API for the uplink/downlink between the OBC app and the ground station
 */

#ifndef GNDSTN_LINK_H_
#define GNDSTN_LINK_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Utils
#include "io_stream.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

extern const io_istream_t gndstn_uplink_socket;
extern const io_ostream_t gndstn_downlink_socket;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void gndstn_link_pre_init(void);

#endif // GNDSTN_LINK_H_
