/**
 * @file obc_comms.c
 * @brief Low-level MIBSPI COMMS driver wrapper.
 * @author Julian Mentasti, Michael Lin
 * @date Feb 20, 2021
 */

#ifndef OBC_COMMS_H_
#define OBC_COMMS_H_

#include "sys_common.h"
#include "obc_base_mibspi.h"
#include "FreeRTOS.h"
#include "rtos_event_groups.h"
#include "rtos_semphr.h"
#include "obc_error.h"

/**
 * @brief Timeout for COMMS mutex grab.
 */
#define COMMS_MUTEX_TIMEOUT_MS 200

/**
 * @brief RTOS handles for data structures used in this driver.
 */
extern EventGroupHandle_t xMibspiCommsEventGroupHandle;
extern SemaphoreHandle_t xMibspiCommsMutexHandle;

/* Startup steps */
void comms_mibspi_create_infra(void);
void comms_mibspi_init_irq(void);

/* User API */
mibspi_err_t comms_mibspi_tx(uint16_t* tx_buffer);
mibspi_err_t comms_mibspi_rx(uint16_t* rx_buffer);
mibspi_err_t comms_mibspi_tx_and_rx(uint16_t* tx_buffer, uint16_t* rx_buffer);

/* User Advanced API */
mibspi_err_t comms_to_mibspi_tx(uint16_t* tx_buffer, uint32_t timeout);
mibspi_err_t comms_to_mibspi_rx(uint16_t* rx_buffer, uint32_t timeout);
mibspi_err_t comms_to_mibspi_tx_and_rx(uint16_t* tx_buffer, uint16_t* rx_buffer, uint32_t timeout);

#endif /* OBC_COMMS_H_ */
