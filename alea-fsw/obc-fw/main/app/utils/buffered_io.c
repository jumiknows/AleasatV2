/**
 * @file buffered_io.c
 * @brief Buffered I/O
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "buffered_io.h"

// Utils
#include "io_stream.h"
#include "obc_utils.h"

// Standard Library
#include <stdint.h>
#include <string.h>

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Reset the internal state of a buffered input stream
 *
 * @param[in] buf_in Pointer to the buffered input stream
 */
void buffered_io_input_reset(buffered_input_t *buf_in) {
    buf_in->start = 0;
    buf_in->valid_len = 0;
}

/**
 * @brief Reset the internal state of a buffered output stream
 *
 * @param[in] buf_out Pointer to the buffered output stream
 */
void buffered_io_output_reset(buffered_output_t *buf_out) {
    buf_out->offset = 0;
}

/**
 * @brief Write data to a buffered output. The data will be accumulated in the
 * internal buffer and only actually be written to the output stream in chunks
 * of buf_out->size bytes.
 *
 * This function is compatible with the io_ostream_t->write API.
 *
 * @param[in]  handle       Pointer to a buffered_output_t data structure
 * @param[in]  data         Data to write
 * @param[in]  num_bytes    Number of bytes to write
 * @param[in]  timeout      Overall timeout for the write operation. Same units as
 *                          expected by buf_out->output->write(...)
 * @param[out] timeout_left Pointer to store remaining timeout after the write operation.
 *                          Can be NULL if the caller doesn't care about the remaining timeout.
 *
 * @return Number of bytes actually written (internally, not necessarily to the output stream).
 */
uint32_t buffered_io_write(void *handle, const uint8_t *data, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left) {
    buffered_output_t *buf_out = (buffered_output_t *)handle;

    // Check arguments
    if ((buf_out == NULL) || (data == NULL)) {
        return 0;
    }

    uint32_t total_written = 0;
    uint32_t write_size = 0;

    // Loop until all bytes are written or we encounter a timeout (see break statements)
    while (num_bytes > 0) {
        if ((buf_out->offset == 0) && (num_bytes > buf_out->size)) {
            // If there's no data buffered up, and we're writing at least a full buffer, we can write directly to output stream
            // (skips the intermediate memcpys to improve performance)
            write_size = io_stream_write(buf_out->output, data, buf_out->size, timeout, &timeout);

            // Handle error
            if (write_size != buf_out->size) {
                total_written += write_size;
                break;
            }
        } else {
            // This is always positive because anytime offset reaches size, we flush the buffer to the output stream
            // and reset offset
            uint32_t buf_space_left = (buf_out->size - buf_out->offset);

            // This is always positive because of the while loop condition
            write_size = MIN(buf_space_left, num_bytes);

            // Do the actual copy
            memcpy(&(buf_out->buf[buf_out->offset]), data, write_size);

            buf_out->offset += write_size;

            // Flush the buffer to the output stream when the buffer is full
            if (buf_out->offset == buf_out->size) {
                bool flushed = buffered_io_flush(buf_out, timeout, &timeout);

                // Handle error
                if (!flushed) {
                    // TODO ALEA-840 deal with data was that partially flushed
                    break;
                }
            }
        }

        // Update loop variables
        num_bytes     -= write_size;
        data          += write_size;
        total_written += write_size;
    }

    // Assumes that the processing time in this function is negligible compared to the processing time in the underylying stream.
    if (timeout_left != NULL) {
        *timeout_left = timeout;
    }

    return total_written;
}

/**
 * @brief Flush a buffered output stream. Any data currently (internally) buffered will
 * be written to the output stream.
 *
 * This function is compatible with the io_ostream_t->flush API.
 *
 * @param[in] handle  Pointer to a buffered_output_t data structure
 * @param[in] timeout Overall timeout for the write operation. Same units as
 *                    expected by buf_out->output->write(...)
 *
 * @return Number of bytes written to the output stream during the flush
 */
bool buffered_io_flush(void *handle, uint32_t timeout, uint32_t *timeout_left) {
    buffered_output_t *buf_out = (buffered_output_t *)handle;

    // Check arguments
    if ((buf_out == NULL)) {
        return false;
    }

    bool success = false;

    // Write all the buffered bytes
    uint32_t written = io_stream_write(buf_out->output, buf_out->buf, buf_out->offset, timeout, &timeout);

    if (written == buf_out->offset) {
        // Reset the offset
        buf_out->offset = 0;

        // Flush the output stream (in case it's also buffered)
        success = io_stream_flush(buf_out->output, timeout, timeout_left);
    }

    if (timeout_left != NULL) {
        *timeout_left = timeout;
    }

    return success;
}

/**
 * @brief Read from a buffered input stream. More than num_bytes may be read from the underlying
 * stream (handle->input). Excess bytes will be stored in the internal buffer until requested by
 * a subsequent call to buffered_io_read.
 *
 * This function is compatible with the io_istream_t->read API.
 *
 * @param[in]  handle       Pointer to a buffered_input_t data structure
 * @param[out] buf          Pointer to where the read data will be placed. Must have size >= num_bytes
 * @param[in]  num_bytes    Number of bytes to read
 * @param[in]  timeout      Overall timeout for the read operation. Same units as
 *                          expected by buf_in->input->read_block(...)
 * @param[out] timeout_left Pointer to store remaining timeout after the read operation.
 *                          Can be NULL if the caller doesn't care about the remaining timeout.
 *
 * @return Number of bytes read
 */
uint32_t buffered_io_read(void *handle, uint8_t *buf, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left) {
    buffered_input_t *buf_in = (buffered_input_t *)handle;

    // Check arguments
    if ((buf_in == NULL) || (buf == NULL)) {
        return 0;
    }

    // If max_block_size is bigger than the buffer size, we risk the chance of overflowing on a read
    if (buf_in->input->max_block_size > buf_in->size) {
        return 0;
    }

    uint32_t total_read = 0;

    // Iterate until we've read all the bytes
    // OR a timeout occurs (see break statement in loop)
    while (total_read < num_bytes) {
        // If there is buffered data, use that first
        if (buf_in->valid_len > 0) {
            // Copy the smaller of: 1) the data available OR 2) the number of bytes we still need
            uint32_t bytes_to_copy = MIN(buf_in->valid_len, (num_bytes - total_read));
            memcpy(&(buf[total_read]), &(buf_in->buf[buf_in->start]), bytes_to_copy);

            total_read += bytes_to_copy;

            // We've consumed buffer data so update the index pointers
            buf_in->start += bytes_to_copy;
            buf_in->valid_len -= bytes_to_copy; // Subtraction is safe: bytes_to_copy cannot be greater than buf_in->valid_len due to the MIN above

            // Reset start if the buffer was fully consumed
            if (buf_in->valid_len == 0) {
                buf_in->start = 0;
            }
        } else {
            // If we're here, the buffer is fully consumed (buf_in->start == 0) and we need to read new data

            // Choose a read target:
            //   - If we still have to read more than the max_block_size then we can read directly to the user buf
            //   - If we won't use a full max_block_size we have to read to the internal buffer so excess data is saved
            uint8_t *read_target = buf_in->buf;
            bool direct_read = false;

            if ((num_bytes - total_read) > buf_in->input->max_block_size) {
                read_target = &(buf[total_read]);
                direct_read = true;
            }

            // Perform the read
            uint32_t new_bytes_read = buf_in->input->read_block(buf_in->input->handle, read_target, buf_in->size, timeout, &timeout);

            // Check for timeout
            if (new_bytes_read == 0) {
                break;
            }

            // Only update the valid_len if we read to the internal buffer
            if (!direct_read) {
                buf_in->valid_len = new_bytes_read;
            } else {
                // Otherwise update total_read directly
                total_read += new_bytes_read;
            }
        }
    }

    // Assumes that the processing time in this function is negligible compared to the processing time in the underlying stream.
    if (timeout_left != NULL) {
        *timeout_left = timeout;
    }

    return total_read;
}
