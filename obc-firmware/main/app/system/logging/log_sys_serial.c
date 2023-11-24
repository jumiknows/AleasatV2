/**
 * @file log_sys_serial.c
 * @brief Log system serial driver
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "log_sys_serial.h"

// OBC
#include "obc_serial_tx.h"
#include "obc_serial_defs.h"
#include "obc_rtos.h"
#include "obc_watchdog.h"
#include "obc_tasks_ids_gen.h"

// Utils
#include "rtos_stream.h"
#include "io_stream.h"
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define LOGS_MSG_BUFFER_SIZE 255U

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static MessageBufferHandle_t logs_msg_buffer = NULL;

/**
 * @brief Recursive mutex to protect logs so that the logging functions themselves
 * can log errors.
 */
static SemaphoreHandle_t logs_out_mutex = NULL;

static rtos_msg_ostream_t logs_msg_stream = {
    .msg_buf = NULL, // Assigned in log_sys_serial_create_infra
    .mutex   = NULL, // Assigned in log_sys_serial_create_infra
};

static uint8_t logs_data_buf[OBC_SERIAL_DATAGRAM_MAX_DATA_SIZE];
static obc_serial_tx_task_params_t logs_task_params = {
    .task_id       = OBC_TASK_ID_OBC_SERIAL_TX_LOGS,
    .datagram_type = OBC_SERIAL_DATAGRAM_LOG,
    .msg_buf       = NULL, // Assigned in obc_serial_tx_create_infra
    .data_buf      = logs_data_buf,
    .data_buf_len  = sizeof(logs_data_buf),
};

static const io_ostream_t obc_serial_logs_out = {
    .handle = &logs_msg_stream,
    .write  = &rtos_stream_write_msg,
    .flush  = NULL,
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize FreeRTOS data structures for log system serial driver
 */
void log_sys_serial_create_infra(void) {
    static StaticStreamBuffer_t logs_msg_buffer_buf                  = { 0 };
    static uint8_t logs_msg_buffer_storage[LOGS_MSG_BUFFER_SIZE + 1] = { 0 }; // See https://www.freertos.org/xMessageBufferCreateStatic.html for explanation of + 1

    logs_msg_buffer = xMessageBufferCreateStatic(LOGS_MSG_BUFFER_SIZE, logs_msg_buffer_storage, &logs_msg_buffer_buf);
    logs_msg_stream.msg_buf  = logs_msg_buffer;
    logs_task_params.msg_buf = logs_msg_buffer;

    static StaticSemaphore_t logs_out_mutex_buf;
    logs_out_mutex = xSemaphoreCreateRecursiveMutexStatic(&logs_out_mutex_buf);
    logs_msg_stream.mutex = logs_out_mutex;
}

/**
 * @brief Create task for transmitting logs
 */
void log_sys_serial_create_task(void) {
    obc_serial_tx_create_task(OBC_TASK_ID_OBC_SERIAL_TX_LOGS, &logs_task_params, OBC_WATCHDOG_ACTION_ALLOW);
}

/**
 * @brief Write a log to the serial output stream.
 * 
 * This function uses a mutex internally and therefore may be called from multiple threads.
 * 
 * @param log_data      Pointer to data buffer to log
 * @param data_len      Length of data to log
 * @param timeout_ticks Timeout in ticks
 */
void log_sys_serial_write_log(const uint8_t *log_data, uint8_t data_len, uint32_t timeout_ticks) {
    io_stream_write(&obc_serial_logs_out, log_data, data_len, timeout_ticks, NULL);
}
