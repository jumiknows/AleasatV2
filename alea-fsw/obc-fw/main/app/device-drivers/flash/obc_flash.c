/**
 * @file obc_flash.c
 * @brief Generic driver for OBC flash device.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_flash.h"

// OBC
#include "obc_featuredefs.h"
#include "logger.h"
#include "flash_mt25ql.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define MANUFACTURER_ID_JEDEC_MICRON_TECHNOLOGY  0x20

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the flash.
 */
void flash_init(void) {
#if FEATURE_FLASH_FS
    mt25ql_init();
#else
    LOG_FLASH__FLASH_FS_NOT_ENABLED();
#endif
}

/**
 * @brief Prints the identity of the flash
 */
void flash_identify(void) {
#if FEATURE_FLASH_FS
    uint8_t device_data[20] = { 0x00 };
    mt25ql_read_id(device_data);

    uint8_t manufacturer = device_data[0];

    if (manufacturer == MANUFACTURER_ID_JEDEC_MICRON_TECHNOLOGY) {
        LOG_FLASH__IDENTIFIED_AS_MICRON();
    } else {
        LOG_FLASH__FAILED_TO_IDENTIFY();
    }

#else
    LOG_FLASH__IDENTIFY_NOT_SUPPORTED();
    LOG_FLASH__FLASH_FS_NOT_ENABLED();
#endif
}

/**
 * @brief Erases the flash in various sized chunks.
 *
 * @param addr: Address within the sector/block that is to be erased, must be byte-aligned.
 * @param erase_size: Block size to erase. This flash can do 64KB, 32KB, 4KB, or full flash.
 * @return FLASH_OK if no error, error code otherwise
 */
flash_err_t flash_erase(uint32_t addr, flash_erase_sz_t erase_size) {
#if FEATURE_FLASH_FS
    return mt25ql_erase(addr, erase_size);
#else
    LOG_FLASH__FLASH_FS_NOT_ENABLED();
    return FLASH_FS_DISABLED_ERR;
#endif
}

/**
 * @brief Program the flash with data to store.
 *
 * @param addr: Address to write to, must be byte-aligned
 * @param size: Amount of data to write in bytes
 * @param data: Buffer storing the data to write
 * @return: FLASH_OK if no error, error code otherwise
 */
flash_err_t flash_write(uint32_t addr, uint32_t size_bytes, const uint8_t *data) {
#if FEATURE_FLASH_FS

    if (size_bytes == 64) {
        return mt25ql_write_64(addr, data);
    } else {
        // Unhandled case: can only do 64 byte writes
        return FLASH_INVALID_SIZE_ERR;
    }

#else
    LOG_FLASH__FLASH_FS_NOT_ENABLED();
    return FLASH_FS_DISABLED_ERR;
#endif
}

/**
 * @brief Read data from the flash.
 *
 * @param addr: Address to read from, must be byte-aligned
 * @param size: Amount of data to read in bytes
 * @param data: Buffer where read data will be stored
 * @return: FLASH_OK if no error, error code otherwise
 */
flash_err_t flash_read(uint32_t addr, uint32_t size_bytes, uint8_t *data) {
#if FEATURE_FLASH_FS

    if (size_bytes == 64) {
        return mt25ql_read_64(addr, data);
    } else {
        // Unhandled case: can only do 64 byte reads
        return FLASH_INVALID_SIZE_ERR;
    }

#else
    LOG_FLASH__FLASH_FS_NOT_ENABLED();
    return FLASH_FS_DISABLED_ERR;
#endif
}

/**
 * @brief Place flash chip in deep power-down mode.
 */
flash_err_t flash_sleep(void) {
#if FEATURE_FLASH_FS
    return mt25ql_enter_deep_sleep();
#else
    LOG_FLASH__FLASH_FS_NOT_ENABLED();
    return FLASH_FS_DISABLED_ERR;
#endif
}

/**
 * @brief Wake flash chip from deep power-down mode.
 */
flash_err_t flash_wake(void) {
#if FEATURE_FLASH_FS
    return mt25ql_exit_deep_sleep();
#else
    LOG_FLASH__FLASH_FS_NOT_ENABLED();
    return FLASH_FS_DISABLED_ERR;
#endif
}
