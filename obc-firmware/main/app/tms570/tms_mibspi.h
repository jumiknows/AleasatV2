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
#include "rtos.h"

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
 * @brief Return value for MIBSPI functions, indicating any errors.
 */
typedef enum {
    /**
     * @brief Indicates that there were no IO errors.
     */
    MIBSPI_NO_ERR = 0,

    /**
     * @brief Indicates that an overflow has occurred in the RX buffer,
     * and unread data was overwritten.
     */
    MIBSPI_RX_OVERFLOW = -1,

    /**
     * @brief Indicates there was a mismatch in the data that was transmitted
     * and the internal TX data.
     */
    MIBSPI_TX_BITERR = -2,

    /**
     * @brief Indicates a data length error has occurred.
     */
    MIBSPI_DATALEN_ERR = -3,

    /**
     * @brief Indicates a timeout has occurred in the MIBSPI transaction.
     */
    MIBSPI_TIMEOUT = -4,

    /**
     * @brief Indicates a failure to grab the MIBSPI mutex. Most likely a timeout.
     */
    MIBSPI_MUTEX_GRAB_ERR = -5,

    /**
     * @brief No mutex exists for the given MIBSPI transfer group
     */
    MIBSPI_MUTEX_INVALID_ERR = -6,

    /**
     * @brief No event group handle exists for the given MIBSPI transfer group
     */
    MIBSPI_EVENTGROUP_INVALID_ERR = -7,

    /**
     * @brief Unknown error occurred.
     */
    MIBSPI_UNKNOWN_ERR = -8
} mibspi_err_t;

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
void tms_mibspi_create_infra(void);
void tms_mibspi_init_hw(void);

/* User API */
mibspi_err_t tms_mibspi_tx(const mibspi_tg_t* tg, uint16_t* tx_buffer, uint32_t timeout);
mibspi_err_t tms_mibspi_rx(const mibspi_tg_t* tg, uint16_t* rx_buffer, uint32_t timeout);
mibspi_err_t tms_mibspi_tx_rx(const mibspi_tg_t* tg, uint16_t* tx_buffer, uint16_t* rx_buffer, uint32_t timeout);

/* Public helper function */
EventGroupHandle_t get_eventgroup_handle(mibspiBASE_t *reg);

#endif /* TMS_MIBSPI_H_ */
