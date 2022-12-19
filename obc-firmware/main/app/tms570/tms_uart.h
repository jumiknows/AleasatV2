
/**
 * @file tms_uart.h
 * @brief Low level generic UART driver.
 * 
 * @note SCI communication drivers for the TMS570
 * 
 *  HALCOGEN SETUP:
 *      - Requires interrupt channel 13 to be set (SCI/LIN) RX interrupt.
 * 
 * TODO: This module will be removed once the new GPS driver is ready
 */

#ifndef OBC_UART_H_
#define OBC_UART_H_

#include "rtos.h"
#include "sci.h"
#include "printf.h"
#include "obc_error.h"

/* UART Globals, Task and Queue Handles */
extern QueueHandle_t xGpsSerialRXQueue;
extern unsigned char curr_gps_char;

/* Startup Steps */
void uart_create_infra(void);
void uart_init_hw(void);
void uart_init_irq(void);

/* General API */
void gps_send(const uint8_t* data, uint32_t size_bytes);

#endif /* OBC_UART_H_ */
