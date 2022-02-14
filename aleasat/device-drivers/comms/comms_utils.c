/**
 * @file comms_utils.h
 * @brief Comms related utility functions
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_defs.h"
#include "comms_utils.h"

// OBC
#include "obc_misra.h"

// Standard Library
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

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
    if (cmd_struct->data_len > COMMS_MAX_CMD_DATA_NUM_BYTES) {
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
 * @brief Checks validity of contents of buffer that contains an OpenLST packet
 * TODO: check hwids, cmd_num
 *
 * @param[in] buf
 *
 * @return comms_err_t indicating if given command struct is valid or not
 */
comms_err_t comms_check_buffer(uint16_t* buf) {
    uint8_t* bufp = (uint8_t*)&buf[0];

    if (bufp[0] != COMMS_ESP_START_BYTE_0) {
        return COMMS_PKT_ESP_MISMATCH_ERR;
    }
    if (bufp[1] != COMMS_ESP_START_BYTE_1) {
        return COMMS_PKT_ESP_MISMATCH_ERR;
    }
    if ((bufp[2] < COMMS_MIN_PKT_LEN_VAL) || (bufp[2] > COMMS_MAX_PKT_LEN_VAL)) {
        return COMMS_PKT_LEN_ERR;
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
comms_err_t comms_buffer_to_cmd_struct(uint16_t* buf, comms_command_t* cmd_struct) {
    uint8_t* bufp = ((uint8_t*)&buf[0]) + COMMS_ESP_NUM_BYTES;
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

    // No idea why MISRA complains about this (#1487-D (MISRA-C:2004 12.2/R))
    OBC_MISRA_CHECK_OFF
    memcpy(cmd_struct->data, bufp, cmd_struct->data_len);
    OBC_MISRA_CHECK_ON

    return COMMS_SUCCESS;
}

/**
 * @brief Converts command struct to buffer
 *
 * @param[in] cmd_struct
 * @param[out] buf pointer to destination buffer, function assumes there is enough space allocated
 *
 * @return comms_err_t indicating if conversion is successful or not
 */
comms_err_t comms_cmd_struct_to_buffer(comms_command_t* cmd_struct, uint16_t* buf) {
    uint8_t* bufp = (uint8_t*)&buf[0];
    comms_err_t cmd_struct_err;

    cmd_struct_err = comms_check_cmd_struct(cmd_struct);
    if (cmd_struct_err != COMMS_SUCCESS) {
        return cmd_struct_err;
    }

    bufp[0] = COMMS_ESP_START_BYTE_0;
    bufp[1] = COMMS_ESP_START_BYTE_1;
    bufp[2] = cmd_struct->data_len + COMMS_MSG_HEADER_SIZE_BYTES;
    bufp[3] = (uint8_t)(cmd_struct->header.seqnum);  // LSB first
    // set MSBit to be response bit
    // seqnum and is_response is in valid range because we checked it already
    bufp[4] = ((uint8_t)(cmd_struct->header.seqnum >> 8)) | ((uint8_t)(((uint8_t)(cmd_struct->header.is_response)) << 7));
    bufp[5] = (uint8_t)(cmd_struct->header.dest_hwid);  // LSB first
    bufp[6] = (uint8_t)(cmd_struct->header.dest_hwid >> 8);
    bufp[7] = (uint8_t)(cmd_struct->header.src_hwid);  // LSB first
    bufp[8] = (uint8_t)(cmd_struct->header.src_hwid >> 8);
    bufp[9] = cmd_struct->header.command;
    bufp += 10;
    memcpy(bufp, cmd_struct->data, cmd_struct->data_len);

    return COMMS_SUCCESS;
}

/**
 * @brief Fills a buffer based on arguments, skips having to create intermediate struct
 *
 * @param[in] is_response
 * @param[in] dest_hwid
 * @param[in] seqnum
 * @param[in] cmd_num
 * @param[in] cmd_data data to send as part of command, cannot be NULL if cmd_data_len is not zero
 * @param[in] cmd_data_len length of cmd_data
 * @param[out] buf pointer to destination buffer, function assumes there is enough space allocated
 *
 * @return comms_err_t indicating if conversion is successful or not
 */
comms_err_t comms_build_buffer(
    uint8_t is_response,
    hwid_t dest_hwid,
    uint16_t seqnum,
    uint8_t cmd_num,
    uint8_t* cmd_data,
    uint8_t cmd_data_len,
    uint16_t* buf
) {
    uint8_t* bufp = (uint8_t*)&buf[0];
    comms_err_t params_err;

    if (seqnum > 0x7FFF) {
        return COMMS_BAD_SEQNUM_ERR;
    }

    if (is_response > 1) {
        return COMMS_BAD_RESPONSE_BIT_FIELD_ERR;
    }

    bufp[0] = COMMS_ESP_START_BYTE_0;
    bufp[1] = COMMS_ESP_START_BYTE_1;
    bufp[2] = cmd_data_len + COMMS_MSG_HEADER_SIZE_BYTES;
    bufp[3] = (uint8_t)(seqnum);  // LSB first
    // set MSBit to be response bit
    // seqnum and is_response is in valid range because we checked it already
    bufp[4] = ((uint8_t)(seqnum >> 8)) | ((uint8_t)(((uint8_t)(is_response)) << 7));
    bufp[5] = (uint8_t)(dest_hwid);  // LSB first
    bufp[6] = (uint8_t)(dest_hwid >> 8);
    bufp[7] = (uint8_t)(obc_hwid);  // LSB first
    bufp[8] = (uint8_t)(obc_hwid >> 8);
    bufp[9] = cmd_num;
    bufp += 10;
    memcpy(bufp, cmd_data, cmd_data_len);

    params_err = comms_check_buffer(buf);
    if (params_err != COMMS_SUCCESS) {
        return params_err;
    }

    return COMMS_SUCCESS;
}

void inc_seqnum(uint16_t* seqnum) {
    *seqnum = ((*seqnum) + 1) & 0x7FFF;
}
