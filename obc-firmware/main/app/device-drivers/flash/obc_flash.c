/**
 * @file obc_flash.c
 * @brief Generic driver for OBC flash device.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_flash.h"

// OBC
#include "logger.h"
#ifdef PLATFORM_LAUNCHPAD_1224
#include "flash_mock.h"
#endif
#ifdef PLATFORM_ALEA_V1
#include "flash_mt25ql.h"
#endif

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
#ifdef PLATFORM_LAUNCHPAD_1224
    // TODO ALEA-855 Enable mock filesystem
    // flash_init_mock();
#endif
#ifdef PLATFORM_ALEA_V1
    mt25ql_init();
#endif
}

/**
 * @brief Prints the identity of the flash
 * 
 */
void flash_identify(void) {
#ifdef PLATFORM_LAUNCHPAD_1224
    log_str(ERROR, LOG_FLASH, "Flash identification not supported on Launchpad.");
#endif
#ifdef PLATFORM_ALEA_V1
    uint8_t device_data[20] = { 0x00 };
    mt25ql_read_id(device_data);

    uint8_t manufacturer = device_data[0];

    if (manufacturer == MANUFACTURER_ID_JEDEC_MICRON_TECHNOLOGY) {
        uint8_t mem_type = device_data[1];
        uint8_t mem_capacity = device_data[2];

        /* Print manufacturer, mem type, and mem capacity */
        log_str(INFO, LOG_FLASH, "Flash identified: Micron Technology %02x%02x", mem_type, mem_capacity);
    }
    else {
        log_str(ERROR, LOG_FLASH, "Failed to identify flash manufacturer");
    }
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
#ifdef PLATFORM_LAUNCHPAD_1224
    // TODO ALEA-855 Enable mock filesystem
    return FLASH_MOCK_ERR;
    // return flash_erase_mock(addr, erase_size);
#endif
#ifdef PLATFORM_ALEA_V1
    return mt25ql_erase(addr, erase_size);
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
flash_err_t flash_write(uint32_t addr, uint32_t size_bytes, const uint8_t* data) {
#ifdef PLATFORM_LAUNCHPAD_1224
    // TODO ALEA-855 Enable mock filesystem
    return FLASH_MOCK_ERR;
    // return flash_write_mock(addr, size_bytes, data);
#endif
#ifdef PLATFORM_ALEA_V1
    if (size_bytes == 64) {
        return mt25ql_write_64(addr, data);
    } else {
        // Unhandled case: can only do 64 byte writes
        return FLASH_INVALID_SIZE_ERR;
    }
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
flash_err_t flash_read(uint32_t addr, uint32_t size_bytes, uint8_t* data) {
#ifdef PLATFORM_LAUNCHPAD_1224
    // TODO ALEA-855 Enable mock filesystem
    return FLASH_MOCK_ERR;
    // return flash_read_mock(addr, size_bytes, data);
#endif
#ifdef PLATFORM_ALEA_V1
    if (size_bytes == 64) {
        return mt25ql_read_64(addr, data);
    } else {
        // Unhandled case: can only do 64 byte reads
        return FLASH_INVALID_SIZE_ERR;
    }
#endif
}
