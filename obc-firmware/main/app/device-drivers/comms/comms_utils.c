/**
 * @file comms_utils.h
 * @brief Comms related utility functions
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_defs.h"
#include "comms_utils.h"

// Standard Library
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static comms_err_t comms_check_buffer(const uint8_t* buf);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Checks validity of command struct
 * TODO: check hwids, cmd_num
 *
 * @param[in] cmd_struct
 *
 * @return comms_err_t indicating if given command struct is valid or not
 */
comms_err_t comms_check_cmd_struct(comms_command_t* cmd_struct) {
    if (cmd_struct->data_len > COMMS_MAX_CMD_PAYLOAD_NUM_BYTES) {
        return COMMS_PKT_LEN_ERR;
    }

    if (cmd_struct->header.seqnum > 0x7FFF) {
        return COMMS_BAD_SEQNUM_ERR;
    }

    if (cmd_struct->header.is_response > 1) {
        return COMMS_BAD_RESPONSE_BIT_FIELD_ERR;
    }

    return COMMS_SUCCESS;
}

/**
 * @brief Converts buffer to command struct
 *
 * @param[in] buf
 * @param[out] cmd_struct
 *
 * @return comms_err_t indicating if conversion is successful or not
 */
comms_err_t comms_buffer_to_cmd_struct(const uint8_t* buf, comms_command_t* cmd_struct) {
    const uint8_t* bufp = &buf[COMMS_ESP_NUM_BYTES];
    comms_err_t buf_err;

    buf_err = comms_check_buffer(buf);
    if (buf_err != COMMS_SUCCESS) {
        return buf_err;
    }

    // subtract off bytes in header and we are left with bytes in data buffer
    cmd_struct->data_len = bufp[0] - COMMS_MSG_HEADER_SIZE_BYTES;
    bufp += 1;

    // Response bit is MSBit of the two bytes that seqnum is in
    // and seqnum is the LS 15 bits
    cmd_struct->header.seqnum = ((uint16_t)bufp[0]) | ((uint16_t)(((uint16_t)bufp[1]) << 8));
    cmd_struct->header.is_response = (uint8_t)((cmd_struct->header.seqnum & 0x8000) != 0);
    cmd_struct->header.seqnum &= 0x7FFF;
    bufp += 2;

    cmd_struct->header.dest_hwid = ((uint16_t)bufp[0]) | ((uint16_t)(((uint16_t)bufp[1]) << 8));
    bufp += 2;

    cmd_struct->header.src_hwid = ((uint16_t)bufp[0]) | ((uint16_t)(((uint16_t)bufp[1]) << 8));
    bufp += 2;

    cmd_struct->header.command = bufp[0];
    bufp += 1;

    memcpy(cmd_struct->data, bufp, cmd_struct->data_len);

    return COMMS_SUCCESS;
}

/**
 * @brief Converts command struct to buffer
 *
 * @param[in]  cmd_struct command to serialize
 * @param[out] buf        pointer to destination buffer, function assumes there is enough space allocated
 * @param[out] msg_len    pointer to store serialized message length (total number of bytes to be transmitted)
 *
 * @return comms_err_t indicating if conversion is successful or not
 */
comms_err_t comms_cmd_struct_to_buffer(comms_command_t* cmd_struct, uint8_t* buf, uint16_t *msg_len) {
    comms_err_t cmd_struct_err;

    cmd_struct_err = comms_check_cmd_struct(cmd_struct);
    if (cmd_struct_err != COMMS_SUCCESS) {
        return cmd_struct_err;
    }

    buf[0] = COMMS_ESP_START_BYTE_0;
    buf[1] = COMMS_ESP_START_BYTE_1;
    buf[2] = cmd_struct->data_len + COMMS_MSG_HEADER_SIZE_BYTES;
    buf[3] = (uint8_t)(cmd_struct->header.seqnum);  // LSB first
    // set MSBit to be response bit
    // seqnum and is_response is in valid range because we checked it already
    buf[4] = ((uint8_t)(cmd_struct->header.seqnum >> 8)) | ((uint8_t)(((uint8_t)(cmd_struct->header.is_response)) << 7));
    buf[5] = (uint8_t)(cmd_struct->header.dest_hwid);  // LSB first
    buf[6] = (uint8_t)(cmd_struct->header.dest_hwid >> 8);
    buf[7] = (uint8_t)(cmd_struct->header.src_hwid);  // LSB first
    buf[8] = (uint8_t)(cmd_struct->header.src_hwid >> 8);
    buf[9] = cmd_struct->header.command;
    memcpy(&buf[10], cmd_struct->data, cmd_struct->data_len);

    *msg_len = (COMMS_PKT_START_NUM_BYTES + COMMS_MSG_HEADER_SIZE_BYTES + cmd_struct->data_len);

    return COMMS_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Checks validity of contents of buffer that contains an OpenLST packet
 * TODO: check hwids, cmd_num
 *
 * @param[in] buf
 *
 * @return comms_err_t indicating if given command struct is valid or not
 */
static comms_err_t comms_check_buffer(const uint8_t* buf) {
    if (buf[0] != COMMS_ESP_START_BYTE_0) {
        return COMMS_PKT_ESP_MISMATCH_ERR;
    }
    if (buf[1] != COMMS_ESP_START_BYTE_1) {
        return COMMS_PKT_ESP_MISMATCH_ERR;
    }
    if ((buf[2] < COMMS_MIN_PKT_LEN_VAL) || (buf[2] > COMMS_MAX_PKT_LEN_VAL)) {
        return COMMS_PKT_LEN_ERR;
    }

    return COMMS_SUCCESS;
}
