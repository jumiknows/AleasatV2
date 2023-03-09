/**
 * @file tms_uart.c
 * @brief Low level generic UART driver implementation.
 * 
 * TODO: This module will be removed once the new GPS driver is ready
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "tms_uart.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_watchdog.h"
#include "obc_utils.h"
#include "obc_task_info.h"
#include "sys_common.h"
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include "printf.h"
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

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
#define GPS_UART_RX_TIMEOUT_MS 2000

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/


/**
 * @brief Buffers and mutex for debug UART and GPS UART
 *
 * We use the debug UART for sending log messages and downlinking
 * files. These operations may occur in different tasks, so the hardware
 * needs a mutex around it.
 */
SemaphoreHandle_t gps_uart_mutex = NULL;
StaticSemaphore_t gps_uart_mutex_buffer;

unsigned char curr_gps_char = '\0';

QueueHandle_t xGpsSerialRXQueue;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static inline void uart_send_bytes(const uint8_t* data, uint32_t size_bytes, sciBASE_t* uart_port);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Creates RTOS infrastructure that the UART requires to function.
 * @note This should occur first.
 * These queues are fed by the interrupt handler (RX) or are fed by the serial task,
 * so create the queues before enabling interrupts or starting the task.
 */
void uart_create_infra(void) {
    gps_uart_mutex    = xSemaphoreCreateMutexStatic(&gps_uart_mutex_buffer);
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

    sciReceive(UART_GPS, 1, &curr_gps_char);     // place into receive mode
}

/**
 * @brief Enables the UART interrupts.
 * @pre @ref uart_init_hw() and uart_start_task() have been called.
 */
void uart_init_irq(void) {
    sciEnableNotification(UART_GPS, SCI_RX_INT);
    sciReceive(UART_GPS, 1, &curr_gps_char); // place into receive mode
}

/**
 * @brief Send an arbitrary number of bytes over the GPS UART port
 *
 * @param data[in]: Pointer to the data to send.
 * @param size_bytes[in]: The number of bytes to send.
 */
void gps_send(const uint8_t* data, uint32_t size_bytes) {
    xSemaphoreTake(gps_uart_mutex, portMAX_DELAY);
    uart_send_bytes(data, size_bytes, UART_GPS);
    xSemaphoreGive(gps_uart_mutex);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Send an arbitrary number of bytes over the specified UART port
 *
 * @param data[in]: Pointer to the data to send.
 * @param size_bytes[in]: The number of bytes to send.
 * @param uart_port[in]: UART port base address.
 */
static inline void uart_send_bytes(const uint8_t* data, uint32_t size_bytes, sciBASE_t* uart_port) {
    sciSend(uart_port, size_bytes, (uint8_t*)data);
}
