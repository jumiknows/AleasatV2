/**
 * @file io_stream.h
 * @brief Re-usable I/O stream definitions and functions
 */

#ifndef IO_STREAM_H_
#define IO_STREAM_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Representation of a continuous output stream
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
     * @brief Write data to a stream
     *
     * @param[in]  handle       Handle to identify the stream
     * @param[in]  data         Pointer to data to write
     * @param[in]  num_bytes    Number of bytes to write
     * @param[in]  timeout      Timeout (units determined by implementation)
     * @param[out] timeout_left Pointer to store remaining timeout after the write operation.
     *                          Can be NULL if the caller doesn't care about the remaining timeout.
     *
     * @return Number of bytes written
     */
    uint32_t (*write)(void *handle, const uint8_t *data, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left);

    /**
     * @brief Flush data buffered in a stream.
     *
     * This function pointer may be NULL if the stream does not support flushing.
     *
     * @param[in] handle        Handle to identify the stream
     * @param[in] timeout       Timeout (units determined by implementation)
     * @param[out] timeout_left Pointer to store remaining timeout after the write operation.
     *                          Can be NULL if the caller doesn't care about the remaining timeout.
     */
    bool (*flush)(void *handle, uint32_t timeout, uint32_t *timeout_left);
} io_ostream_t;

/**
 * @brief Representation of a continuous input stream
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
     * @brief Read data from a stream
     *
     * @param[in]  handle       Handle to identify the stream
     * @param[out] buf          Pointer to buffer where received data will be stored
     * @param[in]  num_bytes    Number of bytes to read
     * @param[in]  timeout      Timeout (units determined by implementation)
     * @param[out] timeout_left Pointer to store remaining timeout after the read operation.
     *                          Can be NULL if the caller doesn't care about the remaining timeout.
     *
     * @return Number of bytes read
     */
    uint32_t (*read)(void *handle, uint8_t *buf, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left);
} io_istream_t;

/******************************************************************************/
/*                       I N L I N E  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Wrapper for io_ostream_t->write() that automatically passes the handle
 */
static inline uint32_t io_stream_write(const io_ostream_t *stream, const uint8_t *data, uint32_t num_bytes, uint32_t timeout,
                                       uint32_t *timeout_left) {
    return stream->write(stream->handle, data, num_bytes, timeout, timeout_left);
}

/**
 * @brief Wrapper for io_ostream_t->flush() that automatically checks for NULL flush function and passes the handle
 */
static inline bool io_stream_flush(const io_ostream_t *stream, uint32_t timeout, uint32_t *timeout_left) {
    if (stream->flush != NULL) {
        return stream->flush(stream->handle, timeout, timeout_left);
    }

    return true;
}

/**
 * @brief Wrapper for io_istream_t->read() that automatically passes the handle
 */
static inline uint32_t io_stream_read(const io_istream_t *stream, uint8_t *buf, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left) {
    return stream->read(stream->handle, buf, num_bytes, timeout, timeout_left);
}

#endif // IO_STREAM_H_
