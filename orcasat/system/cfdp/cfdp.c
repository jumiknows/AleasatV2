/*
 * cfdp.c
 *
 *  Created on: May 11, 2020
 *      Author: Julie Belleville
 */

#include <string.h>
#include "cfdp.h"
#include "obc_error.h"
#include "obc_uart.h"

/**
 * @brief Modes for the CFDP PDU. This adjusts the PDU format in a
 * non-standard way.
 *
 * This is temporary to allow simultaneous support for Orca simple downlink
 * and pieces of CFDP.
 */
typedef enum { CFDP = 0, ORCA = 1 } cfdp_or_orca_t;

/**
 * @brief Configures how the reserved for future use (RFU) 0 field of the PDU headers is set.
 * This can be used to bypass CFDP processing on the Houston side while we get it all working.
 *
 * In CFDP mode, the PDU contains some sort of data to be processed by CFDP handlers in Houston. RFU 0 field is set to 0 (CFDP default).
 * In ORCA mode, the PDU contains data that will be processed with existing ORCA log message handlers in Houston,
 * such as the simple downlink handler. RFU 0 field is set to 1.
 *
 * This is intended to be temporary, to support simple downlinking while we are working on the guts of CFDP.
 */
cfdp_or_orca_t mode = ORCA;

/**
 * @brief The size of local buffers for CFDP transmission. This should be sized so that:
 *  1. The PDU header fits.
 *  2. The maximum number of bytes of a PDU type (file data, EOF, etc.) also fits.
 */
#define LOCAL_BUFFER_SIZE 100

/**
 * @brief Sets the CFDP PDU headers to be in ORCA mode.
 *
 * Field RFU 0 is 1.
 */
void set_orca_mode(void) {
    mode = ORCA;
}

/**
 * @brief Sets the CFDP PDU headers to be in CFDP mode.
 *
 * Field RFU 0 is 0.
 */
void set_cfdp_mode(void) {
    mode = CFDP;
}

/**
 * @Brief generates CFDP PDU header. Assumes that the direction is towards file receiver (since we are single link).
 *
 * Full definition here: https://public.ccsds.org/Pubs/720x2g3ec1.pdf (table 2-4, p. 19)
 *
 * @param[out] header: struct to populate to return header data
 * @param is_file_data: true if the PDU will be file data, false otherwise
 * @param is_unacknowledged: true if PDU is to be sent in unacknowledged mode, false otherwise
 * @param has_CRC: true if PDU is to be sent with CRC, false otherwise
 * @param field_length: length of data inside PDU (stored over 2 uint8_ts, so full uint16_t range of values may be used)
 * @param transaction_seq_num: transaction sequence number for this PDU
 *
 * @return CFDP_ok (no error possible for now, but leaving room for change there)
 */
cfdp_err_t cfdp_get_PDU_header(PDU_header_t* header, bool is_file_data, bool is_unacknowledged, bool has_CRC, uint16_t field_length, uint32_t transaction_seq_num) {
    // header start
    header->header_data[0] = 0b00000000;

    // Temporary: encode CFDP/ORCA mode into RFU bit 0.
    if (mode == ORCA) {
        header->header_data[0] |= 0b10000000;
    }
    if (is_file_data) {
        header->header_data[0] |= 0b00001000;
    }
    if (is_unacknowledged) {
        header->header_data[0] |= 0b00100000;
    }
    if (has_CRC) {
        header->header_data[0] |= 0b01000000;
    }

    // separate field length into 2 uint8_ts
    header->header_data[1] = (field_length >> 8) & 0x00FF;
    header->header_data[2] = field_length & 0x00FF;

    // length of entity IDs are 1 (hence the bits corresponding to entity ID length are length - 1 = 1 - 1 = 0)
    // transaction sequence number length set to 4 (hence the bits hold 4 - 1 = 3)
    // [0] 		RFU 1 = 0.
    // [3:1] 	Length of entity IDs minus one. (0)
    // [4] 		RFU 2 = 0.
    // [7:5]		Length of sequence number minus one. (3 = 0b11)
    header->header_data[3] = 0b01100000;

    // source entity ID: set to 10101010 (170, 0xAA) for OBC
    header->header_data[4] = 0b10101010;

    // transaction id (split into 4 uint8_ts)
    header->header_data[5] = (transaction_seq_num >> 24) & 0x00FF;
    header->header_data[6] = (transaction_seq_num >> 16) & 0x00FF;
    header->header_data[7] = (transaction_seq_num >> 8) & 0x00FF;
    header->header_data[8] = transaction_seq_num & 0x00FF;

    // destination entity ID: set to 00011100 (28, 0x1C) for Ground Station
    header->header_data[9] = 0b00011100;

    return CFDP_OK;
}

/**
 * @Brief Provides the next unused transaction id (if all IDs are used, wraps around back to 1)
 * 		  transaction id 0 is reserved for logger messages
 * @return the next transaction id
 */
uint32_t cfdp_get_next_transaction_seq_num(void) {
    static uint32_t next_transaction_seq_num = 0;

    next_transaction_seq_num++;
    if (next_transaction_seq_num == 0) {
        next_transaction_seq_num = 1; // better it repeat than be 0, since 0 is reserved for logger messages
    }
    return next_transaction_seq_num;
}

/**
 * @brief Creates and sends a CFDP metadata PDU.
 * @param[in] file_name The name of the file. Must be null-terminated.
 * @param[in] file_size_bytes The size of the file in bytes.
 * @return The length of the PDU in bytes. Zero if the inputs are invalid.
 */
uint16_t send_cfdp_metadata_pdu(const char* file_name, uint32_t file_size_bytes) {
    uint8_t buffer[LOCAL_BUFFER_SIZE] = {0};
    uint32_t file_name_len            = strlen(file_name);

    // Check the PDU size before continuing.
    if (LOCAL_BUFFER_SIZE <= (CFDP_PDU_HEADER_SIZE + 7 /* base length of metadata PDU */
                              + 1                      /* Destination file name length byte.*/
                              + 3                      /* TLV field */
                              + (2 * file_name_len))) {
        return 0;
    }
    cfdp_metadata_pdu_t meta = {
        .file_directive_code  = METADATA_PDU,
        .segmentation_control = 0x00, // 0 = file boundaries respected
        .reserved_0           = 0x00,
        .file_size_bytes      = file_size_bytes,
        .source_file_name_len = file_name_len,
        .dest_file_name_len   = file_name_len,
    };

    // Populate the metadata PDU buffer
    memcpy(&buffer[CFDP_PDU_HEADER_SIZE], (void*)&meta, 7); // Bytes 0-6
    uint16_t pdu_length = 7;
    memcpy(&buffer[CFDP_PDU_HEADER_SIZE + pdu_length], file_name, meta.source_file_name_len); // File name (source)
    pdu_length += meta.source_file_name_len;
    memcpy(&buffer[CFDP_PDU_HEADER_SIZE + pdu_length], (void*)&meta.dest_file_name_len, 1); // Destination file name length
    pdu_length += 1;
    memcpy(&buffer[CFDP_PDU_HEADER_SIZE + pdu_length], file_name, meta.dest_file_name_len); // Destination file name
    pdu_length += meta.dest_file_name_len;

    // Populate the TLV fields
    cfdp_meta_tlv_t tlv = {.type = TLV_FILESTORE_REQUEST, .length = 0x00, .value = 0x00};
    memcpy(&buffer[CFDP_PDU_HEADER_SIZE + pdu_length], (void*)&tlv, 3);
    pdu_length += 3;

    // Create the CFDP fixed header and place it in the buffer before the CFDP metadata PDU.
    // Set to CFDP mode so that Houston will parse this as a CFDP PDU and not a logger message.
    set_cfdp_mode();
    PDU_header_t header = {.header_data = buffer};
    cfdp_get_PDU_header(&header, false /* is_file_data */, true /* is_unacknowledged */, false /* has_CRC */, pdu_length /* field_length */, cfdp_get_next_transaction_seq_num());
    set_orca_mode();

    // Transmit the header and then the metadata PDU
    serial_send(buffer, CFDP_PDU_HEADER_SIZE + pdu_length);
    return pdu_length;
}

/**
 * @brief Sends file data.
 * Up to @ref CFDP_FILE_DATA_PDU_MAX_BYTES can be sent at a time.
 *
 * @param[in] data The file data to transmit.
 * @param[in] length_bytes The number of bytes to transmit.
 * @param[in] offset The offset of @c data relative to the 0th byte of the file.
 * @return CFDP_OK or CFDP_ERR_TOO_LARGE if the length is too large.
 */
cfdp_err_t send_cfdp_file_data(const uint8_t* data, uint32_t length_bytes, uint32_t offset) {
    uint8_t buffer[LOCAL_BUFFER_SIZE]        = {0};
    const uint32_t file_data_pdu_base_length = sizeof(cfdp_file_data_t);
    uint32_t pdu_length                      = file_data_pdu_base_length + length_bytes;

    // Sanity check the length.
    if ((length_bytes > CFDP_FILE_DATA_PDU_MAX_BYTES) || (LOCAL_BUFFER_SIZE < (CFDP_PDU_HEADER_SIZE + file_data_pdu_base_length + length_bytes))) {
        return CFDP_ERR_TOO_LARGE;
    }

    // Copy the file data PDU into the output buffer.
    cfdp_file_data_t file_data_pdu = {.segment_offset = offset};
    memcpy(&buffer[CFDP_PDU_HEADER_SIZE], &file_data_pdu, file_data_pdu_base_length);      // File data PDU.
    memcpy(&buffer[CFDP_PDU_HEADER_SIZE + file_data_pdu_base_length], data, length_bytes); // The file data.

    // Copy the header ahead of the file data PDU.
    set_cfdp_mode();
    PDU_header_t header = {.header_data = buffer};
    cfdp_get_PDU_header(&header, true /* is_file_data */, true /* is_unacknowledged */, false /* has_CRC */, pdu_length /* field_length */, cfdp_get_next_transaction_seq_num());
    set_orca_mode();

    // Transmit the header and then the file data PDU.
    serial_send(buffer, CFDP_PDU_HEADER_SIZE + pdu_length);
    return CFDP_OK;
}

/**
 * @brief Sends the end of file (EOF) PDU.
 * @param[in] file_size_bytes The size of the file in bytes.
 * @param[in] checksum The checksum calculated over the file data.
 * @param[in] condition is the CFDP condition code.
 */
void send_cfdp_eof_pdu(uint32_t file_size_bytes, uint32_t checksum, cfdp_condition_code_t condition) {
    uint8_t buffer[LOCAL_BUFFER_SIZE] = {0};
    const uint32_t pdu_length         = sizeof(cfdp_eof_pdu_t);

    cfdp_eof_pdu_t eof_pdu = {
        .file_directive_code = EOF_PDU,
        .file_size_bytes     = file_size_bytes,
        .checksum            = checksum,
        .condition_code      = condition,
    };

    // Copy the EOF PDU ahead of the fixed PDU header.
    memcpy(&buffer[CFDP_PDU_HEADER_SIZE], &eof_pdu, pdu_length);

    // Copy the header ahead of the EOF PDU.
    PDU_header_t header = {.header_data = buffer};
    set_cfdp_mode();
    cfdp_get_PDU_header(&header, false /* is_file_data */, true /* is_unacknowledged */, false /* has_CRC */, pdu_length /* field_length */, cfdp_get_next_transaction_seq_num());
    set_orca_mode();

    // Transmit the header and then the EOF PDU.
    serial_send(buffer, CFDP_PDU_HEADER_SIZE + pdu_length);
}
