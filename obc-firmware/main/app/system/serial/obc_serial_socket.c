/**
 * @file obc_serial_socket.c
 * @brief Socket-like streaming API for the debug serial port
 * 
 * This module is a temporary shim between obc_serial and the command system.
 * Once the comms stack is integrated with obc_serial this module will no longer
 * be necessary.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_serial_socket.h"

// OBC Serial
#include "obc_serial_rx.h"
#include "obc_serial_tx.h"

// Utils
#include "io_stream.h"
#include "buffered_io.h"
#include "rtos_stream.h"
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

// Input
static uint8_t input_buf[OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE] = { 0 };

static buffered_input_t buffered_input = {
    .size      = sizeof(input_buf),
    .buf       = input_buf,
    .input     = &obc_serial_comms_in,

    .start     = 0,
    .valid_len = 0,
};

// Output
static uint8_t output_buf[OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE] = { 0 };

static buffered_output_t buffered_output = {
    .size   = sizeof(output_buf),
    .buf    = output_buf,
    .output = &obc_serial_comms_out,

    .offset = 0,
};

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

const io_ostream_t obc_serial_socket_out = {
    .handle = &buffered_output,
    .write  = &buffered_io_write,
    .flush  = &buffered_io_flush,
};

const io_istream_t obc_serial_socket_in = {
    .handle = &buffered_input,
    .read   = &buffered_io_read,
};
