/**
 * @file obc_filesystem.h
 * @brief OBC filesystem, handles all filesystem related activities
 */

#ifndef OBC_FILESYSTEM_H_
#define OBC_FILESYSTEM_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_error.h"

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    FS_SEEK_START   = 0,   // Seek relative to the start of the file
    FS_SEEK_CUR     = 1,   // Seek relative to the current file position
    FS_SEEK_END     = 2,   // Seek relative to the end of the file
} fs_whence_flags;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void filesystem_create_infra(void);
void filesystem_start_task(void);

fs_err_t fs_open(lfs_file_t *file, const char *filename, uint16_t mutex_timeout);
fs_err_t fs_close(lfs_file_t *file);

fs_err_t fs_write_enqueue(const uint8_t *buf, size_t size, const char *filename, uint16_t mutex_timeout);
fs_err_t fs_write(lfs_file_t *file, const uint8_t *buf, size_t size);
fs_err_t fs_read(lfs_file_t *file, uint8_t *buf, size_t size);

fs_err_t fs_force_flush(uint16_t mutex_timeout);

fs_err_t fs_seek(lfs_file_t *file, int32_t offset, fs_whence_flags whence);
fs_err_t fs_zero(lfs_file_t *file);
fs_err_t fs_truncate(lfs_file_t *file, int32_t size);
fs_err_t fs_remove(const char *filename, uint16_t mutex_timeout);
int32_t fs_tell(lfs_file_t *file);
int32_t fs_size(lfs_file_t *file);

fs_err_t fs_init(void);
fs_err_t fs_deinit(void);
fs_err_t fs_erase(void);

fs_err_t fs_self_test(void);

#endif /* OBC_FILESYSTEM_H_ */
