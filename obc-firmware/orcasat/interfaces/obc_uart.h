/*
 * obc_uart.h
 *
 *  Created on: Feb 8, 2017
 *      Author: ORCASAT C&DH Team
 *
 *      SCI communication drivers for the TMS570
 *		HALCOGEN SETUP:
 *      		- Requires interrupt channel 13 to be set (SCI/LIN) RX interrupt
 */

#ifndef OBC_UART_H_
#define OBC_UART_H_

#include "rtos.h"
#include "sci.h"
#include "printf.h"
#include "obc_error.h"

/**
 * @brief Size of GPS-UART RX buffer in bytes
 *
 * Size of this buffer is based on the typical size of BESTXYZ ASCII response messages from the
 * OEM7X module (~300 bytes). An arbitrary margin is added since ASCII message sizes can vary due to
 * plain-text representations of numerical parameters.
 */
#define GPS_RX_BUFFER_SIZE 400

/* UART Globals, Task and Queue Handles */
extern TaskHandle_t xSerialTaskHandle;
extern QueueHandle_t xDebugSerialRXQueue;
extern QueueHandle_t xGpsSerialRXQueue;
extern unsigned char curr_debug_char;
extern unsigned char curr_gps_char;

/* Startup Steps */
void uart_create_infra(void);
void uart_init_hw(void);
void uart_start_task(void);
void uart_init_irq(void);

/* General API */
void serial_send(const uint8_t* data, uint32_t size_bytes); /* Debug port arbitrary data send */
void serial_send_string(const char* str_to_send);           /* Debug port string send */
void gps_serial_send(const char* str_to_send);              /* GPS-UART port string send */

/* Special Functions */
uart_err_t gps_serial_send_and_receive(const char* str_to_send, char* gps_response_msg); /* GPS-UART port string send and receive */
bool uart_get_rtos_mode(void);                                                           /* Getter for using rtos flag - required by sciNotification (htp_notification.c) */

#endif /* OBC_UART_H_ */
