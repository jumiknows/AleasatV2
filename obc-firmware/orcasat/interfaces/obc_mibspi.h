/**
 * @file obc_mibspi.h
 * @brief Low-level MIBSPI driver wrapper.
 * @author Andrada Zoltan
 * @date June 9, 2019
 */

#ifndef OBC_MIBSPI_H_
#define OBC_MIBSPI_H_

#include "sys_common.h"
#include "tms_mibspi.h"
#include "rtos.h"
#include "obc_error.h"

/**
 * @brief RTOS handles for data structures used in this driver.
 */
extern EventGroupHandle_t xMibspiEventGroupHandle;
extern SemaphoreHandle_t xMibspiMutexHandle;

/* Startup steps */
void mibspi_create_infra(void);
void mibspi_init_irq(void);

/* User API */
mibspi_err_t mibspi_transmit(mibspi_tg_t tg, uint16_t* tx_buffer);
mibspi_err_t mibspi_transmit_receive(mibspi_tg_t tg, uint16_t* tx_buffer, uint16_t* rx_buffer);

#endif /* OBC_MIBSPI_H_ */
