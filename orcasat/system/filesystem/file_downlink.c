/**
 * @file file_downlink.c
 * @brief File downlink protocol implementation.
 * This implementation is based on the CFDP protocol which
 * - starts by sending a metadata PDU that includes the file name and size
 * - It then reads the file in chunks and sends it over the serial
 * - The transaction is completed by sending a EOF PDU that inlcudes
 *   - number of bytes transmitted
 *   - checksum over the transmitted bytes
 *   - and an error condition
 *
 * @author Andrada Zoltan, Alireza Alidousti
 * @date April 2, 2020
 */

#include "file_downlink.h"
#include "cfdp.h"
#include "cfdp_defs.h"
#include "filesystem.h"
#include "logger.h"
#include "obc_uart.h"
#include "obc_utils.h"
#include "sys_common.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"

static uint32_t next_checksum(uint32_t current_checksum, uint8_t* buffer, uint32_t length);

/**
 * @brief Performs a simple file downlink
 *
 * @param path: file to downlink
 * returns: Negative error code if unsuccessful
 */
fs_err_t simple_file_downlink(const char* path) {
    if (path == NULL) {
        return FS_PARAMISNULL_ERR;
    }
    log_str(INFO, FS_LOG, false, "%s Downlink", path);

    // Initialize Variables
    uint32_t size = 0, // The size of the file in bytes
        length    = 0, // Number of bytes available in the buffer
        read      = 0, // Offset relative to the 0th byte of the file
        checksum  = 0; // Current checksum calculated over the bytes read so far

    uint8_t buffer[CFDP_FILE_DATA_PDU_MAX_BYTES]; // Read the file in CFDP_FILE_DATA_PDU_MAX_BYTES byte chunks

    // Calculate the file size and send a metadata PDU
    fs_err_t err = fs_file_size(path, &size);
    if (err) {
        return err;
    }
    send_cfdp_metadata_pdu(path, size);

    // Read the file in CFDP_FILE_DATA_PDU_MAX_BYTES byte chunks and send file-data PDUs
    // If there is an error send an EOF PDU
    for (read = 0; read < size; read += (uint32_t)CFDP_FILE_DATA_PDU_MAX_BYTES) {
        length = MIN(CFDP_FILE_DATA_PDU_MAX_BYTES, size - read);

        err = fs_read(path, read, buffer, length);
        if (err) {
            // Terminate the operation with a failure condition
            send_cfdp_eof_pdu(read, checksum, CFDP_CONDITION_FILESTORE_REJECTION);
            return err;
        }

        checksum = next_checksum(checksum, buffer, length);
        send_cfdp_file_data(buffer, length, read);
    }

    // Complete the operation by sending an EOF PDU
    send_cfdp_eof_pdu(size, checksum, CFDP_CONDITION_NO_ERROR);
    log_str(INFO, FS_LOG, false, "Downlink Completed");
    return FS_OK;
}

/**
 * @brief Returns the CFDP checksum over data processed so far and the bytes passed in
 *
 * You can read about the checksum function in CFDP at the "CCSDS FILE DELIVERY PROTOCOL (CFDP)— PART 2" in sections 2.5.1 and 7.4
 *
 * @param current_checksum: The current CFDP checksum for data process up to now
 * @param buffer: A buffer containing the next sequence of bytes
 * @param length: Number of bytes available in the buffer
 *
 * @returns: CFDP checksum calculated over data processed so far and the bytes passed in
 */
static uint32_t next_checksum(uint32_t current_checksum, uint8_t* buffer, uint32_t length) {
    uint32_t read = 0, acc = 0;
    while (read < length) {
        uint8_t octed_buffer[4]   = {'\0'};
        uint32_t num_bytes_to_cpy = MIN(4, length - read);
        memcpy(octed_buffer, buffer + read, num_bytes_to_cpy);

        read += num_bytes_to_cpy;
        uint32_t octed = *(uint32_t*)octed_buffer;
        acc            = (acc + octed) % 0xffffffff;
    }

    return (current_checksum + acc) % 0xffffffff;
}
