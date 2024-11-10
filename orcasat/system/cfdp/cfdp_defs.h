/*
 * cfdp_defs.h
 *
 *  Created on: Jul 19, 2020
 *      Author: Richard
 */

#ifndef ORCASAT_INTERFACES_CFDP_DEFS_H_
#define ORCASAT_INTERFACES_CFDP_DEFS_H_

/**
 * @brief The maximum number of bytes that can be contained within
 * a CFDP file data PDU. This should be sized so that the PDU
 * fits nicely within a radio packet.
 */
#define CFDP_FILE_DATA_PDU_MAX_BYTES 64

/**
 * @brief PDU header (as defined in table 2.4, page 19: https://public.ccsds.org/Pubs/720x2g3ec1.pdf)
 *
 * Contains the PDU header for a transmission. Header stored as a series of uint8_ts
 * because that's what we want to send onto the queue
 *
 * Note that the struct contains only the pointer right now, but is being kept as a struct
 * in case we want it to contain some other information later
 */
typedef struct PDU_header {
    /**
     * @brief PDU header contents. Bytes contain in order:
     *
     * 0. [start, reserved for future use]
     * 1. MSB of data field length
     * 2. LSB of data field length
     * 3. [reserved for future use 2, length of <transaction sequence number - 1>]
     *    Note that transaction id length is 4 in our system (hence field should indicate 3)]
     * 4. Source ID (OBC: 10101010 = 170 = 0xAA, Ground: 00011100 = 28 = 0x1C)
     * 5. MSB of transaction id
     * 6. 2nd MSB byte of transaction id
     * 7. 3rd MSB byte of transaction id
     * 8. LSB byte of transaction id
     * 9. Destination target id (OBC: 10101010 = 170 = 0xAA, Ground: 00011100 = 28 = 0x1C)
     */
    /**
     * @brief pointer to the start of the PDU header
     */
    uint8_t* header_data;
} PDU_header_t;

/**
 * @brief CFDP file directive codes.
 */
#define RESERVED_0   0
#define RESERVED_1   1
#define RESERVED_2   2
#define RESERVED_3   3
#define EOF_PDU      4
#define FINISHED_PDU 5
#define ACK_PDU      6
#define METADATA_PDU 7
#define NAK_PDU      8
#define PROMPT_PDU   9
// Note: CFDP does not define 10 or 11.
#define KEEPALIVE_PDU 12

/**
 * @brief CFDP type codes.
 */
#define TLV_FILESTORE_REQUEST       0
#define TLV_MESSAGE_TO_USER         2
#define TLV_FAULT_HANDLER_OVERRIDES 4
#define TLV_FLOW_LABEL              5

/**
 * @brief CFDP condition codes.
 * Used in EOF PDU.
 */
typedef enum {
    CFDP_CONDITION_NO_ERROR                   = 0x0000,
    CFDP_CONDITION_POSITIVE_ACK_LIMIT_REACHED = 0x0001,
    CFDP_CONDITION_KEEPALIVE_LIMIT            = 0x0010,
    CFDP_CONDITION_INVALID_TRANSMISSION_MODE  = 0x0011,
    CFDP_CONDITION_FILESTORE_REJECTION        = 0x0100,
    CFDP_CONDITION_FILE_CHECKSUM_FAILURE      = 0x0101,
    CFDP_CONDITION_FILE_SIZE_ERROR            = 0x0110,
    CFDP_CONDITION_NAK_LIMIT_REACHED          = 0x0111,
    CFDP_CONDITION_INACTIVITY_DETECTED        = 0x1000,
    CFDP_CONDITION_INVALID_FILE_STRUCTURE     = 0x1001,
    // Note: 0x1010-0x1101 are reserved.
    CFDP_CONDITION_SUSPEND_REQUEST_RECEIVED = 0x1110,
    CFDP_CONDITION_CANCEL_REQUEST_RECEIVED  = 0x1111,
} cfdp_condition_code_t;

/**
 * @brief Most structural bytes of the CFDP file metadata PDU.
 *
 * Includes the first 7 bytes of the metadata PDU plus the destination
 * file name length.
 */
#pragma pack(push, 1) // Byte-align everything. Default alignment is word (32-bits).
typedef struct Meta_PDU {
    /**
     * @brief File directive code, defined as 0x07 for this PDU type.
     */
    uint8_t file_directive_code;           // Byte 0
    unsigned int segmentation_control : 1; // Byte 1
    unsigned int reserved_0 : 7;           // Byte 1
    uint32_t file_size_bytes;              // Byte 2-5
    uint8_t source_file_name_len;          // Byte 6
    // Not included: source file name
    uint8_t dest_file_name_len; // Byte position depends on source file name length
    // Not included: destination file name
    // Not included: TLV
} cfdp_metadata_pdu_t;
#pragma pack(pop)

/**
 * @brief The CFDP metadata TLV (type, length, value) fields, in order.
 */
#pragma pack(push, 1) // Byte-align everything. Default alignment is word (32-bits).
typedef struct cfdp_tlv {
    /**
     * @note Use the TLV_ #defines above for type.
     */
    uint8_t type;
    uint8_t length;
    uint8_t value;
} cfdp_meta_tlv_t;
#pragma pack(pop)

/**
 * @brief CFDP file data PDU. Does not contain space for the data,
 * the data should be placed after this structure in the final
 * output buffer.
 */
#pragma pack(push, 1) // Byte-align everything. Default alignment is word (32-bits).
typedef struct cfdp_file_data {
    /**
     * @brief The offset within the file that corresponds to the
     *  0th byte of data in this PDU.
     */
    uint32_t segment_offset;
} cfdp_file_data_t;
#pragma pack(pop)

/**
 * @brief CFDP end of file (EOF) PDU.
 */
#pragma pack(push, 1) // Byte-align everything. Default alignment is word (32-bits).
typedef struct cfdp_eof {
    /**
     * @brief Defined as 0x04.
     */
    uint8_t file_directive_code;
    /**
     * @brief Condition code.
     * This is a 4-bit bitfield and should be populated with one
     * of the enumerators from @ref condition_code_t.
     */
    unsigned int condition_code : 4;
    /**
     * @brief Checksum of the file data.
     */
    uint32_t checksum;
    /**
     * @brief Size of the file in bytes.
     */
    uint32_t file_size_bytes;
} cfdp_eof_pdu_t;
#pragma pack(pop)
#endif /* ORCASAT_INTERFACES_CFDP_DEFS_H_ */
