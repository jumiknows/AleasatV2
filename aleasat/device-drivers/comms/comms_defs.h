/**
 * @file comms_defs.h
 * @brief Comms common definitions
 */

#ifndef COMMS_DEFS_H_
#define COMMS_DEFS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define COMMS_ESP_START_BYTE_0 0x22           /** First start byte  */
#define COMMS_ESP_START_BYTE_1 0x69           /** Second start byte  */

/**
 * @brief Maximum size of an OpenLST packet, including ESP header bytes and length byte
 * @ref https://drive.google.com/file/d/1Az8ncSEAZElHQlV8OGT50WNs_bD_RgJ_/view?USP=sharing
 * @ref https://gitlab.com/alea-2020/communications/comms-firmware-openlst-1/-/blob/dev2/open-lst/common/commands.h
 */
#define COMMS_MAX_MSG_SIZE_BYTES    254
#define COMMS_MIN_MSG_SIZE_BYTES    9  // ESP(2) + len(1) + hwid(2) + seqnum(2) + system(1) + cmd(1)
#define COMMS_MSG_HEADER_SIZE_BYTES 3  // ESP(2) + len(1)

// hwid(2) + seqnum(2) + system(1) + cmd(1)
#define COMMS_PKT_HEADER_SIZE_BYTES    (COMMS_MIN_MSG_SIZE_BYTES - COMMS_MSG_HEADER_SIZE_BYTES)

#define COMMS_MAX_PKT_LEN_VAL    (COMMS_MAX_MSG_SIZE_BYTES - COMMS_MSG_HEADER_SIZE_BYTES)
#define COMMS_MIN_PKT_LEN_VAL    (COMMS_MIN_MSG_SIZE_BYTES - COMMS_MSG_HEADER_SIZE_BYTES)

// cmd data can be from 0 to 245 bytes
#define COMMS_MAX_CMD_DATA_SIZE_BYTES    (COMMS_MAX_MSG_SIZE_BYTES - COMMS_MIN_MSG_SIZE_BYTES)

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum comms_err_type {
    /**
     * @brief No error
     */
    COMMS_SUCCESS = 0,

    /**
     * @brief Comms MIBSPI transaction event group timed out
     */
    COMMS_MIBSPI_TIMEOUT = -1,

    /**
     * @brief Comms MIBSPI transaction error
     */
    COMMS_MIBSPI_ERR = -2,

    /**
     * @brief Comms MIBSPI mutex timed out
     */
    COMMS_MIBSPI_MUTEX_TIMEOUT = -3,

    /**
     * @brief Comms response parameters mutex timed out
     */
    COMMS_RESPONSE_MUTEX_TIMEOUT = -4,

    /**
     * @brief Comms receiver waiting for response message timed out
     */
    COMMS_RECEIVER_EVTGRP_TIMEOUT = -5,

    /**
     * @brief OpenLST message ESP bytes incorrect
     */
    COMMS_MSG_ESP_MISMATCH_ERR = -6,

    /**
     * @brief OpenLST packet length byte out of range
     */
    COMMS_PKT_LEN_ERR = -7,

    /**
     * @brief Comms unknown error
     */
    COMMS_UNKNOWN_ERR = -8
} comms_err_t;

typedef uint16_t comms_hwid_t;

typedef struct comms_command_header_struct {
    comms_hwid_t hwid;
    uint16_t seqnum;
    uint8_t system;
    uint8_t command;
} comms_command_header_t;

typedef struct comms_command_struct {
    comms_command_header_t header;
    uint8_t data[COMMS_MAX_CMD_DATA_SIZE_BYTES];
    uint8_t data_len;  // number of bytes in data
} comms_command_t;


typedef enum {
  COMMS_COMMON_MSG_ACK = 0x10,
  COMMS_COMMON_MSG_NACK = 0xff,
  COMMS_COMMON_MSG_ASCII = 0x11,
  COMMS_CUSTOM_ALEA_ACK = 0xee
} comms_common_msg_no_t;


typedef enum {
    // Older versions of the bootloader used
    // separate command codes for serial connections
    // and radio connections.
    // Both are included here for compatiblity
    COMMS_BOOTLOADER_MSG_PING        = 0x00,
    COMMS_BOOTLOADER_MSG_ERASE       = 0x0c,
    COMMS_BOOTLOADER_MSG_WRITE_PAGE  = 0x02,
    COMMS_BOOTLOADER_MSG_ACK         = 0x01,
    COMMS_BOOTLOADER_MSG_NACK        = 0x0f
} comms_bootloader_msg_no_t;


typedef enum {
    COMMS_RADIO_MSG_REBOOT       = 0x12,
    COMMS_RADIO_MSG_GET_TIME     = 0x13,
    COMMS_RADIO_MSG_SET_TIME     = 0x14,
    COMMS_RADIO_MSG_RANGING      = 0x15,
    COMMS_RADIO_MSG_RANGING_ACK  = 0x16,
    COMMS_RADIO_MSG_GET_TELEM    = 0x17,
    COMMS_RADIO_MSG_TELEM        = 0x18,
    COMMS_RADIO_MSG_GET_CALLSIGN = 0x19,
    COMMS_RADIO_MSG_SET_CALLSIGN = 0x1a,
    COMMS_RADIO_MSG_CALLSIGN     = 0x1b,
    COMMS_ALEA_RADIO_MSG_GET_RADIOTELEM     = 0x21,
    COMMS_ALEA_RADIO_MSG_RADIOTELEM         = 0x22,
    COMMS_ALEA_RADIO_MSG_SET_RADIOTELEM     = 0x23,
    COMMS_ALEA_RADIO_MSG_RESET_RADIOTELEM   = 0x24
} comms_radio_msg_no_t;

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

extern comms_hwid_t comms_hwid;
extern uint8_t comms_system;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void set_comms_hwid(comms_hwid_t hwid);
void set_comms_system(uint8_t system);

#endif /* COMMS_DEFS_H_ */
