/**
 * @file buffered_io.h
 * @brief Buffered I/O
 */

#ifndef BUFFERED_IO_H_
#define BUFFERED_IO_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Utils
#include "io_stream.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Representation of a buffered output stream
 */
typedef struct {
    /**
     * @brief Size of buf
     */
    const uint32_t size;
    /**
     * @brief Buffer to store data before writing it to the output stream
     */
    uint8_t * const buf;

    /**
     * @brief Output stream to which data will be written
     */
    const io_ostream_t * const output;

    /**
     * @brief Current amount of data in the buffer
     */
    uint32_t offset;
} buffered_output_t;

/**
 * @brief Representation of a block-buffered input stream
 */
typedef struct {
    /**
     * @brief Identifier for the stream
     * 
     * Can be passed to write and flush functions if the same function is re-used
     * for multiple streams.
     */
    void *handle;

    /**
     * @brief Maximum block size for the stream
     * 
     * i.e. Largest amount of data that could be returned by read_block.
     */
    uint32_t max_block_size;

    /**
     * @brief Read a block of data from the stream
     * 
     * @param[in]  handle       Handle to identify the stream
     * @param[out] buf          Pointer to buffer where received data will be stored
     * @param[in]  buf_len      Size of the buffer (if the buffer is too small, no data will be returned)
     * @param[in]  timeout      Timeout (units determined by implementation)
     * @param[out] timeout_left Pointer to store remaining timeout after the read operation.
     *                          Can be NULL if the caller doesn't care about the remaining timeout.
     * 
     * @return Number of bytes read (size of the block)
     */
    uint32_t (*read_block)(void *handle, uint8_t *buf, uint32_t buf_len, uint32_t timeout, uint32_t *timeout_left);
} buffered_block_istream_t;

typedef struct {
    /**
     * @brief Size of buf
     */
    const uint32_t size;
    /**
     * @brief Buffer to store data as it's read from the input stream
     */
    uint8_t * const buf;

    /**
     * @brief Input stream from which blocks will be read
     */
    const buffered_block_istream_t * const input;

    /**
     * @brief Starting point in the buffer for valid data
     */
    uint32_t start;
    /**
     * @brief Current amount of valid data in the buffer (starting at start)
     */
    uint32_t valid_len;
} buffered_input_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// These function signatures are compatible with io_ostream_t and io_istream_t (see io_stream.h)
// so they can be used directly in stream declarations

void buffered_io_input_reset(buffered_input_t *buf_in);
void buffered_io_output_reset(buffered_output_t *buf_out);

uint32_t buffered_io_write(void *handle, const uint8_t *data, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left);
bool buffered_io_flush(void *handle, uint32_t timeout, uint32_t *timeout_left);

uint32_t buffered_io_read(void *handle, uint8_t *buf, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left);

/******************************************************************************/
/*                       I N L I N E  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Wrapper for buffered_block_istream_t->read_block() that automatically passes the handle
 */
static inline uint32_t buffered_block_istream_read_block(const buffered_block_istream_t *stream, uint8_t *buf, uint32_t buf_len, uint32_t timeout, uint32_t *timeout_left) {
    return stream->read_block(stream->handle, buf, buf_len, timeout, timeout_left);
}

#endif // BUFFERED_IO_H_
