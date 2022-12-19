/**
 * @file obc_cmd_serial.h
 * @brief OBC command system serial handler
 * 
 * TODO: This whole module will be removed, but it was created temporarily
 * to separate the changes to the serial driver and the command system.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_cmd_serial.h"

// Logger
#include "logger.h"

// OBC
#include "obc_cmd.h"
#include "obc_serial_socket.h"
#include "obc_watchdog.h"
#include "obc_task_info.h"

// Utils
#include "io_stream.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief The maximum length of a command.
 */
#define MAX_CMD_LEN_BYTES 64

/**
 * @brief The tick rate at which the UART RX queue contents are checked/processed
 */
#define UART_BUFFER_READ_INTERVAL_MS 100

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void vSerialTask(void* pvParameters);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief Task buffers.
 */
StaticTask_t uart_task_buffer;
StackType_t uart_task_stack[CMD_TASK_STACK_SIZE];

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Starts the serial processing task.
 * @pre @ref uart_create_infra() and @ref uart_start_irq_hw() have been called.
 * @pre The scheduler has been started because commands received on the UART are processed by the
 * scheduler.
 */
void obc_cmd_serial_start_task(void) {
    task_create_static(&vSerialTask, "cmd_serial", CMD_TASK_STACK_SIZE, NULL, CMD_TASK_PRIORITY, uart_task_stack, &uart_task_buffer);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

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

    set_task_status(wd_task_id, task_asleep);
    while (1) {
        // Block task until first byte placed in queue by ISR
        while (io_stream_read(&obc_serial_socket_in, (uint8_t *)&rx_curr_rcvd_char, 1, portMAX_DELAY, NULL) == 1) {
            set_task_status(wd_task_id, task_alive);
            /* check for [CR] terminator and exclude from extracted command */
            if (rx_curr_rcvd_char == '\r') {
                rx_buf[rx_buf_idx] = '\0';

                obc_cmd_parse_and_invoke(rx_buf, EXTERNAL);

                rx_buf_idx = 0;
            } else {
                rx_buf[rx_buf_idx++] = rx_curr_rcvd_char;
                if (rx_buf_idx >= MAX_CMD_LEN_BYTES) {
                    log_str(ERROR, LOG_CMD_GENERAL, "Max command len exceeded");
                    rx_buf_idx = 0;
                }
            }
        }
    }
}
