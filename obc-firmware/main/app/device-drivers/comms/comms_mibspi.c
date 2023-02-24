/**
 * @file comms_mibspi.c
 *
 * @brief Low-level Comms driver using MIBSPI
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_mibspi.h"
#include "tms_mibspi.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_watchdog.h"
#include "obc_task_info.h"

// HAL
#include "het.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// Any MIBSPI transaction should not take more than this value
#define COMMS_MIBSPI_TIMEOUT_MS 100
#define COMMS_MIBSPI_NUM_BYTES  256

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static comms_dev_err_type_t comms_mibspi_tx(uint16_t* tx_buffer, uint16_t num_bytes);
static comms_dev_err_type_t comms_mibspi_rx(uint16_t* rx_buffer, uint16_t buffer_len);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief Definition of the MIBSPI transfer groups available for use when
 * communicating with the comms board.
 */
static const mibspi_tg_t COMMS_256_BYTES_TG = {
    .reg = COMMS_MIBSPI_REG,
    .transfer_group = COMMS_TRANSFER_GROUP,
    .cs_port = COMMS_CS_PORT,
    .cs_pin = COMMS_CS_PIN
};

/**
 * @brief Definition of the comms driver interface for this device
 */
comms_device_t cdev = {
    .tx = &comms_mibspi_tx,
    .rx = &comms_mibspi_rx,
    .cb = NULL,
    .cb_param = NULL
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Enables MIBSPI and COMMS_INT interrupts.
 */
void comms_mibspi_init(void) {
    mibspiEnableGroupNotification(COMMS_MIBSPI_REG, COMMS_TRANSFER_GROUP, COMMS_MIBSPI_TGI_LEVEL);
    edgeEnableNotification(COMMS_INT_REG, COMMS_INT_EDGE);
}

/**
 * @brief Invoke a registered ISR callback function
 *
 * @warning This is called in ISR context, must use ISR versions of RTOS functions
 */
void comms_mibspi_invoke_irq_cb(void) {
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    comms_dev_cb_func_t cb_func = cdev.cb;
    void* param = cdev.cb_param;

    if(cb_func != NULL) {
        cb_func(param);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Transmit 256 bytes to Comms.
 *
 * @pre @ref mibspi_init_hw
 * @pre comms_mibspi_init
 *
 * @param[in] tx_buffer pointer to the data to transmit, MUST be 256 bytes long
 *
 * @return COMMS_DEV_SUCCESS if no error
 *         COMMS_DEV_BUS_FAILURE otherwise
 */
static comms_dev_err_type_t comms_mibspi_tx(
    uint16_t* tx_buffer,
    uint16_t num_bytes
) {
    comms_dev_err_type_t ret = COMMS_DEV_SUCCESS;

    if(num_bytes > COMMS_MIBSPI_NUM_BYTES) {
        ret = COMMS_DEV_ERR_INVALID_ARG;
    }
    else if(tms_mibspi_tx(&COMMS_256_BYTES_TG,
                     tx_buffer, COMMS_MIBSPI_TIMEOUT_MS) != MIBSPI_NO_ERR) {
        ret = COMMS_DEV_ERR_BUS_FAILURE;
    }

    return ret;
}

/**
 * @brief Receive 256 bytes from Comms.
 *
 * @pre @ref mibspi_init_hw
 * @pre comms_mibspi_init
 *
 * @param[out] rx_buffer pointer to the data to receive, MUST be 256 bytes long
 *
 * @return COMMS_DEV_SUCCESS if no error,
 *         COMMS_DEV_BUS_FAILURE otherwise
 */
static comms_dev_err_type_t comms_mibspi_rx(
    uint16_t* rx_buffer,
    uint16_t buffer_len
) {
    comms_dev_err_type_t ret = COMMS_DEV_SUCCESS;

    if(buffer_len != COMMS_MIBSPI_NUM_BYTES) {
        ret = COMMS_DEV_ERR_INVALID_ARG;
    }
    else if(tms_mibspi_rx(&COMMS_256_BYTES_TG,
                     rx_buffer, COMMS_MIBSPI_TIMEOUT_MS) != MIBSPI_NO_ERR) {
        ret = COMMS_DEV_ERR_BUS_FAILURE;
    }

    return ret;
}

/**
 * @brief Get a driver handle for this device
 *
 * @return comms_dev_hdl_t opaque driver interface handle
 */
comms_dev_handle_t comms_mibspi_get_handle(void) {
    return (comms_dev_handle_t)&cdev;
}

