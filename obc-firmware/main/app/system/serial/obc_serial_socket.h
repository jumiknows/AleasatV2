/**
 * @file obc_serial_socket.h
 * @brief Socket-like streaming API for the debug serial port
 * 
 * This module is a temporary shim between obc_serial and the command system.
 * Once the comms stack is integrated with obc_serial this module will no longer
 * be necessary.
 */

#ifndef OBC_SERIAL_SOCKET_H_
#define OBC_SERIAL_SOCKET_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Utils
#include "io_stream.h"

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

extern const io_ostream_t obc_serial_socket_out;
extern const io_istream_t obc_serial_socket_in;

#endif // OBC_SERIAL_SOCKET_H_
