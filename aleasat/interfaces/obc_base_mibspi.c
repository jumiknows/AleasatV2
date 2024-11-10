/**
 * @file obc_mibspi.c
 * @brief Low-level Generic MIBSPI driver wrapper.
 * @author Andrada Zoltan, Julian Mentasti
 * @date March 3, 2021
 * 
 * Any MIBSPI driver must use this driver wrapper. 
 * That driver must have:
 * - EventGroupHandle_t
 * - StaticStemaphore_t
 * declared and initiated in their higher level driver.
 * 
 * They need to also have the following configured:
 * - transfer group
 * - data format
 * and must have enabled transfer group notifications.
 * 
 * This driver assumes that you are using negative SPI clock polarity
 * and using a GIO pin as the slave select.
 */

#include "obc_base_mibspi.h"
#include "gio.h"
#include "obc_misra.h"
#include "obc_hardwaredefs.h"

/* Private Function Prototypes -----------------------------------------------*/
static mibspi_err_t mibspi_error_handler(uint32_t error_flags);

/**
 * @brief Initializes MIBSPI hardware.
 */
void mibspi_init_hw(void) {
    mibspiInit();
}

/**
 * @brief Transmit data using a particular MibSPI transfer group and EventGroup
 *
 * @pre @ref mibspi_init_hw
 * @pre event group notification has been enabled
 *
 * @param tg: Transfer group struct
 * @param eg_handle: Event group handle
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 * @param timeout: Timeout in ms for the transaction.
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t mibspi_base_tx(const mibspi_tg_t* tg, EventGroupHandle_t eg_handle, uint16_t* tx_buffer, uint32_t timeout) {
    EventBits_t uxBits;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(timeout);

    /* Waiving MISRA check due to false positive result for 12.2 (AZ) */
    OBC_MISRA_CHECK_OFF
    // Set up data to be transferred
    mibspiSetData(tg->reg, tg->transfer_group, tx_buffer);

    // Enable CS
    gioSetBit(tg->cs_port, tg->cs_pin, 0);

    // Transfer Data
    mibspiTransfer(tg->reg, tg->transfer_group);
    OBC_MISRA_CHECK_ON

    // Wait for either a MIBSPI error interrupt or a MIBSPI transfer complete
    // interrupt to signal

    uxBits = xEventGroupWaitBits(
    		eg_handle,
			((uint8_t)((uint8_t)1U << tg->transfer_group)) | MIBSPI_ERR_NOTIF,
			pdTRUE,   /* The bits should be cleared before returning. */
			pdFALSE,  /* Wait for only one bit. */
			xTicksToWait);

    // Disable CS
    /* Waiving MISRA check due to false positive result for 12.2 (AZ) */
    OBC_MISRA_CHECK_OFF
    gioSetBit(tg->cs_port, tg->cs_pin, 1);
    OBC_MISRA_CHECK_ON

    if (uxBits & MIBSPI_ERR_NOTIF) {
        // Read error flag register
        return mibspi_error_handler(tg->reg->FLG);
    } else if (uxBits == 0) {
        // If no bits were set, the event group wait call timed out
        return MIBSPI_TIMEOUT;
    } else {
        return MIBSPI_NO_ERR;
    }
}

/**
 * @brief Receive data using a particular MibSPI transfer group.
 * The data received will be the same size as the transfer group size.
 * @pre mibspi_init_hw
 * @pre event group notification has been enabled
 *
 * @param tg: Transfer group struct
 * @param eg_handle: Event group handle
 * @param rx_buffer: pointer to the data to receive
 * @param timeout: Timeout in ms for the transaction.
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t mibspi_base_rx(const mibspi_tg_t* tg, EventGroupHandle_t eg_handle, uint16_t* rx_buffer, uint32_t timeout) {
    uint16_t empty_data[16] = {0x0000};
    uint16_t* data_ptr = empty_data;
    // Transmit empty data, set the clock and slave select to allow the
    // slave to simultaneously send data.
    mibspi_err_t err = mibspi_base_tx(tg, eg_handle, data_ptr, timeout);
    if (err != MIBSPI_NO_ERR) {
        return err;
    }

    /* Waiving MISRA check due to false positive result for 12.2 (AZ) */
    OBC_MISRA_CHECK_OFF
	// Move data from the receive buffer to rx_buffer.
    uint32_t error_flags = mibspiGetData(tg->reg, tg->transfer_group, rx_buffer);
    OBC_MISRA_CHECK_ON

    // check for errors
    return mibspi_error_handler(error_flags);
}

/**
 * @brief Transmit data and wait for received data using a particular MibSPI transfer group.
 * and Event group
 *
 * @pre mibspi_init_hw
 * @pre event group notification has been enabled
 *
 * @param tg: Transfer group struct
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 * @param rx_buffer: Pointer to where received data is stored, length of data that is
 * stored depends on the transfer group size
 * @param timeout: Timeout in ms for the transaction.
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t mibspi_base_tx_rx(const mibspi_tg_t* tg, EventGroupHandle_t eg_handle, uint16_t* tx_buffer, uint16_t* rx_buffer, uint32_t timeout) {
    // Transmit data
    mibspi_err_t err = mibspi_base_tx(tg, eg_handle, tx_buffer, timeout);
    if (err != MIBSPI_NO_ERR) {
        return err;
    }

    // Recv data
    OBC_MISRA_CHECK_OFF
    uint32_t error_flags = mibspiGetData(tg->reg, tg->transfer_group, rx_buffer);
    OBC_MISRA_CHECK_ON

    return mibspi_error_handler(error_flags);
}

/**
 * @brief MIBSPI error handler. Parses the MibSPI error register to check
 * if an error has occurred.
 *
 * @pre @ref mibspi_init
 *
 * @param error_flags: Copy of the contents in the MibSPI error register.
 * @return: Error code
 */
static mibspi_err_t mibspi_error_handler(uint32_t error_flags) {
    // No error, or ignore RX overflow error
    if ((error_flags == 0) || ((error_flags & 0x40) != 0)) {
        return MIBSPI_NO_ERR;
    }

    // TX bit error
    if ((error_flags & 0x08) != 0) {
        return MIBSPI_TX_BITERR;
    }

    // Data length error
    if ((error_flags & 0x01) != 0) {
        return MIBSPI_DATALEN_ERR;
    }

    return MIBSPI_UNKNOWN_ERR;
}
