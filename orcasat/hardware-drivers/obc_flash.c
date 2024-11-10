/**
 * @file obc_flash.c
 * @brief Generic driver for OBC flash device.
 * @author Andrada Zoltan
 * @date April 22, 2020
 */

#include "obc_flash.h"
#ifdef PLATFORM_ORCA_V1
#include "flash_mt25ql.h"
#endif
#ifdef PLATFORM_LAUNCHPAD_1224
#include "flash_mock.h"
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove once ALEA flash driver is working (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/56)
#include "flash_mock.h"
#endif

/**
 * @brief Initializes the flash.
 */
void flash_init(void) {
#ifdef PLATFORM_ORCA_V1
    mt25ql_init();
#endif
#ifdef PLATFORM_LAUNCHPAD_1224
    flash_init_mock();
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove once ALEA flash driver is working (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/56)
    flash_init_mock();
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
#ifdef PLATFORM_ORCA_V1
    return mt25ql_erase(addr, erase_size);
#endif
#ifdef PLATFORM_LAUNCHPAD_1224
    return flash_erase_mock(addr, erase_size);
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove once ALEA flash driver is working (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/56)
    return flash_erase_mock(addr, erase_size);
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
#ifdef PLATFORM_ORCA_V1
    return mt25ql_write(addr, size_bytes, data);
#endif
#ifdef PLATFORM_LAUNCHPAD_1224
    return flash_write_mock(addr, size_bytes, data);
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove once ALEA flash driver is working (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/56)
    return flash_write_mock(addr, size_bytes, data);
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
#ifdef PLATFORM_ORCA_V1
    return mt25ql_read(addr, size_bytes, data);
#endif
#ifdef PLATFORM_LAUNCHPAD_1224
    return flash_read_mock(addr, size_bytes, data);
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove once ALEA flash driver is working (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/56)
    return flash_read_mock(addr, size_bytes, data);
#endif
}
