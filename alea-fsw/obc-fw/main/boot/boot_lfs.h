/**
 * @file boot_lfs.h
 * @brief Minimal wrapper around LFS for use in bootloader
 */

#ifndef BOOT_LFS_H_
#define BOOT_LFS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Boot
#include "boot_error.h"

// Lib
#include "lfs.h"

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    FS_SEEK_START   = 0,   // Seek relative to the start of the file
    FS_SEEK_CUR     = 1,   // Seek relative to the current file position
    FS_SEEK_END     = 2,   // Seek relative to the end of the file
} boot_lfs_whence_flags;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

boot_err_t boot_fs_open(lfs_file_t *file, const char *filename);
boot_err_t boot_fs_close(lfs_file_t *file);
boot_err_t boot_fs_read(lfs_file_t *file, uint8_t *buf, size_t size);

boot_err_t boot_fs_seek(lfs_file_t *file, int32_t offset, boot_lfs_whence_flags whence);
int32_t boot_fs_size(lfs_file_t *file);

boot_err_t boot_fs_init(void);


#endif /* BOOT_LFS_H_ */
