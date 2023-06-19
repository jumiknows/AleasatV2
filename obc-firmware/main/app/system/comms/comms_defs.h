/**
 * @file comms_defs.h
 * @brief Comms common definitions
 */

#ifndef COMMS_DEFS_H_
#define COMMS_DEFS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_api.h"
#include "comms_device.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/


#define COMMS_MAX_NUM_ENDPOINTS 3
#define COMMS_ESP_NUM_BYTES 2
#define COMMS_ESP_START_BYTE_0 0x22           /** First start byte  */
#define COMMS_ESP_START_BYTE_1 0x69           /** Second start byte  */

/**
 * @brief Maximum size of an OpenLST packet, including ESP header bytes and length byte
 * @ref https://drive.google.com/file/d/1Az8ncSEAZElHQlV8OGT50WNs_bD_RgJ_/view?USP=sharing
 * @ref https://gitlab.com/alea-2020/communications/comms-firmware-openlst-1/-/blob/dev2/open-lst/common/commands.h
 */
#define COMMS_MIN_PKT_SIZE_BYTES    10   // ESP(2) + len(1) + seqnum(2) + dest_hwid(2) + src_hwid(2) + cmd(1)
#define COMMS_MAX_PKT_SIZE_BYTES    (COMMS_MAX_CMD_PAYLOAD_NUM_BYTES + COMMS_MIN_PKT_SIZE_BYTES)
#define COMMS_PKT_START_NUM_BYTES    3    // ESP(2) + len(1)

#define COMMS_MSG_HEADER_SIZE_BYTES    (COMMS_MIN_PKT_SIZE_BYTES - COMMS_PKT_START_NUM_BYTES)

#define COMMS_MAX_PKT_LEN_VAL    (COMMS_MAX_PKT_SIZE_BYTES - COMMS_PKT_START_NUM_BYTES)
#define COMMS_MIN_PKT_LEN_VAL    (COMMS_MIN_PKT_SIZE_BYTES - COMMS_PKT_START_NUM_BYTES)

#define OBC_HWID    0x7000
#define COMMS_HWID  0x0107
#define GROUND_HWID 0x8000

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef uint16_t hwid_t;

typedef struct comms_command_header_struct {
    uint16_t seqnum;  // only lower 15 bits is valid, do not use first bit
    hwid_t dest_hwid;
    hwid_t src_hwid;
    uint8_t command;
    uint8_t is_response;  // only last bit is valid, do not use upper 7 bits
} comms_command_header_t;

typedef struct comms_command_struct {
    comms_command_header_t header;
    uint8_t data[COMMS_MAX_CMD_PAYLOAD_NUM_BYTES];
    uint8_t data_len;  // number of bytes in data, actual length byte is calculated when converting to array for sending
} comms_command_t;


typedef enum {
  COMMS_COMMON_MSG_ACK = 0x10,
  COMMS_COMMON_MSG_NACK = 0xff,
  COMMS_COMMON_MSG_ASCII = 0x11,
  COMMS_CUSTOM_MSG_OBC_DATA = 0x60,
  COMMS_CUSTOM_ALEA_ACK = 0xee
} comms_common_msg_no_t;

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
    COMMS_RADIO_MSG_START        = 0x1e,
    COMMS_RADIO_MSG_REBOOTING    = 0x1f,
    COMMS_ALEA_RADIO_MSG_GET_RADIOTELEM     = 0x21,
    COMMS_ALEA_RADIO_MSG_RADIOTELEM         = 0x22,
    COMMS_ALEA_RADIO_MSG_SET_RADIOTELEM     = 0x23,
    COMMS_ALEA_RADIO_MSG_RESET_RADIOTELEM   = 0x24
} comms_radio_msg_no_t;

typedef enum {
    // Older versions of the bootloader used
    // separate command codes for serial connections
    // and radio connections.
    // Both are included here for compatiblity
    COMMS_BOOTLOADER_MSG_PING        = 0x00,
    COMMS_BOOTLOADER_MSG_ERASE       = 0x0c,
    COMMS_BOOTLOADER_MSG_WRITE_PAGE  = 0x02,
    COMMS_BOOTLOADER_MSG_ACK         = 0x01,
    COMMS_BOOTLOADER_MSG_NACK        = 0x0f,
    COMMS_BOOTLOADER_MSG_START       = 0x0e
} comms_bootloader_msg_no_t;

/******************************************************************************/
/*                  F U N C T I O N  P R O T O T Y P E S                      */
/******************************************************************************/

comms_err_t comms_mngr_send_cmd(
    comms_command_t* cmd
);

void comms_service_packet_failure(
    comms_command_t* cmd
);

void comms_service_packet_input(
    comms_command_t* cmd_in
);

void comms_mngr_start_task(
    comms_dev_handle_t cdev_hdl
);

void comms_mngr_create_infra(
    void
);

#endif /* COMMS_DEFS_H_ */
