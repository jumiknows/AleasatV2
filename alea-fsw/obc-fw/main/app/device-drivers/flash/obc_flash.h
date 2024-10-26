/*
 * @file obc_flash.h
 * @brief Generic driver for OBC flash device.
 */

#ifndef OBC_FLASH_H_
#define OBC_FLASH_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_hardwaredefs.h"
#include "obc_error.h"

// FreeRTOS
#include "rtos.h"

// HAL
#include "sys_common.h"

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

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
} flash_erase_sz_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void flash_init(void);
void flash_identify(void);
flash_err_t flash_erase(uint32_t addr, flash_erase_sz_t erase_size);
flash_err_t flash_write(uint32_t addr, uint32_t size_bytes, const uint8_t *data);
flash_err_t flash_read(uint32_t addr, uint32_t size_bytes, uint8_t *data);

#endif // OBC_FLASH_H_
