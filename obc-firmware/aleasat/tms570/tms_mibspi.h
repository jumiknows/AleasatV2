/**
 * @file tms_mibspi.h
 * @brief Low-level Generic MIBSPI driver wrapper.
 */

#ifndef TMS_MIBSPI_H_
#define TMS_MIBSPI_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_error.h"

// HAL
#include "mibspi.h"
#include "sys_common.h"

// FreeRtos
#include "FreeRTOS.h"
#include "rtos_event_groups.h"
#include "rtos_semphr.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Default timeout for SPI transactions.
 */
#define SPI_TIMEOUT_MS 100

/**
 * @brief In the event group wait bits, the 8th bit is used to indicate whether or
 * not a MIBSPI error has occurred. In the MIBSPI error callback (@ref mibspiNotification
 * in @ref orcasat/obc_notification.c, this bit will be set which signals waiting threads
 * of an error.
 */
#define MIBSPI_ERR_NOTIF ((EventBits_t)1U << 8)


/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Wrapper for a MIBSPI transfer group.
 *
 * Contains a pointer to the mibspiBASE_t* to be used in this transaction,
 * as well as information about the chip select.
 */
typedef struct {
    mibspiBASE_t* reg;
    uint8_t transfer_group;
    gioPORT_t* cs_port;
    uint32_t cs_pin;
} mibspi_tg_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/* Startup steps */
void mibspi_init_hw(void);

/* User API */
mibspi_err_t tms_mibspi_tx(const mibspi_tg_t* tg, EventGroupHandle_t eg_handle, uint16_t* tx_buffer, uint32_t timeout);
mibspi_err_t tms_mibspi_rx(const mibspi_tg_t* tg, EventGroupHandle_t eg_handle, uint16_t* rx_buffer, uint32_t timeout);
mibspi_err_t tms_mibspi_tx_rx(const mibspi_tg_t* tg, EventGroupHandle_t eg_handle, uint16_t* tx_buffer, uint16_t* rx_buffer, uint32_t timeout);

#endif /* TMS_MIBSPI_H_ */
