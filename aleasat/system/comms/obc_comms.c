/**
 * @file obc_comms.c
 * @brief Low-level MIBSPI comms driver wrapper.
 * @author Julian Mentasti, Michael Lin
 * @date Feb 20, 2021
 */

#include "obc_comms.h"
#include "gio.h"
#include "obc_misra.h"
#include "obc_hardwaredefs.h"

/**
 * @brief Definition of the MIBSPI transfer groups available for use when
 * communicating with the comms board.
 */
static const mibspi_tg_t COMMS_256_BYTES_TG = {.reg = COMMS_MIBSPI_REG, .transfer_group = COMMS_TRANSFER_GROUP, .cs_port = COMMS_CS_PORT, .cs_pin = COMMS_CS_PIN};

/* External Variable Initializations -----------------------------------------*/
EventGroupHandle_t xMibspiCommsEventGroupHandle;
SemaphoreHandle_t xMibspiCommsMutexHandle;

StaticEventGroup_t xMibspiCommsStaticEventGroup;
StaticSemaphore_t xMibspiCommsStaticMutex;

/* API Functions -------------------------------------------------------------*/

/**
 * @brief Initializes the mibSPI peripheral and enables the appropriate transfer group
 * interrupts.
 */
void comms_mibspi_create_infra(void) {
    // Initialize RTOS event group
    xMibspiCommsEventGroupHandle = xEventGroupCreateStatic(&xMibspiCommsStaticEventGroup);

    // Initialize MIBSPI mutex
    xMibspiCommsMutexHandle = xSemaphoreCreateMutexStatic(&xMibspiCommsStaticMutex);
}

/**
 * @brief Enables MIBSPI interrupts.
 *
 * Most importantly, the interrupts for various MIBSPI transfer groups are set up here.
 * When adding a new transfer group, be sure to enable notifications (interrupts) on it.
 */
void comms_mibspi_init_irq(void) {
    mibspiEnableGroupNotification(COMMS_MIBSPI_REG, COMMS_TRANSFER_GROUP, COMMS_DATA_FORMAT);
}

/**
 * @brief Transmit data to COMMS
 *
 * It will transfer 256 bytes worth of data,
 * with the default SPI_TIMEOUT_MS.
 *
 * @pre @ref mibspi_init_hw
 * @pre comms_mibspi_init_irq
 * @pre comms_mibspi_create_infra
 *
 * @param tx_buffer: pointer to the data to transmit
 * MUST be 256 bytes long
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t comms_mibspi_tx(uint16_t* tx_buffer) {
    return tms_mibspi_tx(&COMMS_256_BYTES_TG, xMibspiCommsEventGroupHandle, tx_buffer, SPI_TIMEOUT_MS);
}

/**
 * @brief Receive data from COMMS
 *
 * It will receive 256 bytes worth of data,
 * with the default SPI_TIMEOUT_MS.
 *
 * @pre @ref mibspi_init_hw
 * @pre comms_mibspi_init_irq
 * @pre comms_mibspi_create_infra
 *
 * @param rx_buffer: pointer to the data to receive
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t comms_mibspi_rx(uint16_t* rx_buffer) {
    return tms_mibspi_rx(&COMMS_256_BYTES_TG, xMibspiCommsEventGroupHandle, rx_buffer, SPI_TIMEOUT_MS);
}

/**
 * @brief Transmit data and wait for received data from Comms
 *
 * It will receive/recv 256 bytes worth of data
 * with the default SPI_TIMEOUT_MS.
 *
 * @pre @ref mibspi_init_hw
 * @pre comms_mibspi_init_irq
 * @pre comms_mibspi_create_infra
 *
 * @param tg: Transfer group struct
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 * @param rx_buffer: Pointer to where received data is stored, length of data that is
 * stored depends on the transfer group size
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t comms_mibspi_tx_and_rx(uint16_t* tx_buffer, uint16_t* rx_buffer) {
    return tms_mibspi_tx_rx(&COMMS_256_BYTES_TG, xMibspiCommsEventGroupHandle, tx_buffer, rx_buffer, SPI_TIMEOUT_MS);
}

/**
 * @brief Transmit data to COMMS
 *
 * It will transfer 256 bytes worth of data,
 * with a as specific timeout.
 *
 * @pre @ref mibspi_init_hw
 * @pre comms_mibspi_init_irq
 * @pre comms_mibspi_create_infra
 *
 * @param tx_buffer: pointer to the data to transmit
 * MUST be 256 bytes long
 * @param timeout: Timeout in ms for the transaction.
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t comms_to_mibspi_tx(uint16_t* tx_buffer, uint32_t timeout) {
    return tms_mibspi_tx(&COMMS_256_BYTES_TG, xMibspiCommsEventGroupHandle, tx_buffer, timeout);
}

/**
 * @brief Receive data from COMMS
 *
 * It will receive 256 bytes worth of data,
 * with a specific timeout.
 *
 * @pre @ref mibspi_init_hw
 * @pre comms_mibspi_init_irq
 * @pre comms_mibspi_create_infra
 *
 * @param rx_buffer: pointer to the data to receive
 * @param timeout: Timeout in ms for the transaction.
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t comms_to_mibspi_rx(uint16_t* rx_buffer, uint32_t timeout) {
    return tms_mibspi_rx(&COMMS_256_BYTES_TG, xMibspiCommsEventGroupHandle, rx_buffer, timeout);
}

/**
 * @brief Transmit data and wait for received data from Comms
 *
 * It will receive/recv 256 bytes worth of data,
 * with a specific timeout.
 * 
 * @pre @ref mibspi_init_hw
 * @pre comms_mibspi_init_irq
 * @pre comms_mibspi_create_infra
 *
 * @param tg: Transfer group struct
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 * @param rx_buffer: Pointer to where received data is stored, length of data that is
 * stored depends on the transfer group size
 * @param timeout: Timeout in ms for the transaction.
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t comms_to_mibspi_tx_and_rx(uint16_t* tx_buffer, uint16_t* rx_buffer, uint32_t timeout) {
    return tms_mibspi_tx_rx(&COMMS_256_BYTES_TG, xMibspiCommsEventGroupHandle, tx_buffer, rx_buffer, timeout);
}
