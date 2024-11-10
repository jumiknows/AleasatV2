/*
 * cfdp.h
 *
 *  Created on: May 10, 2020
 *      Author: Julie Belleville
 */

#ifndef ORCASAT_INTERFACES_CFDP_H_
#define ORCASAT_INTERFACES_CFDP_H_

#include "obc_error.h"
#include "cfdp_defs.h"

#define CFDP_PDU_HEADER_SIZE 10

void set_orca_mode(void);
void set_cfdp_mode(void);
uint32_t cfdp_get_next_transaction_seq_num(void);

// CFDP PDU creation
cfdp_err_t cfdp_get_PDU_header(PDU_header_t* header, bool is_file_data, bool is_unacknowledged, bool has_CRC, uint16_t field_length, uint32_t transaction_seq_num);
uint16_t send_cfdp_metadata_pdu(const char* file_name, uint32_t file_size_bytes);
cfdp_err_t send_cfdp_file_data(const uint8_t* data, uint32_t length_bytes, uint32_t offset);
void send_cfdp_eof_pdu(uint32_t file_size_bytes, uint32_t checksum, cfdp_condition_code_t condition);
#endif /* ORCASAT_INTERFACES_CFDP_H_ */
