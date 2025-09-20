/**
 * @file boot_lfs.c
 * @brief Minimal wrapper around LFS for use in bootloader
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "boot_lfs.h"

// Boot
#include "boot_mt25ql.h"
#include "boot_error.h"

// OBC
#include "obc_flashdefs.h"

// Lib
#include "lfs.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/* Size of read/program operations handed to our flash driver by LFS */
#define READ_PROG_SIZE  64

/* Size of the static cache used by LFS */
#define CACHE_SIZE      64

/* Number of erase cycles performed before LFS moves metadata pairs for wear leveling

   From lfs.c:
    block_cycles is the number of erase cycles before littlefs evicts
    metadata logs as a part of wear leveling. Suggested values are in the
    range of 100-1000, or set block_cycles to -1 to disable block-level
    wear-leveling.
*/
#define BLOCK_CYCLES    500

/* Size of the lookahead buffer used by LFS */
#define LOOKAHEAD_SIZE  256

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static int32_t boot_bd_read(const struct lfs_config *cnfg, lfs_block_t block, lfs_off_t off_bytes, void *buffer, lfs_size_t size_bytes);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/
static lfs_t boot_lfs;

/*
 * NOTE: Even though littlefs is compiled with LFS_READONLY, it still expects to be given
 * a prog_buffer and prog_size, otherwise it tries to malloc it's own prog_buffer and fails
 */

static uint8_t lfs_read_buffer[CACHE_SIZE];
static uint8_t lfs_prog_buffer[0];
static uint8_t lfs_lookahead_buffer[LOOKAHEAD_SIZE];
static uint8_t lfs_file_buffer[CACHE_SIZE];

static const struct lfs_config cfg = {
    .context = NULL,

    // boot block device operations
    .read = &boot_bd_read,

    // block device configuration
    .read_size = READ_PROG_SIZE,
    .prog_size = READ_PROG_SIZE,
    .block_size = EXT_FLASH_BLOCK_SIZE,
    .block_count = EXT_FLASH_BLOCK_COUNT,
    .cache_size = CACHE_SIZE,
    .lookahead_size = LOOKAHEAD_SIZE,
    .block_cycles = BLOCK_CYCLES,
    .read_buffer = lfs_read_buffer,
    .prog_buffer = lfs_prog_buffer,
    .lookahead_buffer = lfs_lookahead_buffer,
};

static const struct lfs_file_config boot_file_cfg = {
    .buffer = lfs_file_buffer
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/*
 * @brief Public API to open a file
 *
 * @param[in]  file             File handle address
 * @param[in]  filename         String
 * @param[in]  mutex_timeout    Timeout waiting for mutex access
 *
 * @return BOOT_SUCCESS if successful
 */
boot_err_t boot_fs_open(lfs_file_t *file, const char *filename) {
    if (lfs_file_opencfg(&boot_lfs, file, filename, LFS_O_RDONLY, &boot_file_cfg) != 0) {
        return BOOT_LFS_OPEN_ERR;
    }

    return BOOT_SUCCESS;
}

/*
 * @brief Public API to close a file
 *
 * @param[in]  file             File handle address
 *
 * @return BOOT_SUCCESS if successful
 */
boot_err_t boot_fs_close(lfs_file_t *file) {
    if (lfs_file_close(&boot_lfs, file) != 0) {
        return BOOT_LFS_OPEN_ERR;
    }

    return BOOT_SUCCESS;
}

/*
 * @brief Public API to read from a file in flash
 *
 * Data is read relative to start of the file (since fs_write() always rewinds file cursor)
 * Use fs_seek() to adjust start location before reading if required
 *
 * @param[in]  file             File handle
 * @param[in]  buf              Pointer to where data should be stored
 * @param[in]  size             Size of the data to read
 *
 * @return BOOT_SUCCESS if successful
 */
boot_err_t boot_fs_read(lfs_file_t *file, uint8_t *buf, size_t size) {

    lfs_ssize_t bytes_read = lfs_file_read(&boot_lfs, file, buf, size);

    if (bytes_read != size) {
        lfs_file_close(&boot_lfs, file);
        return BOOT_LFS_READ_ERR;
    }

    return BOOT_SUCCESS;
}

/*
 * @brief Public API to seek to a position in a file
 *
 * Use fs_seek(0, LFS_SEEK_{START,END}, filename, timeout) to seek to start/end of file
 *
 * @param[in]  file             File handle
 * @param[in]  offset           Offset position (signed value)
 * @param[in]  whence           Relative position to seek from (see lfs_whence_flags)
 *
 * @return BOOT_SUCCESS if successful
 */
boot_err_t boot_fs_seek(lfs_file_t *file, int32_t offset, boot_lfs_whence_flags whence) {

    lfs_soff_t seeker = lfs_file_seek(&boot_lfs, file, (lfs_soff_t) offset, whence);

    if (seeker < 0) {
        lfs_file_close(&boot_lfs, file);
        return BOOT_LFS_READ_ERR;
    }

    return BOOT_SUCCESS;
}

/*
 * @brief Public API to find size of a file
 *
 * @param[in]  file             File handle
 *
 * @return Returns size of file, or a negative error code on failure.
 */
int32_t boot_fs_size(lfs_file_t *file) {
    return (int32_t) lfs_file_size(&boot_lfs, file);
}

/**
 * @brief Initializes the filesystem. Must be called before any other FS functions.
 */
boot_err_t boot_fs_init(void) {
    if (lfs_mount(&boot_lfs, &cfg) != 0) {
        return BOOT_LFS_MOUNT_ERR;
    }

    return BOOT_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Read from a particular block in the flash.
 *
 * @param cnfg: configuration structure used to initialize the filesystem
 * @param block: block number to read from (blocks are 4KiB size)
 * @param off_bytes: offset in bytes to read from
 * @param[out] buffer: data buffer to store read data
 * @param size_bytes: size in bytes of data to read
 * @return Negative error code if unsuccessful
 */
static int32_t boot_bd_read(const struct lfs_config *cnfg, lfs_block_t block, lfs_off_t off_bytes, void *buffer, lfs_size_t size_bytes) {
    if (size_bytes == 64) {
        return boot_mt25ql_read_64((block * cnfg->block_size) + off_bytes, (uint8_t *) buffer);
    } else {
        // Unhandled case: can only do 64 byte reads
        return -1;
    }
}
