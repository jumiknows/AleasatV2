/*
 * obc_uart.c
 *
 *  Created on: Feb 8, 2017
 *      Author: Richard Arthurs
 *      UART (serial port) API functions.
 *
 */
#include "obc_cmd.h"
#include <stdlib.h>
#include <string.h>
#include "sys_common.h"
#include "obc_uart.h"
#include "obc_hardwaredefs.h"
#include "obc_watchdog.h"
#include "printf.h"
#include "logger.h"
#include "obc_misra.h"
#include "obc_utils.h"
#include "obc_task_info.h"
#include "rtos_timer.h"

/**
 * @brief The maximum length of a command.
 */
#define MAX_CMD_LEN_BYTES 64

/**
 * @brief Depth of the receive queue in units of @ref MAX_CMD_LEN_BYTES.
 *
 * This should be sized according to how fast commands may be sent in relation
 * to how fast commands can be processed. Being able to buffer two commands seems
 * reasonable for now, as we have never had any commands be dropped.
 */
#define DEBUG_RX_QUEUE_DEPTH 2

/**
 * @brief Depth of GPS-UART RX queue in bytes
 *
 * Size of this queue is based on the typical size of BESTXYZ ASCII response messages from the OEM7X
 * module (~300 bytes). An arbitrary margin is added since ASCII message sizes can vary due to
 * plain-text representations of numerical parameters.
 */
#define GPS_RX_QUEUE_DEPTH 400

/**
 * @brief Track GPS-UART RX timeout and ensure that incomplete or missing responses from the module
 * don't hang the requesting task.
 *
 * After a request message is sent, a timer of duration GPS_UART_RX_TIMEOUT_MS is started, and the
 * RX response queue is polled at an interval of GPS_UART_RX_POLL_INTERVAL_MS. If no complete
 * response message is received after the timer has expired, then abort the send-and-receive
 * operation and signal a timeout error.
 */
static TimerHandle_t xUartRecvTimer;
#define GPS_UART_RX_TIMEOUT_MS 2000

/**
 * @brief The tick rate at which the UART RX queue contents are checked/processed
 */
#define UART_BUFFER_READ_INTERVAL_MS 100

/**
 * @brief Task buffers.
 */
StaticTask_t uart_task_buffer;
StackType_t uart_task_stack[UART_TASK_STACK_SIZE];

/**
 * @brief Buffers and mutex for debug UART.
 *
 * We use the debug UART for sending log messages and downlinking
 * files. These operations may occur in different tasks, so the hardware
 * needs a mutex around it.
 */
SemaphoreHandle_t debug_uart_mutex = NULL;
StaticSemaphore_t debug_uart_mutex_buffer;

unsigned char curr_debug_char = '\0';
unsigned char curr_gps_char   = '\0';

static bool using_rtos = false; /* Flag used to disregard incoming characters until RTOS is running */

QueueHandle_t xDebugSerialRXQueue;
QueueHandle_t xGpsSerialRXQueue;
TaskHandle_t xSerialTaskHandle = NULL;

static void gps_uart_timer_callback(TimerHandle_t timer_handle);
static inline void uart_send_bytes(const uint8_t* data, uint32_t size_bytes, sciBASE_t* uart_port);
static void vSerialTask(void* pvParameters); /* Serial TX/RX Task */
bool take_debug_uart_mutex(void);
void give_debug_uart_mutex(void);

/**
 * @brief xUartRecvTimer callback
 */
static void gps_uart_timer_callback(TimerHandle_t timer_handle) {
    // Callback needed as FreeRTOS timers don't seem to expire with a NULL function pointer..
}

/**
 * @brief Creates RTOS infrastructure that the UART requires to function.
 * @note This should occur first.
 * These queues are fed by the interrupt handler (RX) or are fed by the serial task,
 * so create the queues before enabling interrupts or starting the task.
 */
void uart_create_infra(void) {
    static StaticTimer_t xUartRecvTimerBuffer;

    debug_uart_mutex    = xSemaphoreCreateMutexStatic(&debug_uart_mutex_buffer);
    xDebugSerialRXQueue = xQueueCreate((MAX_CMD_LEN_BYTES * DEBUG_RX_QUEUE_DEPTH), sizeof(portCHAR));
    xGpsSerialRXQueue   = xQueueCreate(GPS_RX_QUEUE_DEPTH, sizeof(portCHAR));
    xUartRecvTimer      = xTimerCreateStatic("GPS UART recv timer", pdMS_TO_TICKS(GPS_UART_RX_TIMEOUT_MS), pdFALSE, NULL, &gps_uart_timer_callback, &xUartRecvTimerBuffer);
}

/**
 * @brief Initializes the SCI (UART) but does not turn on interrupts.
 *
 * The interrupts should be enabled once the task to process the interrupts
 * has been created.
 * @pre @ref uart_create_infra() has been called.
 */
void uart_init_hw(void) {
    sciInit(); // initialize the SCI HAL driver

    sciReceive(UART_DEBUG, 1, &curr_debug_char); // place into receive mode
    sciReceive(UART_GPS, 1, &curr_gps_char);     // place into receive mode
}

/**
 * @brief Enables the UART interrupts.
 * @pre @ref uart_init_hw() and uart_start_task() have been called.
 */
void uart_init_irq(void) {
    sciEnableNotification(UART_DEBUG, SCI_RX_INT);
    sciReceive(UART_DEBUG, 1, &curr_debug_char); // place into receive mode

    sciEnableNotification(UART_GPS, SCI_RX_INT);
    sciReceive(UART_GPS, 1, &curr_gps_char); // place into receive mode
}

/**
 * @brief Starts the serial processing task.
 * @pre @ref uart_create_infra() and @ref uart_start_irq_hw() have been called.
 * @pre The scheduler has been started because commands received on the UART are processed by the
 * scheduler.
 */
void uart_start_task(void) {
    xSerialTaskHandle = task_create_static(&vSerialTask, "serial", UART_TASK_STACK_SIZE, NULL, SERIAL_TASK_DEFAULT_PRIORITY, uart_task_stack, &uart_task_buffer);
}

/**
 * @brief Send an arbitrary number of bytes over the specified UART port
 *
 * @param data[in]: Pointer to the data to send.
 * @param size_bytes[in]: The number of bytes to send.
 * @param uart_port[in]: UART port base address.
 */
static inline void uart_send_bytes(const uint8_t* data, uint32_t size_bytes, sciBASE_t* uart_port) {
    // Disable MISRA warning about removing const. sciSend doesn't take a const ptr by design, although it does not modify it. (RA, March 3 2020)
    OBC_MISRA_CHECK_OFF
    sciSend(uart_port, size_bytes, (uint8_t*)data);
    OBC_MISRA_CHECK_ON
}

/**
 * @brief Send an arbitrary number of bytes over the debug UART port
 *
 * @param data[in]: Pointer to the data to send.
 * @param size_bytes[in]: The number of bytes to send.
 */
void serial_send(const uint8_t* data, uint32_t size_bytes) {
    take_debug_uart_mutex();
    uart_send_bytes(data, size_bytes, UART_DEBUG);
    give_debug_uart_mutex();
}

/**
 * @brief Basic debug UART string transmission.
 * @param str_to_send[in]: The string to send. Must be null terminated.
 */
void serial_send_string(const char* str_to_send) {
    take_debug_uart_mutex();
    uart_send_bytes((const uint8_t*)str_to_send, strlen(str_to_send), UART_DEBUG);
    give_debug_uart_mutex();
}

/**
 * @brief Basic GPS-UART string transmission.
 * @param str_to_send[in]: The string to send. Must be null terminated
 */
void gps_serial_send(const char* str_to_send) {
    uart_send_bytes((const uint8_t*)str_to_send, strlen(str_to_send), UART_GPS);
}

/**
 * @brief Gets the UART mutex (blocking forever) if the UART is in RTOS mode.
 * @return True when the mutex is acquired.
 */
bool take_debug_uart_mutex(void) {
    if (using_rtos) {
        return xSemaphoreTake(debug_uart_mutex, portMAX_DELAY);
    } else {
        return true;
    }
}

/**
 * @brief Gives the UART debug mutex.
 */
void give_debug_uart_mutex(void) {
    xSemaphoreGive(debug_uart_mutex);
}

/**
 * @brief Place bytes onto the queue for the GPS-UART to send.
 * Wait for a complete response message to arrive (delimited by /n or /r/n) or
 * until the receive timeout is triggered.
 *
 * @param str_to_send[in]: The string to send. Must be null terminated
 * @param gps_response_msg[out]: GPS-UART response buffer
 *
 * @returns UART_RX_OK if a complete response message was received
 * @returns UART_RX_TIMEOUT if a complete message was not received before timeout
 * @returns UART_RX_OVERFLOW if a complete message was not received before RX buffer overflow
 */
uart_err_t gps_serial_send_and_receive(const char* str_to_send, char* gps_response_msg) {
    gps_response_msg[0]           = '\0';
    uint16_t gps_rx_buf_idx       = 0;
    uint8_t gps_rx_curr_rcvd_char = '\0';
    uint8_t gps_rx_prev_rcvd_char = '\0';

    // GPS-UART RX timeout
    if (xTimerStart(xUartRecvTimer, 0) == pdFAIL) {
        log_str(ERROR, GPS_LOG, true, "GPS-UART RX timer init fail");
        return UART_RX_TIMEOUT;
    }

    // Make sure the RX buffer is cleared of stray bytes before forming a new message
    xQueueReset(xGpsSerialRXQueue);
    // Send the command message to the GPS module
    gps_serial_send(str_to_send);

    // Verify that the timer has actually been started by the scheduler
    while (!xTimerIsTimerActive(xUartRecvTimer)) {
        vTaskDelay(1);
    }

    bool resp_header_found   = false;
    bool resp_found          = false;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        if (xQueueReceive(xGpsSerialRXQueue, &gps_rx_curr_rcvd_char, 0) == pdTRUE) {
            // If command response header (i.e. <OK\r\n) not yet found, then wait for '<'
            if ((!resp_header_found) && (gps_rx_curr_rcvd_char == (uint8_t)'<')) {
                resp_header_found = true;
                xTimerReset(xUartRecvTimer, 0);
            }
            if (resp_header_found) {
                /* If command response not yet found, then wait for \r\n header terminator
                 * If command response found, then wait for final \r\n response terminator
                 */
                if ((gps_rx_curr_rcvd_char == '\n') && (gps_rx_prev_rcvd_char == '\r')) {
                    if (resp_found) {
                        gps_response_msg[gps_rx_buf_idx - 1] = '\0';
                        return UART_OK;
                    } else {
                        resp_found = true;
                    }
                }

                if (gps_rx_buf_idx >= GPS_RX_QUEUE_DEPTH) {
                    log_str(ERROR, GPS_LOG, true, "RX buffer len exceeded");
                    return UART_RX_OVERFLOW;
                }
                gps_response_msg[gps_rx_buf_idx++] = gps_rx_curr_rcvd_char;
            } else {
            }

            gps_rx_prev_rcvd_char = gps_rx_curr_rcvd_char;
        } else if (!xTimerIsTimerActive(xUartRecvTimer)) {
            log_str(ERROR, GPS_LOG, true, "RX timeout. Recv bytes: %d", gps_rx_buf_idx);
            return UART_RX_TIMEOUT;
        } else {
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(UART_BUFFER_READ_INTERVAL_MS));
        }
    }
}

/**
 * @brief Indicates whether the UART infrastructure has access to RTOS features or not.
 * @return True if the RTOS infrastructure is up, False if RTOS features cannot be relied upon for
 * UART
 */
bool uart_get_rtos_mode(void) {
    return using_rtos;
}

/**
 * This task is responsible for the handling of all UART related functions.
 *
 * It transmits characters coming in from the TX queue.
 * It receives characters from the ISR on the RX queue and passes them to the command handler.
 *
 * @param pvParameters (unused)
 */
static void vSerialTask(void* pvParameters) {
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    // Debug serial buffer
    char rx_buf[MAX_CMD_LEN_BYTES] = "";
    uint8_t rx_buf_idx             = 0;
    char rx_curr_rcvd_char         = '\0';

    using_rtos = true; /* Set flag so that UART functions will use RTOS features */
    set_task_status(wd_task_id, task_asleep);
    while (1) {
        // Block task until first byte placed in queue by ISR
        if (xQueueReceive(xDebugSerialRXQueue, &rx_curr_rcvd_char, portMAX_DELAY) == pdPASS) {
            set_task_status(wd_task_id, task_alive);
            vTaskDelay(1); // Allow additional characters in transmission to arrive before processing queue

            do {
                /* check for [CR] terminator and exclude from extracted command */
                if (rx_curr_rcvd_char == '\r') {
                    rx_buf[rx_buf_idx] = '\0';

                    obc_cmd_parse_and_invoke(rx_buf, EXTERNAL);

                    rx_buf_idx = 0;
                } else {
                    rx_buf[rx_buf_idx++] = rx_curr_rcvd_char;
                    if (rx_buf_idx >= MAX_CMD_LEN_BYTES) {
                        log_str(ERROR, CMD_GENERAL, true, "Max command len exceeded");
                        rx_buf_idx = 0;
                    }
                }

            } while (xQueueReceive(xDebugSerialRXQueue, &rx_curr_rcvd_char, 0) == pdPASS);

            set_task_status(wd_task_id, task_asleep);
        }
    }
}
