/**
 * @file obc_mibspi.c
 * @brief Low-level MIBSPI driver wrapper.
 * @author Andrada Zoltan, Julian Mentasti
 * @date March 3, 2021
 */

#include "obc_mibspi.h"
#include "gio.h"
#include "obc_misra.h"
#include "obc_hardwaredefs.h"

/* External Variable Initializations -----------------------------------------*/
EventGroupHandle_t xMibspiEventGroupHandle;
SemaphoreHandle_t xMibspiMutexHandle;

StaticEventGroup_t xMibspiStaticEventGroup;
StaticSemaphore_t xMibspiStaticMutex;

/* API Functions -------------------------------------------------------------*/

/**
 * @brief Initializes the mibSPI peripheral and enables the appropriate transfer group
 * interrupts.
 */
void mibspi_create_infra(void) {
    // Initialize RTOS event group
    xMibspiEventGroupHandle = xEventGroupCreateStatic(&xMibspiStaticEventGroup);

    // Initialize MIBSPI mutex
    xMibspiMutexHandle = xSemaphoreCreateMutexStatic(&xMibspiStaticMutex);
}

/**
 * @brief Enables MIBSPI interrupts.
 *
 * The interrupts for various MIBSPI transfer groups are set up here.
 * When adding a new transfer group, be sure to enable notifications (interrupts) on it.
 */
void mibspi_init_irq(void) {

#ifndef PLATFORM_ALEA_V1 // TODO this is temporary until the flash driver and RTC are configured for ALEASAT
                         // (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/56)
                         // (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/58)
    mibspiEnableGroupNotification(RTC_TIME_MIBSPI_REG, RTC_TIME_MIBSPI_GROUP, RTC_DATA_FORMAT);
    mibspiEnableGroupNotification(RTC_REG_MIBSPI_REG, RTC_REG_MIBSPI_GROUP, RTC_DATA_FORMAT);
    mibspiEnableGroupNotification(FLASH_MIBSPI_REG, FLASH_1_BYTE_GROUP, FLASH_DATA_FORMAT);
    mibspiEnableGroupNotification(FLASH_MIBSPI_REG, FLASH_2_BYTE_GROUP, FLASH_DATA_FORMAT);
    mibspiEnableGroupNotification(FLASH_MIBSPI_REG, FLASH_4_BYTE_GROUP, FLASH_DATA_FORMAT);
    mibspiEnableGroupNotification(FLASH_MIBSPI_REG, FLASH_20_BYTE_GROUP, FLASH_DATA_FORMAT);
#endif // PLATFORM_ALEA_V1
#if defined(PLATFORM_ORCA_V1)
    mibspiEnableGroupNotification(MRAM_MIBSPI_REG, MRAM_1_BYTE_GROUP, MRAM_DATA_FORMAT);
    mibspiEnableGroupNotification(MRAM_MIBSPI_REG, MRAM_2_BYTE_GROUP, MRAM_DATA_FORMAT);
    mibspiEnableGroupNotification(MRAM_MIBSPI_REG, MRAM_4_BYTE_GROUP, MRAM_DATA_FORMAT);
    mibspiEnableGroupNotification(ECC_FLASH_MIBSPI_REG, ECC_FLASH_1_BYTE_GROUP, ECC_FLASH_DATA_FORMAT);
    mibspiEnableGroupNotification(ECC_FLASH_MIBSPI_REG, ECC_FLASH_2_BYTE_GROUP, ECC_FLASH_DATA_FORMAT);
    mibspiEnableGroupNotification(ECC_FLASH_MIBSPI_REG, ECC_FLASH_5_BYTE_GROUP, ECC_FLASH_DATA_FORMAT);
    mibspiEnableGroupNotification(ECC_FLASH_MIBSPI_REG, ECC_FLASH_22_BYTE_GROUP, ECC_FLASH_DATA_FORMAT);
#endif
}

/**
 * @brief Transmit data using a particular MibSPI transfer group.
 *
 * @pre @ref mibspi_init_hw
 * @pre mibspi_init_irq
 * @pre mibspi_create_infra
 *
 * @param tg: Transfer group struct
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t mibspi_transmit(mibspi_tg_t tg, uint16_t* tx_buffer) {
    return mibspi_base_tx(&tg, xMibspiEventGroupHandle, tx_buffer, SPI_TIMEOUT_MS);
}

/**
 * @brief Transmit data and wait for received data using a particular MibSPI transfer group.
 *
 * @pre @ref mibspi_init_hw
 * @pre mibspi_init_irq
 * @pre mibspi_create_infra
 *
 * @param tg: Transfer group struct
 * @param tx_buffer: Pointer to the data to transmit, length of data that is transmitted
 * depends on the transfer group size
 * @param rx_buffer: Pointer to where received data is stored, length of data that is
 * stored depends on the transfer group size
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t mibspi_transmit_receive(mibspi_tg_t tg, uint16_t* tx_buffer, uint16_t* rx_buffer) {
    return mibspi_base_tx_rx(&tg, xMibspiEventGroupHandle, tx_buffer, rx_buffer, SPI_TIMEOUT_MS);
}
