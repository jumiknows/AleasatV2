/**
 * @file boot_updater.c
 * @brief Logic for updating firmware slots
*/

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "boot_updater.h"

// Boot
#include "boot_mt25ql.h"
#include "boot_lfs.h"
#include "boot_error.h"
#include "boot_fapi.h"
#include "boot_mibspi.h"

// OBC
#include "fw_memmap.h"

// HAL
#include "mibspi.h"
#include "system.h"

// Standard library
#include <stddef.h>
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define FW_BUFFER_SIZE  32*1024U // 32KB

#define MIN(X, Y)       (((X) < (Y)) ? (X) : (Y))

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static lfs_file_t boot_lfs_file = { 0 };
static uint8_t fwimg_buffer[FW_BUFFER_SIZE];

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Puts the bootloader into a state ready to perform fw update
 *
 *  NOTE: This function is not fully implemented yet!
 *
 * (1) Initialize F021 Flash API
 * (2) Check validity of update parameters
 * (3) Initialize HAL mibspi drivers
 * (4) Check ID of external flash to check if we are able to communicate
 * (5) Initialize LFS filesystem on external flash
 *
 * @return boot_err_t
 */
boot_err_t boot_preupdate_checks(uint8_t slot_num) {

    // (1) Initialize internal flash
    boot_err_t err = boot_fapi_init(slot_num);

    if (err != BOOT_SUCCESS) {
        return BOOT_INT_FLASH_INIT_ERR;
    }

    // TODO: (2) Check that we are not trying to update the slot we're currently booted from

    // (3) Hardware initialization
    boot_mibspi_init();

    // (4) Check external flash
    err = boot_mt25ql_init();

    if (err != BOOT_SUCCESS) {
        return BOOT_EXT_FLASH_ERR;
    }

    // (5) Initialize littlefs
    err = boot_fs_init();

    if (err != BOOT_SUCCESS) {
        return BOOT_LFS_MOUNT_ERR;
    }

    return BOOT_SUCCESS;
}

/**
 * @brief Actually do the firmware update
 *
 * NOTE: This function is not fully implemented yet!
 *
 * (1) Erase appropriate sectors of the firmware slot we want to update
 * (2) Start reading from external flash and writing to appropriate sectors
 *
 * @return boot_err_t
 */
boot_err_t boot_update(uint8_t slot_num) {
    uint32_t start_addr = 0;
    uint32_t fw_slot_size;

    switch (slot_num) {
    case FLASH_SLOT_BOOT_A:
        start_addr = FW_MEMMAP_FLASH1_START_ADDR;
        fw_slot_size = FW_MEMMAP_FLASH1_SIZE;
        break;

    case FLASH_SLOT_BOOT_B:
        start_addr = FW_MEMMAP_FLASH2_START_ADDR;
        fw_slot_size = FW_MEMMAP_FLASH2_SIZE;
        break;

    case FLASH_SLOT_APP_CORE:
        start_addr = FW_MEMMAP_FLASH3_START_ADDR;
        fw_slot_size = FW_MEMMAP_FLASH3_SIZE;
        break;

    case FLASH_SLOT_APP_EXT_A:
        start_addr = FW_MEMMAP_FLASH4_START_ADDR;
        fw_slot_size = FW_MEMMAP_FLASH4_SIZE;
        break;

    case FLASH_SLOT_APP_EXT_B:
        start_addr = FW_MEMMAP_FLASH5_START_ADDR;
        fw_slot_size = FW_MEMMAP_FLASH5_SIZE;
        break;

    default:
        return BOOT_INVAL_ERR;
    }

    // (1) Erase existing firmware
    if (boot_fapi_erase_slot(slot_num) != BOOT_SUCCESS) {
        return BOOT_INT_FLASH_ERASE_ERR;
    }

    // (2) Read from external flash and write to internal flash
    boot_err_t err = boot_fs_open(&boot_lfs_file, "FWIMAGE");

    if (err != BOOT_SUCCESS) {
        return BOOT_LFS_OPEN_ERR;
    }

    uint32_t bytes_left = boot_fs_size(&boot_lfs_file);

    if (bytes_left > fw_slot_size) {
        return BOOT_INVAL_ERR;
    }

    //  NOTE: If the fw image is smaller than the slot size, we need to pad with zeros since leaving
    // the flash erased is an invalid ECC condition
    uint32_t num_zero_padding = fw_slot_size - bytes_left;

    while (bytes_left > 0) {
        uint32_t to_read = MIN(FW_BUFFER_SIZE, bytes_left);

        err = boot_fs_read(&boot_lfs_file, fwimg_buffer, to_read);

        if (err != BOOT_SUCCESS) {
            return BOOT_LFS_READ_ERR;
        }

        // Write to internal flash, start_addr is incremented by function
        err = boot_fapi_write(&start_addr, fwimg_buffer, to_read);

        if (err != BOOT_SUCCESS) {
            return BOOT_INT_FLASH_WRITE_ERR;
        }

        bytes_left -= to_read;
    }

    // Pad with zeros
    if (num_zero_padding > 0) {
        memset(fwimg_buffer, 0, num_zero_padding);  // Set first num_zero_padding bytes of fwimg_buffer to 0
        err = boot_fapi_write(&start_addr, fwimg_buffer, num_zero_padding);

        if (err != BOOT_SUCCESS) {
            return BOOT_INT_FLASH_WRITE_ERR;
        }
    }

    return BOOT_SUCCESS;
}

/**
 * @brief Verifies the success of an update
 *
 *  NOTE: This function is not implemented yet!
 *
 * (1) Deinitialize internal flash
 * (2) Validate CRC32 on new firmware image  NOTE: Check for ECC errors
 * (3) Write new burn number
 *
 * @return boot_err_t
 */
boot_err_t boot_postupdate_checks(uint8_t slot_num) {
    // (1) Deinitialize internal flash. ECC errors are fatal from now on
    boot_fapi_deinit();

    // Stub
    return BOOT_SUCCESS;
}
