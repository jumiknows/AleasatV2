/*
 * @file obc_flash.h
 * @brief Generic driver for OBC flash device.
 * @author Andrada Zoltan
 * @date April 22, 2020
 */

#ifndef OBC_FLASH_H_
#define OBC_FLASH_H_

#include "obc_hardwaredefs.h"
#include "obc_error.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"
#include "sys_common.h"

/**
 * @brief Erasable sizes for the flash.
 */
typedef enum flash_erase_sz {
    /**
     * @brief Erases the full flash device.
     */
    FULL_CHIP = 0,

    /**
     * @brief Erases a 64KB sector.
     */
    SECTOR_64K = 1,

    /**
     * @brief Erases a 32KB sector.
     */
    SECTOR_32K = 2,

    /**
     * @brief Erases a 4KB sector.
     */
    SECTOR_4K = 3,

#ifdef PLATFORM_LAUNCHPAD_1224
    SECTOR_1K = 4,
#endif
#ifdef PLATFORM_ALEA_V1 // TODO remove once flash driver is supported (https://gitlab.com/alea-2020/command-data-handling/obc2-firmware/-/issues/56)
    SECTOR_1K = 4,
#endif
} flash_erase_sz_t;

void flash_init(void);
flash_err_t flash_erase(uint32_t addr, flash_erase_sz_t erase_size);
flash_err_t flash_write(uint32_t addr, uint32_t size_bytes, const uint8_t* data);
flash_err_t flash_read(uint32_t addr, uint32_t size_bytes, uint8_t* data);

#endif /* OBC_FLASH_H_ */
