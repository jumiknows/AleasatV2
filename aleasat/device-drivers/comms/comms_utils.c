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

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Checks validity of command struct
 * TODO: check hwid, system, cmd_num
 *
 * @param[in] cmd_struct
 *
 * @return comms_err_t indicating if given command struct is valid or not
 */
comms_err_t comms_check_cmd_struct(comms_command_t* cmd_struct) {
    if (cmd_struct->data_len > COMMS_MAX_CMD_DATA_SIZE_BYTES) {
        return COMMS_PKT_LEN_ERR;
    }

    return COMMS_SUCCESS;
}

/**
 * @brief Checks validity of contents of buffer that contains an OpenLST message
 * TODO: check hwid, system, cmd_num
 *
 * @param[in] buf
 *
 * @return comms_err_t indicating if given command struct is valid or not
 */
comms_err_t comms_check_buffer(uint16_t* buf) {
    uint8_t* bufp = (uint8_t*)&buf[0];

    if (bufp[0] != COMMS_ESP_START_BYTE_0) {
        return COMMS_MSG_ESP_MISMATCH_ERR;
    }
    if (bufp[1] != COMMS_ESP_START_BYTE_1) {
        return COMMS_MSG_ESP_MISMATCH_ERR;
    }
    if ((bufp[2] < COMMS_MIN_PKT_LEN_VAL) || (bufp[2] > COMMS_MAX_PKT_LEN_VAL)) {
        return COMMS_PKT_LEN_ERR;
    }

    return COMMS_SUCCESS;
}

/**
 * @brief Checks validity of given parameters as an OpenLST message
 * TODO: check hwid, system, cmd_num
 *
 * @param[in] hwid
 * @param[in] system
 * @param[in] seqnum
 * @param[in] cmd_num
 * @param[in] cmd_data
 * @param[in] cmd_data_len
 *
 * @return comms_err_t indicating if given command struct is valid or not
 */
comms_err_t comms_check_cmd_params(
    comms_hwid_t hwid,
    uint8_t system,
    uint16_t seqnum,
    uint8_t cmd_num,
    uint8_t* cmd_data,
    uint8_t cmd_data_len
) {
    if (cmd_data_len > COMMS_MAX_CMD_DATA_SIZE_BYTES) {
        return COMMS_PKT_LEN_ERR;
    }
    if ((cmd_data_len > 0) && (cmd_data == NULL)) {
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
    uint8_t* bufp = (uint8_t*)&buf[0];
    comms_err_t buf_err;

    buf_err = comms_check_buffer(buf);
    if (buf_err != COMMS_SUCCESS) {
        return buf_err;
    }

    // subtract off bytes in (hwid + seqnum + system + command) and we are left with bytes in data buffer
    cmd_struct->data_len = bufp[2] - COMMS_PKT_HEADER_SIZE_BYTES;

    cmd_struct->header.hwid = (uint16_t)bufp[3] + (uint16_t)(((uint16_t)bufp[4]) << 8);
    cmd_struct->header.seqnum = (uint16_t)bufp[5] + (uint16_t)(((uint16_t)bufp[6]) << 8);
    cmd_struct->header.system = bufp[7];
    cmd_struct->header.command = bufp[8];
    bufp += 9;
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
    // see comms_buffer_to_cmd_struct() for breakdown
    bufp[2] = cmd_struct->data_len + COMMS_PKT_HEADER_SIZE_BYTES;

    bufp[3] = (uint8_t)cmd_struct->header.hwid;  // LSB first
    bufp[4] = (uint8_t)(cmd_struct->header.hwid >> 8);
    bufp[5] = (uint8_t)cmd_struct->header.seqnum;  // LSB first
    bufp[6] = (uint8_t)(cmd_struct->header.seqnum >> 8);
    bufp[7] = cmd_struct->header.system;
    bufp[8] = cmd_struct->header.command;
    bufp += 9;
    memcpy(bufp, cmd_struct->data, cmd_struct->data_len);

    return COMMS_SUCCESS;
}

/**
 * @brief Fills a comms_command_t based on arguments
 *
 * @param[in] hwid
 * @param[in] system
 * @param[in] seqnum
 * @param[in] cmd_num
 * @param[in] cmd_data data to send as part of command, cannot be NULL if cmd_data_len is not zero
 * @param[in] cmd_data_len length of cmd_data
 * @param[out] cmd_struct
 *
 * @return comms_err_t indicating if conversion is successful or not
 */
comms_err_t comms_build_cmd_struct(
    comms_hwid_t hwid,
    uint8_t system,
    uint16_t seqnum,
    uint8_t cmd_num,
    uint8_t* cmd_data,
    uint8_t cmd_data_len,
    comms_command_t* cmd_struct
) {
    comms_err_t params_err;

    params_err = comms_check_cmd_params(hwid, system, seqnum, cmd_num, cmd_data, cmd_data_len);
    if (params_err != COMMS_SUCCESS) {
        return params_err;
    }

    cmd_struct->header.hwid = hwid;
    cmd_struct->header.seqnum = seqnum;
    cmd_struct->header.system = system;
    cmd_struct->header.command = cmd_num;
    cmd_struct->data_len = cmd_data_len;
    memcpy(cmd_struct->data, cmd_data, cmd_data_len);

    return COMMS_SUCCESS;
}

/**
 * @brief Fills a buffer based on arguments
 *
 * @param[in] hwid
 * @param[in] system
 * @param[in] seqnum
 * @param[in] cmd_num
 * @param[in] cmd_data data to send as part of command, cannot be NULL if cmd_data_len is not zero
 * @param[in] cmd_data_len length of cmd_data
 * @param[out] buf pointer to destination buffer, function assumes there is enough space allocated
 *
 * @return comms_err_t indicating if conversion is successful or not
 */
comms_err_t comms_build_buffer(
    comms_hwid_t hwid,
    uint8_t system,
    uint16_t seqnum,
    uint8_t cmd_num,
    uint8_t* cmd_data,
    uint8_t cmd_data_len,
    uint16_t* buf
) {
    uint8_t* bufp = (uint8_t*)&buf[0];
    comms_err_t params_err;

    params_err = comms_check_cmd_params(hwid, system, seqnum, cmd_num, cmd_data, cmd_data_len);
    if (params_err != COMMS_SUCCESS) {
        return params_err;
    }

    bufp[0] = COMMS_ESP_START_BYTE_0;
    bufp[1] = COMMS_ESP_START_BYTE_1;
    // see comms_buffer_to_cmd_struct() for breakdown
    bufp[2] = cmd_data_len + COMMS_PKT_HEADER_SIZE_BYTES;

    bufp[3] = (uint8_t)hwid;  // LSB first
    bufp[4] = (uint8_t)(hwid >> 8);
    bufp[5] = (uint8_t)seqnum;  // LSB first
    bufp[6] = (uint8_t)(seqnum >> 8);
    bufp[7] = system;
    bufp[8] = cmd_num;
    bufp += 9;
    memcpy(bufp, cmd_data, cmd_data_len);

    return COMMS_SUCCESS;
}
