
/**
 * @file tms_uart.h
 * @brief Low level generic UART driver.
 * 
 * @note SCI communication drivers for the TMS570
 * 
 *  HALCOGEN SETUP:
 *      - Requires interrupt channel 13 to be set (SCI/LIN) RX interrupt.
 * 
 */

#ifndef OBC_UART_H_
#define OBC_UART_H_

#include "rtos.h"
#include "sci.h"
#include "printf.h"
#include "obc_error.h"

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
void gps_send(const uint8_t* data, uint32_t size_bytes);

/* Special Functions */
bool uart_get_rtos_mode(void);                                                           /* Getter for using rtos flag - required by sciNotification (htp_notification.c) */

#endif /* OBC_UART_H_ */
