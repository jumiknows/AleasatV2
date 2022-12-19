/**
 * @file rtos_stream.h
 * @brief FreeRTOS stream wrappers
 */

#ifndef RTOS_STREAM_H_
#define RTOS_STREAM_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    /**
     * @brief Handle for the MessageBuffer
     */
    MessageBufferHandle_t msg_buf;
} rtos_msg_istream_t;

typedef struct {
    /**
     * @brief Handle for the MessageBuffer
     */
    MessageBufferHandle_t msg_buf;

    /**
     * @brief Handle for an optional mutex
     * 
     * If not NULL, writes to the msg_buf will only occur after the mutex
     * has been successfully obtained. This is useful for multi-writer,
     * single-reader scenarios.
     */
    SemaphoreHandle_t mutex;
} rtos_msg_ostream_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

uint32_t rtos_stream_read_msg(void *handle, uint8_t *buf, uint32_t buf_len, uint32_t timeout, uint32_t *timeout_left);
uint32_t rtos_stream_write_msg(void *handle, const uint8_t *data, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left);

void rtos_stream_handle_timeout(uint32_t start_ticks, uint32_t timeout, uint32_t *timeout_left);

#endif // RTOS_STREAM_H_
