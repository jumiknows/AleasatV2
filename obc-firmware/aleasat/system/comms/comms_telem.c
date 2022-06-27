/**
 * @file comms_telem.c
 * @brief Implementation for comms interface commands
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Comms
#include "comms_telem.h"
#include "comms_defs.h"
#include "comms_cmd.h"
#include "obc_comms.h"

// OBC
#include "logger.h"

// Standard library
#include <stdint.h>
#include <string.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef union int_16bit_union {
    uint16_t int16_u;
    int16_t  int16;
} int_16bit_t;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static int16_t get_16bit_int(uint8_t* bytes, uint8_t* i);
static uint32_t get_32bit_uint(uint8_t* bytes, uint8_t* i);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Gets telemetry from OpenLST via SPI+
 *
 * @param[out] telem_recv
 *
 * @return comms_err_t indicating if telemetry was successfully acquired and
 *         returned via telem_recv
 */
comms_err_t comms_get_telem(comms_telem_t* telem_recv) {
    comms_err_t err;
    comms_command_t resp = { 0 };

    err = comms_send_recv_cmd(comms_hwid, COMMS_RADIO_MSG_GET_TELEM, NULL, 0, &resp, COMMS_MIBSPI_MUTEX_TIMEOUT_MS);

    if (err == COMMS_SUCCESS) {
        uint8_t ind_data = 0;
        uint8_t ind_adc;

        // Parse bytes from SPI response
        telem_recv->reserved = resp.data[ind_data++];
        telem_recv->uptime = get_32bit_uint(resp.data, &ind_data);
        telem_recv->uart0_rx_count = get_32bit_uint(resp.data, &ind_data);
        telem_recv->uart1_rx_count = get_32bit_uint(resp.data, &ind_data);
        telem_recv->rx_mode = resp.data[ind_data++];
        telem_recv->tx_mode = resp.data[ind_data++];

        for (ind_adc = 0; ind_adc < COMMS_ADC_NUM_CHANNELS; ind_adc++) {
            telem_recv->adc[ind_adc] = get_16bit_int(resp.data, &ind_data);
        }

        telem_recv->rssi = (int8_t)resp.data[ind_data++];
        telem_recv->last_lqi = resp.data[ind_data++];
        telem_recv->last_frequest = (int8_t)resp.data[ind_data++];
        telem_recv->packets_sent = get_32bit_uint(resp.data, &ind_data);
        telem_recv->cs_count = get_32bit_uint(resp.data, &ind_data);
        telem_recv->packets_good = get_32bit_uint(resp.data, &ind_data);
        telem_recv->packets_rejected_checksum = get_32bit_uint(resp.data, &ind_data);
        telem_recv->packets_rejected_reserved = get_32bit_uint(resp.data, &ind_data);
        telem_recv->packets_rejected_other = get_32bit_uint(resp.data, &ind_data);
        telem_recv->reserved0 = get_32bit_uint(resp.data, &ind_data);
        telem_recv->reserved1 = get_32bit_uint(resp.data, &ind_data);
        telem_recv->custom0 = get_32bit_uint(resp.data, &ind_data);
        telem_recv->custom1 = get_32bit_uint(resp.data, &ind_data);
    }

    return err;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Converts 2 indices (stored in big endian) of a uint8 array to a single int16
 *
 * @param[in] bytes: The starting address of an array of bytes
 * @param[out] i: The starting index within the bytes array of the desired number (updated after operation)
 *
 * @return int16_t containing the converted integer
 */
static int16_t get_16bit_int(uint8_t* bytes, uint8_t* i) {
    int_16bit_t num;

    num.int16_u = ( (uint16_t) ((uint16_t) bytes[(*i) + 1] << 8) ) | (uint16_t) bytes[(*i)];
    (*i) += 2;

    return num.int16;
}

/**
 * @brief Converts 4 indices (stored in big endian) of a uint8 array to a single uint32
 *
 * @param[in] bytes: The starting address of an array of bytes
 * @param[out] i: The starting index within the bytes array of the desired number (updated after operation)
 *
 * @return uint32_t containing the converted integer
 */
static uint32_t get_32bit_uint(uint8_t* bytes, uint8_t* i) {
    uint32_t num32 = ( (uint32_t) bytes[(*i) + 3] << 24 ) | ( (uint32_t) bytes[(*i) + 2] << 16 )
                    | ( (uint32_t) bytes[(*i) + 1] << 8 ) | (uint32_t) bytes[(*i)];
    (*i) += 4;

    return num32;
}
