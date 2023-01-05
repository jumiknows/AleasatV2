/**
 * @file rtos_stream.c
 * @brief FreeRTOS stream wrappers
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "rtos_stream.h"

// Utils
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief buffered_block_istream_t compatible API to read from a FreeRTOS MessageBuffer.
 * 
 * The handle must be of type rtos_msg_istream_t.
 * 
 * See buffered_block_istream_t in buffered_io.h for details.
 */
uint32_t rtos_stream_read_msg(void *handle, uint8_t *buf, uint32_t buf_len, uint32_t timeout, uint32_t *timeout_left) {
    const rtos_msg_istream_t *stream = (const rtos_msg_istream_t *)handle;

    // Abort if msg_buf is NULL
    if (stream->msg_buf == NULL) {
        if (timeout_left != NULL) {
            *timeout_left = timeout;
        }
        return 0;
    }

    uint32_t start = xTaskGetTickCount();
    uint32_t msg_len = xMessageBufferReceive(stream->msg_buf, buf, buf_len, timeout);
    rtos_stream_handle_timeout(start, timeout, timeout_left);

    return msg_len;
}

/**
 * @brief io_ostream_t compatible API to write to a FreeRTOS MessageBuffer.
 * 
 * The handle must be of type rtos_msg_ostream_t.
 * 
 * See io_ostream_t in io_stream.h for details.
 */
uint32_t rtos_stream_write_msg(void *handle, const uint8_t *data, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left) {
    const rtos_msg_ostream_t *stream = (const rtos_msg_ostream_t *)handle;

    // Abort if msg_buf is NULL
    if (stream->msg_buf == NULL) {
        if (timeout_left != NULL) {
            *timeout_left = timeout;
        }
        return 0;
    }

    // Timeout tracking
    uint32_t start = xTaskGetTickCount();
    uint32_t timeout_left_internal = timeout;

    // Obtain mutex if necessary
    if (stream->mutex != NULL) {
        if (xSemaphoreTake(stream->mutex, timeout) == pdFALSE) {
            return 0;
        }

        // Update timeout since we may have blocked from some time on the mutex
        rtos_stream_handle_timeout(start, timeout, &timeout_left_internal);
    }

    // Write to the message buffer
    uint32_t bytes_written = xMessageBufferSend(stream->msg_buf, data, num_bytes, timeout_left_internal);

    // Release mutex if necessary
    if (stream->mutex != NULL) {
        xSemaphoreGive(stream->mutex);
    }

    // Update timeout
    rtos_stream_handle_timeout(start, timeout, timeout_left);

    return bytes_written;
}

/**
 * @brief io_istream_t compatible API to read from a FreeRTOS StreamBuffer.
 * 
 * The handle must be of type rtos_stream_istream_t.
 * 
 * See io_istream_t in io_stream.h for details.
 */
uint32_t rtos_stream_read_stream(void *handle, uint8_t *buf, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left) {
    const rtos_stream_istream_t *stream = (const rtos_stream_istream_t *)handle;

    // Abort if stream_buf is NULL
    if (stream->stream_buf == NULL) {
        if (timeout_left != NULL) {
            *timeout_left = timeout;
        }
        return 0;
    }

    uint32_t start = xTaskGetTickCount();

    xStreamBufferSetTriggerLevel(stream->stream_buf, num_bytes);
    uint32_t bytes_read = xStreamBufferReceive(stream->stream_buf, buf, num_bytes, timeout);

    rtos_stream_handle_timeout(start, timeout, timeout_left);

    return bytes_read;
}

/**
 * @brief io_ostream_t compatible API to write to a FreeRTOS StreamBuffer.
 * 
 * The handle must be of type rtos_stream_ostream_t.
 * 
 * See io_ostream_t in io_stream.h for details.
 */
uint32_t rtos_stream_write_stream(void *handle, const uint8_t *data, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left) {
    const rtos_stream_ostream_t *stream = (const rtos_stream_ostream_t *)handle;

    // Abort if stream_buf is NULL
    if (stream->stream_buf == NULL) {
        if (timeout_left != NULL) {
            *timeout_left = timeout;
        }
        return 0;
    }

    // Timeout tracking
    uint32_t start = xTaskGetTickCount();
    uint32_t timeout_left_internal = timeout;

    // Obtain mutex if necessary
    if (stream->mutex != NULL) {
        if (xSemaphoreTake(stream->mutex, timeout) == pdFALSE) {
            return 0;
        }

        // Update timeout since we may have blocked from some time on the mutex
        rtos_stream_handle_timeout(start, timeout, &timeout_left_internal);
    }

    // Write to the stream buffer
    uint32_t bytes_written = xStreamBufferSend(stream->stream_buf, data, num_bytes, timeout_left_internal);

    // Release mutex if necessary
    if (stream->mutex != NULL) {
        xSemaphoreGive(stream->mutex);
    }

    // Update timeout
    rtos_stream_handle_timeout(start, timeout, timeout_left);

    return bytes_written;
}

/**
 * @brief Helper function to handle calculating the remaining timeout in a FreeRTOS context.
 * 
 * The timeout_left is calculated as the start subtracted from the current tick count,
 * UNLESS the timeout was portMAX_DELAY in which case the timeout_left always remains portMAX_DELAY
 * to maintain the infinite blocking behaviour.
 * 
 * @param[in]  start        Tick count before the operation
 * @param[in]  timeout      Timeout (in ticks)
 * @param[out] timeout_left Pointer to where the remaining timeout (in ticks) will be stored
 */
void rtos_stream_handle_timeout(uint32_t start_ticks, uint32_t timeout, uint32_t *timeout_left) {
    if (timeout_left != NULL) {
        if (timeout == portMAX_DELAY) {
            *timeout_left = portMAX_DELAY;
        } else {
            uint32_t elapsed = (xTaskGetTickCount() - start_ticks);
            *timeout_left = timeout - MIN(timeout, elapsed);
        }
    }
}
