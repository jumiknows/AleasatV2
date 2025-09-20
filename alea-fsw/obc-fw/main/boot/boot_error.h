/**
 * @file boot_error.h
 * @brief All possible boot errors
 */

#ifndef BOOT_ERROR_H_
#define BOOT_ERROR_H_

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Status codes for boot functions
 */
typedef enum {
    BOOT_SUCCESS                    = 0,
    BOOT_MIBSPI_ERR                 = 1,
    BOOT_EXT_FLASH_ERR              = 2,
    BOOT_LFS_MOUNT_ERR              = 3,
    BOOT_LFS_READ_ERR               = 4,
    BOOT_LFS_OPEN_ERR               = 5,
    BOOT_INT_FLASH_INIT_ERR         = 6,
    BOOT_INT_FLASH_WRITE_ERR        = 7,
    BOOT_INT_FLASH_ERASE_ERR        = 8,
    BOOT_UPDATE_ERR                 = 9,
    BOOT_INVAL_ERR                  = 10
} boot_err_t;

#endif // BOOT_ERROR_H_
