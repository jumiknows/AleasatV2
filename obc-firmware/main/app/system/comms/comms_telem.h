/**
 * @file comms_telem.h
 * @brief Comms interface command definitions
 */

#ifndef COMMS_TELEM_H_
#define COMMS_TELEM_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Comms
#include "comms_api.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define COMMS_ADC_NUM_CHANNELS 10 // From open-lst/radio/telemetry.h

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct comms_telem_struct {
    uint8_t  reserved;
    uint32_t uptime;
    uint32_t uart0_rx_count;
    uint32_t uart1_rx_count;
    uint8_t  rx_mode;
    uint8_t  tx_mode;
    int16_t  adc[COMMS_ADC_NUM_CHANNELS];
    int8_t   rssi;
    uint8_t  last_lqi;
    int8_t   last_frequest;
    uint32_t packets_sent;
    uint32_t cs_count;
    uint32_t packets_good;
    uint32_t packets_rejected_checksum;
    uint32_t packets_rejected_reserved;
    uint32_t packets_rejected_other;
    uint32_t reserved0;
    uint32_t reserved1;
    uint32_t custom0;
    uint32_t custom1;
} comms_telem_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

comms_err_t comms_get_telem(comms_session_handle_t session_handle, comms_telem_t *telem_recv);

#endif // COMMS_TELEM_H_
