/**
 * @file tms_mibspi.c
 * @brief Low-level Generic MIBSPI driver wrapper.
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

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "tms_mibspi.h"

// OBC
#include "obc_hardwaredefs.h"
#include "logger.h"

// HAL
#include "gio.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Timeout for MIBSPI mutex grab.
 */
#define MUTEX_TIMEOUT_MS 200

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

EventGroupHandle_t xMibspi1EventGroupHandle;
EventGroupHandle_t xMibspi3EventGroupHandle;
EventGroupHandle_t xMibspi5EventGroupHandle;

static SemaphoreHandle_t xMibspi1MutexHandle;
static SemaphoreHandle_t xMibspi3MutexHandle;
static SemaphoreHandle_t xMibspi5MutexHandle;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static mibspi_err_t mibspi_tx(const mibspi_tg_t *tg, EventGroupHandle_t eg_handle, uint16_t *tx_buffer, uint32_t timeout);
static mibspi_err_t mibspi_rx(const mibspi_tg_t *tg, EventGroupHandle_t eg_handle, uint16_t *rx_buffer, uint32_t timeout);
static mibspi_err_t mibspi_tx_rx(const mibspi_tg_t *tg, EventGroupHandle_t eg_handle, uint16_t *tx_buffer, uint16_t *rx_buffer, uint32_t timeout);
static mibspi_err_t mibspi_tx_error_handler(uint32_t error_flags);
static mibspi_err_t mibspi_rx_error_handler(uint32_t error_flags);
static SemaphoreHandle_t get_mutex_handle(mibspiBASE_t *reg);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the mibSPI peripheral and enables the appropriate transfer group
 * interrupts.
 */
void tms_mibspi_pre_init(void) {
    static StaticEventGroup_t xMibspi1StaticEventGroup;
    static StaticEventGroup_t xMibspi3StaticEventGroup;
    static StaticEventGroup_t xMibspi5StaticEventGroup;

    static StaticSemaphore_t xMibspi1StaticMutex;
    static StaticSemaphore_t xMibspi3StaticMutex;
    static StaticSemaphore_t xMibspi5StaticMutex;

    // Initialize RTOS event group
    xMibspi1EventGroupHandle = xEventGroupCreateStatic(&xMibspi1StaticEventGroup);
    xMibspi3EventGroupHandle = xEventGroupCreateStatic(&xMibspi3StaticEventGroup);
    xMibspi5EventGroupHandle = xEventGroupCreateStatic(&xMibspi5StaticEventGroup);

    // Initialize MIBSPI mutexes
    xMibspi1MutexHandle = xSemaphoreCreateMutexStatic(&xMibspi1StaticMutex);
    xMibspi3MutexHandle = xSemaphoreCreateMutexStatic(&xMibspi3StaticMutex);
    xMibspi5MutexHandle = xSemaphoreCreateMutexStatic(&xMibspi5StaticMutex);
}

/**
 * @brief Initializes MIBSPI hardware.
 */
void tms_mibspi_init_hw(void) {
    mibspiInit();
}

/**
 * @brief Transmit data using a particular MibSPI transfer group.
 *
 * @pre tms_mibspi_create_infra
 * @pre tms_mibspi_init_hw
 *
 * @param tg: Transfer group struct
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 * @param timeout: MIBSPI timeout in ms
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t tms_mibspi_tx(const mibspi_tg_t *tg, uint16_t *tx_buffer, uint32_t timeout) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    SemaphoreHandle_t mutex = get_mutex_handle(tg->reg);

    if (mutex == NULL) {
        err = MIBSPI_MUTEX_INVALID_ERR;
        return err;
    }

    EventGroupHandle_t eventgroup = get_eventgroup_handle(tg->reg);

    if (eventgroup == NULL) {
        err = MIBSPI_EVENTGROUP_INVALID_ERR;
        return err;
    }

    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        err = mibspi_tx(tg, eventgroup, tx_buffer, timeout);

        xSemaphoreGive(mutex);
    } else {
        err = MIBSPI_MUTEX_GRAB_ERR;
    }

    return err;
}

/**
 * @brief Receive data using a particular MibSPI transfer group.
 * The data received will be the same size as the transfer group size.
 *
 * @pre tms_mibspi_create_infra
 * @pre tms_mibspi_init_hw
 *
 * @param tg: Transfer group struct
 * @param rx_buffer: pointer to the data to receive
 * @param timeout: MIBSPI timeout in ms
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t tms_mibspi_rx(const mibspi_tg_t *tg, uint16_t *rx_buffer, uint32_t timeout) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    SemaphoreHandle_t mutex = get_mutex_handle(tg->reg);

    if (mutex == NULL) {
        err = MIBSPI_MUTEX_INVALID_ERR;
        return err;
    }

    EventGroupHandle_t eventgroup = get_eventgroup_handle(tg->reg);

    if (eventgroup == NULL) {
        err = MIBSPI_EVENTGROUP_INVALID_ERR;
        return err;
    }

    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        err = mibspi_rx(tg, eventgroup, rx_buffer, timeout);

        xSemaphoreGive(mutex);
    } else {
        err = MIBSPI_MUTEX_GRAB_ERR;
    }

    return err;
}

/**
 * @brief Transmit data and wait for received data using a particular MibSPI transfer group.
 *
 * @pre tms_mibspi_create_infra
 * @pre tms_mibspi_init_hw
 *
 * @param tg: Transfer group struct
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 * @param rx_buffer: Pointer to where received data is stored, length of data that is
 * stored depends on the transfer group size
 * @param timeout: MIBSPI timeout in ms
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t tms_mibspi_tx_rx(const mibspi_tg_t *tg, uint16_t *tx_buffer, uint16_t *rx_buffer, uint32_t timeout) {
    mibspi_err_t err = MIBSPI_NO_ERR;

    SemaphoreHandle_t mutex = get_mutex_handle(tg->reg);

    if (mutex == NULL) {
        err = MIBSPI_MUTEX_INVALID_ERR;
        return err;
    }

    EventGroupHandle_t eventgroup = get_eventgroup_handle(tg->reg);

    if (eventgroup == NULL) {
        err = MIBSPI_EVENTGROUP_INVALID_ERR;
        return err;
    }

    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(MUTEX_TIMEOUT_MS))) {
        err = mibspi_tx_rx(tg, eventgroup, tx_buffer, rx_buffer, SPI_TIMEOUT_MS);

        xSemaphoreGive(mutex);
    } else {
        err = MIBSPI_MUTEX_GRAB_ERR;
    }

    return err;
}

/**
 * @brief Get the event group for the corresponding transfer group
 *
 * @param reg: the mibspi register
 * @return EventGroupHandle_t: the relevant event group
 */
EventGroupHandle_t get_eventgroup_handle(mibspiBASE_t *reg) {
    EventGroupHandle_t handle = NULL;

    switch ((intptr_t)reg) {
    case (intptr_t)mibspiREG1: {
        handle = xMibspi1EventGroupHandle;
        break;
    }

    case (intptr_t)mibspiREG3: {
        handle = xMibspi3EventGroupHandle;
        break;
    }

    case (intptr_t)mibspiREG5: {
        handle = xMibspi5EventGroupHandle;
        break;
    }

    default: {
        break;
    }
    }

    return handle;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Transmit data using a particular MibSPI transfer group and EventGroup
 *
 * @pre @ref tms_mibspi_init_hw
 * @pre event group notification has been enabled
 *
 * @param tg: Transfer group struct
 * @param eg_handle: Event group handle
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 * @param timeout: Timeout in ms for the transaction.
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
static mibspi_err_t mibspi_tx(const mibspi_tg_t *tg, EventGroupHandle_t eg_handle, uint16_t *tx_buffer, uint32_t timeout) {
    EventBits_t uxBits;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(timeout);

    // Set up data to be transferred
    mibspiSetData(tg->reg, tg->transfer_group, tx_buffer);

    // Enable CS
    gioSetBit(tg->cs_port, tg->cs_pin, 0);

    // Transfer Data
    mibspiTransfer(tg->reg, tg->transfer_group);

    // Wait for either a MIBSPI error interrupt or a MIBSPI transfer complete
    // interrupt to signal

    uxBits = xEventGroupWaitBits(
                 eg_handle,
                 ((uint8_t)((uint8_t)1U << tg->transfer_group)) | MIBSPI_ERR_NOTIF,
                 pdTRUE,   /* The bits should be cleared before returning. */
                 pdFALSE,  /* Wait for only one bit. */
                 xTicksToWait);

    // Disable CS
    gioSetBit(tg->cs_port, tg->cs_pin, 1);

    if (uxBits & MIBSPI_ERR_NOTIF) {
        // Read error flag register
        return mibspi_tx_error_handler(tg->reg->FLG);
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
 * @pre tms_mibspi_init_hw
 * @pre event group notification has been enabled
 *
 * @param tg: Transfer group struct
 * @param eg_handle: Event group handle
 * @param rx_buffer: pointer to the data to receive
 * @param timeout: Timeout in ms for the transaction.
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
static mibspi_err_t mibspi_rx(const mibspi_tg_t *tg, EventGroupHandle_t eg_handle, uint16_t *rx_buffer, uint32_t timeout) {
    static uint16_t empty_data[128] = {0x0000};
    uint16_t *data_ptr = empty_data;
    // Transmit empty data, set the clock and slave select to allow the
    // slave to simultaneously send data.
    mibspi_err_t err = mibspi_tx(tg, eg_handle, data_ptr, timeout);

    if (err != MIBSPI_NO_ERR) {
        return err;
    }

    // Move data from the receive buffer to rx_buffer.
    uint32_t error_flags = mibspiGetData(tg->reg, tg->transfer_group, rx_buffer);

    // check for errors
    return mibspi_rx_error_handler(error_flags);
}

/**
 * @brief Transmit data and wait for received data using a particular MibSPI transfer group.
 * and Event group
 *
 * @pre tms_mibspi_init_hw
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
static mibspi_err_t mibspi_tx_rx(const mibspi_tg_t *tg, EventGroupHandle_t eg_handle, uint16_t *tx_buffer, uint16_t *rx_buffer, uint32_t timeout) {
    // Transmit data
    mibspi_err_t err = mibspi_tx(tg, eg_handle, tx_buffer, timeout);

    if (err != MIBSPI_NO_ERR) {
        return err;
    }

    // Recv data
    uint32_t error_flags = mibspiGetData(tg->reg, tg->transfer_group, rx_buffer);

    return mibspi_rx_error_handler(error_flags);
}

/**
 * @brief MIBSPI error handler. Parses the MibSPI flag register (SPIFLAG) to check
 * if an error has occurred.
 *
 * @pre @ref mibspi_init
 *
 * @param error_flags: Copy of the contents in the SPIFLAG register.
 * @return: Error code
 */
static mibspi_err_t mibspi_tx_error_handler(uint32_t error_flags) {
    // No error, TXINT or RXINT, or ignore RX overflow error
    if ((error_flags == 0) || ((error_flags & 0x40) != 0)
            || ((error_flags & 0x100) != 0) || ((error_flags & 0x200) != 0)) {
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

    return MIBSPI_UNKNOWN_TX_ERR;
}

/**
 * @brief Parse the flags from the RXRAM register (as returned by mibspiGetData - i.e. already
 * shifted and masked).
 *
 * @param error_flags Copy of the flags from the RXRAM register (bits 31-24) with TXFULL and RXEMPY
 *                    masked off
 * @return Error code
 */
static mibspi_err_t mibspi_rx_error_handler(uint32_t error_flags) {
    // Allow no errors or only RX Overflow error
    if ((error_flags == 0) || (error_flags == 0x40)) {
        return MIBSPI_NO_ERR;
    }

    // TX bit error
    if ((error_flags & 0x10) != 0) {
        return MIBSPI_TX_BITERR;
    }

    // Data length error
    if ((error_flags & 0x01) != 0) {
        return MIBSPI_DATALEN_ERR;
    }

    return MIBSPI_UNKNOWN_RX_ERR;
}

/**
 * @brief Get the mutex for the corresponding transfer group
 *
 * @param reg: the mibspi register
 * @return SemaphorHandle_t: the relevant mutex
 */
static SemaphoreHandle_t get_mutex_handle(mibspiBASE_t *reg) {
    SemaphoreHandle_t handle = NULL;

    switch ((intptr_t)reg) {
    case (intptr_t)mibspiREG1: {
        handle = xMibspi1MutexHandle;
        break;
    }

    case (intptr_t)mibspiREG3: {
        handle = xMibspi3MutexHandle;
        break;
    }

    case (intptr_t)mibspiREG5: {
        handle = xMibspi5MutexHandle;
        break;
    }

    default: {
        break;
    }
    }

    return handle;
}
