/**
 * @file obc_filesystem.c
 * @brief OBC filesystem, handles all filesystem related activities
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "obc_filesystem.h"

// OBC
#include "obc_error.h"
#include "obc_featuredefs.h"
#include "obc_flash.h"
#include "obc_rtos.h"
#include "obc_watchdog.h"
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// Third-Party
#include "lfs.h"

// Lib
#include <string.h>

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

/* Max file size: currently 8 MiB */
#define MAX_FILE_SIZE_BYTES 8*1024*1024

/* Max filename size in chars */
#define FILENAME_MAX_SIZE   20U

/* Size of the write cache buffer */
#define WRITE_CACHE_SIZE    1024

/* Size of the write operations queue */
#define WRITE_QUEUE_SIZE    15U

/* How often task checks for flush conditions */
#define FS_POLL_PERIOD_MS   2000U

/* Max number of seconds to wait for write_cache to meet threshold before triggering a flush anyway
 * Must be a multiple of FS_POLL_PERIOD_MS */
#define FS_MAX_FLUSH_WAIT   600000U

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef struct {
    size_t      size; // Size of write request in bytes
    uint16_t    offset; // Offset of data relative to write_cache->buf
    char        filename[FILENAME_MAX_SIZE];
    bool        flushed; // True if contents have been writted to flash
} fs_write_request_t;

typedef struct {
    uint8_t             buf[WRITE_CACHE_SIZE];
    fs_write_request_t  queue[WRITE_QUEUE_SIZE];
    uint8_t             *head; // Pointer to head of buf
    uint8_t             q_next_index; // Index of empty slot in queue
} fs_write_cache_t;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void filesystem_task(void *pvParameters);

// This guard is here to stop the compiler complaining about unused declarations
#if FEATURE_FLASH_FS
static fs_err_t fs_flush_all(void);

static int32_t bd_read(const struct lfs_config *cnfg, lfs_block_t block, lfs_off_t off_bytes, void *buffer, lfs_size_t size_bytes);
static int32_t bd_prog(const struct lfs_config *cnfg, lfs_block_t block, lfs_off_t off_bytes, const void *buffer, lfs_size_t size_bytes);
static int32_t bd_erase(const struct lfs_config *cnfg, lfs_block_t block);
static int32_t bd_sync(const struct lfs_config *cnfg);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static lfs_t obc_lfs;

static uint8_t lfs_read_buffer[CACHE_SIZE];
static uint8_t lfs_prog_buffer[CACHE_SIZE];
static uint8_t lfs_lookahead_buffer[LOOKAHEAD_SIZE];
static uint8_t lfs_file_buffer[CACHE_SIZE];

static const struct lfs_config cfg = {
    .context = NULL,

    // OBC block device operations
    .read = &bd_read,
    .prog = &bd_prog,
    .erase = &bd_erase,
    .sync = &bd_sync,

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

    // FS configuration
    .name_max = LFS_NAME_MAX,
    .file_max = MAX_FILE_SIZE_BYTES,
    .attr_max = LFS_ATTR_MAX,
    .metadata_max = EXT_FLASH_BLOCK_SIZE
};

static const struct lfs_file_config obc_file_cfg = {
    .buffer = lfs_file_buffer
};

// Double buffering system where if one buffer is full, we swap to the other one and trigger a flush
static fs_write_cache_t write_cache0 = {
    .head = write_cache0.buf,
    .q_next_index = 0,
};

static fs_write_cache_t write_cache1 = {
    .head = write_cache1.buf,
    .q_next_index = 0,
};

static bool flushing = false;

// The write_cache currently in use
static fs_write_cache_t *active_write_cache = &write_cache0;

/* Mutex is taken in fs_write(), fs_read() and when trying to flush */
static SemaphoreHandle_t xFileSystemMutex;

/* Mutex is taken in fs_write_enqueue() and fs_flush_all() */
static SemaphoreHandle_t xWriteQueueMutex;

static TaskHandle_t fs_task_handle = NULL;
#endif

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialise filesystem mutex and queue
*/
void filesystem_create_infra(void) {
#if FEATURE_FLASH_FS
    static StaticSemaphore_t xFileSystemMutexBuffer;
    static StaticSemaphore_t xWriteQueueMutexBuffer;
    xFileSystemMutex = xSemaphoreCreateMutexStatic(&xFileSystemMutexBuffer);
    xWriteQueueMutex = xSemaphoreCreateMutexStatic(&xWriteQueueMutexBuffer);
#endif
}

/**
 * @brief Starts the filesystem task.
*/
void filesystem_start_task(void) {
    obc_rtos_create_task(OBC_TASK_ID_FILESYSTEM, &filesystem_task, NULL, OBC_WATCHDOG_ACTION_ALLOW);
#if FEATURE_FLASH_FS
    fs_task_handle = xTaskGetHandle("FILESYSTEM");
#endif
}

/*
 * @brief Public API to open a file
 *
 * @param[in]  file             File handle address
 * @param[in]  filename         String
 * @param[in]  mutex_timeout    Timeout waiting for mutex access
 *
 * @return FS_OK if successful, error code otherwise (see obc_error.h)
 */
fs_err_t fs_open(lfs_file_t *file, const char *filename, uint16_t mutex_timeout) {
#if FEATURE_FLASH_FS

    if (xSemaphoreTake(xFileSystemMutex, pdMS_TO_TICKS(mutex_timeout)) == pdTRUE) {
        return (fs_err_t) lfs_file_opencfg(&obc_lfs, file, filename, LFS_O_RDWR | LFS_O_CREAT, &obc_file_cfg);
    }

    return FS_MUTEX_TIMEOUT;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/*
 * @brief Public API to close a file
 *
 * @param[in]  file             File handle address
 *
 * @return FS_OK if successful, error code otherwise (see obc_error.h)
 */
fs_err_t fs_close(lfs_file_t *file) {
#if FEATURE_FLASH_FS
    fs_err_t err = (fs_err_t) lfs_file_close(&obc_lfs, file);

    xSemaphoreGive(xFileSystemMutex);

    return err;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/*
 * @brief Public API to write to flash immediately
 *
 * Function blocks for however long it takes to commit data to flash
 *
 * @param[in]  file             File handle
 * @param[in]  buf              Pointer to the data
 * @param[in]  size             Size of the data
 *
 * @return FS_OK if successful, error code otherwise (see obc_error.h)
 */
fs_err_t fs_write(lfs_file_t *file, const uint8_t *buf, size_t size) {
#if FEATURE_FLASH_FS
    lfs_ssize_t bytes_written = lfs_file_write(&obc_lfs, file, buf, size);

    if (bytes_written != size) {
        fs_close(file);
        return (bytes_written < 0) ? (fs_err_t) bytes_written : FS_WRITE_FAILURE_ERR;
    }

    return FS_OK;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/*
 * @brief Public API to append to a file in flash by joining write queue
 *
 * Data is copied into active_write_cache, after which function returns and caller is free to deallocate data. The data will be flushed to flash
 * whenever the filesystem task chooses (up till FS_MAX_FLUSH_WAIT seconds). There are 2 internal write cache buffers. When one is full, the other
 * is used and a flush is triggered.
 *
 * @param[in]  buf              Pointer to the data
 * @param[in]  size             Size of the data
 * @param[in]  filename         Filename (string) to save to
 * @param[in]  mutex_timeout    Timeout to wait for mutex to access flash
 *
 * @return FS_OK if successful, error code otherwise (see obc_error.h)
 */
fs_err_t fs_write_enqueue(const uint8_t *buf, size_t size, const char *filename, uint16_t mutex_timeout) {
#if FEATURE_FLASH_FS

    if (size > WRITE_CACHE_SIZE) {
        return FS_WRITE_TOO_BIG_ERR;
    }

    TickType_t total_ticks = pdMS_TO_TICKS(mutex_timeout);
    TickType_t start_ticks = xTaskGetTickCount();

    if (xSemaphoreTake(xWriteQueueMutex, total_ticks) == pdTRUE) {

        bool flush_required = false;

        // Check if we need to swap buffers
        if (active_write_cache->head - active_write_cache->buf >= WRITE_CACHE_SIZE || active_write_cache->q_next_index >= WRITE_QUEUE_SIZE) {
            // Check that we are not in the middle of flushing the non-active cache
            while (flushing) {
                vTaskDelay(10); // Check every 100ms that the flushing is done

                if (xTaskGetTickCount() - start_ticks > total_ticks) {
                    return FS_WRITE_FAILURE_ERR;
                }
            }

            active_write_cache = (active_write_cache == &write_cache0) ? &write_cache1 : &write_cache0;

            // Check if both buffers are full
            if (active_write_cache->head - active_write_cache->buf >= WRITE_CACHE_SIZE || active_write_cache->q_next_index >= WRITE_QUEUE_SIZE) {
                return FS_ENQUEUE_ERR;
            }

            flush_required = true;
        }


        // Copy into cache
        memcpy(active_write_cache->head, buf, size);

        fs_write_request_t *write_request = &active_write_cache->queue[active_write_cache->q_next_index];

        strcpy(write_request->filename, filename);
        write_request->size = size;
        write_request->offset = active_write_cache->head - active_write_cache->buf;
        write_request->flushed = false;

        active_write_cache->q_next_index++;
        active_write_cache->head += size;

        xSemaphoreGive(xWriteQueueMutex);

        /*
         * We trigger a flush by notifying the filesystem task
         * This will cause a context switch if the current task is lower priority than the filesystem task
         * and may block the calling task. However this is unlikely since the priority of the filesystem task
         * is the same as many app level tasks. The xWriteQueueMutex is freed before this to allow other tasks
         * to call fs_write_enqueue while the calling task may be blocked
         */
        if (flush_required) {
            xTaskNotifyGive(fs_task_handle);
        }

        return FS_OK;
    }

    return FS_MUTEX_TIMEOUT;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
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
 * @return FS_OK if successful, error code otherwise (see obc_error.h)
 */
fs_err_t fs_read(lfs_file_t *file, uint8_t *buf, size_t size) {
#if FEATURE_FLASH_FS
    lfs_ssize_t bytes_read = lfs_file_read(&obc_lfs, file, buf, size);

    if (bytes_read != size) {
        fs_close(file);
        return FS_READ_FAILURE_ERR;
    }

    return FS_OK;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/*
 * @brief Public API to seek to a position in a file
 *
 * Use fs_seek(0, FS_SEEK_{START,END}, filename, timeout) to seek to start/end of file
 *
 * @param[in]  file             File handle
 * @param[in]  offset           Offset position (signed value)
 * @param[in]  whence           Relative position to seek from (see fs_whence_flags)
 *
 * @return FS_OK if successful, error code otherwise (see obc_error.h)
 */
fs_err_t fs_seek(lfs_file_t *file, int32_t offset, fs_whence_flags whence) {
#if FEATURE_FLASH_FS
    lfs_soff_t seeker = lfs_file_seek(&obc_lfs, file, (lfs_soff_t) offset, whence);

    if (seeker < 0) {
        fs_close(file);
        return (fs_err_t) seeker;    // Returns -ve code on failure
    }

    return FS_OK;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/*
 * @brief Public API to reset the file to have size 0
 *
 * @param[in]  file             File handle address
 *
 * @return FS_OK if successful, error code otherwise (see obc_error.h)
 */
fs_err_t fs_zero(lfs_file_t *file) {
#if FEATURE_FLASH_FS
    return (fs_err_t) lfs_file_truncate(&obc_lfs, file, 0);
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/*
 * @brief Public API to truncate size of the file to the specified size
 *
 * @param[in]  file             File handle address
 * @param[in]  size             Size to truncate to
 *
 * @return FS_OK if successful, error code otherwise (see obc_error.h)
 */
fs_err_t fs_truncate(lfs_file_t *file, int32_t size) {
#if FEATURE_FLASH_FS
    return (fs_err_t) lfs_file_truncate(&obc_lfs, file, size);
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/*
 * @brief Public API to remove a file
 *
 * @param[in]  filename         String
 * @param[in]  mutex_timeout    Timeout to wait for mutex to access flash
 *
 * @return FS_OK if successful, error code otherwise (see obc_error.h)
 */
fs_err_t fs_remove(const char *filename, uint16_t mutex_timeout) {
#if FEATURE_FLASH_FS

    if (xSemaphoreTake(xFileSystemMutex, pdMS_TO_TICKS(mutex_timeout)) == pdTRUE) {
        fs_err_t err  = (fs_err_t) lfs_remove(&obc_lfs, filename);

        xSemaphoreGive(xFileSystemMutex);
        return err;
    }

    return FS_MUTEX_TIMEOUT;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/*
 * @brief Public API to find current position in a file
 *
 * @param[in]  file             File handle
 *
 * @return Returns current position in file, or a negative error code on failure (see obc_error.h).
 */
int32_t fs_tell(lfs_file_t *file) {
#if FEATURE_FLASH_FS
    lfs_soff_t pos = lfs_file_tell(&obc_lfs, file);

    if (pos < 0) {
        fs_close(file);
        return (fs_err_t) pos;
    }

    return (int32_t) pos;
#else
    return (int32_t) FS_FLASH_DISABLED_ERR;
#endif
}

/*
 * @brief Public API to find size of a file
 *
 * @param[in]  file             File handle
 *
 * @return Returns size of file, or a negative error code on failure (see obc_error.h).
 */
int32_t fs_size(lfs_file_t *file) {
#if FEATURE_FLASH_FS
    return (int32_t) lfs_file_size(&obc_lfs, file);
#else
    return (int32_t) FS_FLASH_DISABLED_ERR;
#endif
}

/*
 * @brief Public API to force a flush of both write caches. Runs in the calling task, NOT the filesystem task
 *
 * @param[in]  mutex_timeout    Timeout to wait for mutex to access flash
 *
 * @return returns FS_OK on successful flush
*/
fs_err_t fs_force_flush(uint16_t mutex_timeout) {
#if FEATURE_FLASH_FS

    if (xSemaphoreTake(xFileSystemMutex, pdMS_TO_TICKS(mutex_timeout)) == pdTRUE) {
        if (xSemaphoreTake(xWriteQueueMutex, pdMS_TO_TICKS(mutex_timeout)) == pdTRUE) {

            if (fs_flush_all() != FS_OK) {
                xSemaphoreGive(xWriteQueueMutex);
                xSemaphoreGive(xFileSystemMutex);
                return FS_FLUSH_FAILED;
            }

            // Swap to other cache
            active_write_cache = (active_write_cache == &write_cache0) ? &write_cache1 : &write_cache0;

            if (fs_flush_all() != FS_OK) {
                xSemaphoreGive(xWriteQueueMutex);
                xSemaphoreGive(xFileSystemMutex);
                return FS_FLUSH_FAILED;
            }

            xSemaphoreGive(xWriteQueueMutex);
            xSemaphoreGive(xFileSystemMutex);
            return FS_OK;

        }
    }

    return FS_MUTEX_TIMEOUT;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/******************************************************************************/
/* NOTE: Although the following functions are public, they should only be     *
 *       called during the filesystem init/deinit phases and are not guarded  *
 *       with a mutex.                                                        */
/******************************************************************************/

/**
 * @brief Erases the filesystem
 *
 * @return FS_OK if successful, error code otherwise
 */
fs_err_t fs_erase(void) {
#if FEATURE_FLASH_FS
    fs_err_t err = FS_OK;

    err = fs_deinit();

    if (err != FS_OK) {
        return err;
    }

    err = (fs_err_t) lfs_format(&obc_lfs, &cfg);

    if (err != FS_OK) {
        return err;
    }

    err = (fs_err_t) lfs_mount(&obc_lfs, &cfg);

    return err;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/**
 * @brief Initializes the filesystem. Must be called before any other FS functions.
 *
 * @return FS_OK if successful, error code otherwise
 */
fs_err_t fs_init(void) {
#if FEATURE_FLASH_FS
    fs_err_t err = FS_OK;

    // Mount fs
    err = (fs_err_t) lfs_mount(&obc_lfs, &cfg);

    if (err == FS_CORRUPT_ERR) {
        // FS is either yet to be initialized, or irrepairably corrupted
        err = (fs_err_t) lfs_format(&obc_lfs, &cfg);

        if (err != FS_OK) {
            return err;
        }

        // Try again with formatted FS
        err = (fs_err_t) lfs_mount(&obc_lfs, &cfg);
    }

    return err;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/**
 * @brief De-initializes the filesystem.
 *
 * @return FS_OK if successful, error code otherwise
 */
fs_err_t fs_deinit(void) {
#if FEATURE_FLASH_FS
    return (fs_err_t) lfs_unmount(&obc_lfs);
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/**
 * @brief Performs a comprehensive test of the file system capabilities
 *
 * @warning Re-formats FS, erasing all old data
 *
 * @return FS_OK if all tests passed, error code otherwise
 */
fs_err_t fs_self_test(void) {
#if FEATURE_FLASH_FS
    fs_err_t err = FS_OK;

    err = (fs_err_t) lfs_format(&obc_lfs, &cfg);

    if (err != FS_OK) {
        return err;
    }

    err = (fs_err_t) lfs_mount(&obc_lfs, &cfg);

    if (err != FS_OK) {
        return err;
    }

    lfs_file_t file = { 0 };
    err = fs_open(&file, "test_file", 0);

    if (err != FS_OK) {
        return err;
    }

    const uint8_t test_buf[13] =  { 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    const uint8_t test_buf1[13] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    const uint8_t test_buf2[7] =  { 'H', 'e', 'l', 'l', 'o', ',', ' ' };

    // Test write last half
    err = fs_write(&file, test_buf1, sizeof(test_buf1));

    if (err != FS_OK) {
        return err;
    }

    // Rewind to start
    err = fs_seek(&file, 0, FS_SEEK_START);

    if (err != FS_OK) {
        return err;
    }

    // Test write first half
    err = fs_write(&file, test_buf2, sizeof(test_buf2));

    if (err != FS_OK) {
        return err;
    }

    // Test fs_tell()
    int32_t pos = fs_tell(&file);

    if (pos != sizeof(test_buf2)) {
        return FS_TEST_SEEK_ERR;
    }

    // Test file size
    int32_t size = fs_size(&file);

    if (size != sizeof(test_buf)) {
        return FS_TEST_SIZE_ERR;
    }

    // Rewind to start
    err = fs_seek(&file, 0, FS_SEEK_START);

    if (err != FS_OK) {
        return err;
    }

    uint8_t read_buf[sizeof(test_buf)] = { 0 };
    err = fs_read(&file, read_buf, sizeof(test_buf));

    if (err != FS_OK) {
        return err;
    }

    // Ensure written and read data matches
    if (memcmp(test_buf, read_buf, sizeof(test_buf)) != 0) {
        return FS_TEST_STRINGS_UNEQUAL_ERR;
    }

    err = fs_zero(&file);

    if (err != FS_OK) {
        return err;
    }

    size = fs_size(&file);

    if (size != 0) {
        return FS_TEST_SIZE_ERR;
    }

    err = fs_close(&file);

    if (err != FS_OK) {
        return err;
    }

    err = fs_remove("test_file", 0);

    if (err != FS_OK) {
        return err;
    }

    // Reformat before finishing
    err = (fs_err_t) lfs_format(&obc_lfs, &cfg);

    return err;
#else
    return FS_FLASH_DISABLED_ERR;
#endif
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Filesystem task
*/
static void filesystem_task(void *pvParameters) {

    TickType_t fs_poll_period_ticks = pdMS_TO_TICKS(FS_POLL_PERIOD_MS);
    TickType_t last_wake_time = xTaskGetTickCount();

#if FEATURE_FLASH_FS
    uint16_t elapsed_period = 0;
#endif

    while (1) {
        obc_watchdog_pet(OBC_TASK_ID_FILESYSTEM);

#if FEATURE_FLASH_FS
        /*
         * The two conditions that can trigger a flush of write_cache into flash are
         * (1) It's been FS_MAX_FLUSH_WAIT seconds since the last flush
         * (2) The task has a notification pending
         */

        if (elapsed_period >= FS_MAX_FLUSH_WAIT || ulTaskNotifyTake(pdFALSE, 0) == pdTRUE) {
            if (xSemaphoreTake(xFileSystemMutex, 0) == pdTRUE) {
                flushing = true;

                if (fs_flush_all() != FS_OK) {
                    // Notify self to attempt to flush again the next time the task wakes up
                    xTaskNotifyGive(fs_task_handle);
                }

                flushing = false;
                xSemaphoreGive(xFileSystemMutex);
            } else {
                // Notify self to attempt to flush again the next time the task wakes up
                xTaskNotifyGive(fs_task_handle);
            }

            elapsed_period = 0;
        }

        elapsed_period += FS_POLL_PERIOD_MS;
        vTaskDelayUntil(&last_wake_time, fs_poll_period_ticks);
#else
        vTaskDelayUntil(&last_wake_time, fs_poll_period_ticks);
#endif
    }
}

#if FEATURE_FLASH_FS
/**
 * @brief Flushes all items in write_queue to flash
*/
static fs_err_t fs_flush_all(void) {
    // Choose the non-active write cache
    fs_write_cache_t *write_cache = (active_write_cache == &write_cache0) ? &write_cache1 : &write_cache0;
    uint8_t *write_cache_start = write_cache->buf;

    // Return immediately if theres' nothing to write
    if (write_cache->q_next_index == 0) {
        return FS_OK;
    }

    /*
     * Opening and closing the file is by a large margin the most costly operation
     * Thus, we try and batch writes to the same file by searching through all the
     * write requests for the same file.
     */

    for (uint8_t i = 0; i < write_cache->q_next_index; i++) {

        fs_write_request_t *outer_req = &write_cache->queue[i];

        if (outer_req->flushed) {
            continue; // Write request already flushed, continue to next one
        }

        lfs_file_t file = { 0 };
        fs_err_t err = (fs_err_t) lfs_file_opencfg(&obc_lfs, &file, outer_req->filename, LFS_O_RDWR | LFS_O_CREAT | LFS_O_APPEND, &obc_file_cfg);

        if (err != FS_OK) { // TODO: ALEA-3038 what to do with repeated filesystem errors?
            return err;
        }

        lfs_ssize_t bytes_written = lfs_file_write(&obc_lfs, &file, write_cache_start + outer_req->offset, outer_req->size);

        if (bytes_written != outer_req->size) { // TODO: ALEA-3038 what to do with repeated filesystem errors?
            lfs_file_close(&obc_lfs, &file);
            return (bytes_written < 0) ? (fs_err_t) bytes_written : FS_WRITE_FAILURE_ERR;
        }

        // Search for all other write requests that have the same filename
        for (uint8_t j = i + 1; j < write_cache->q_next_index; j++) {

            fs_write_request_t *inner_req = &write_cache->queue[j];

            if (inner_req->flushed) {
                continue;
            }

            if (strcmp(inner_req->filename, outer_req->filename) == 0) {
                bytes_written = lfs_file_write(&obc_lfs, &file, write_cache_start + inner_req->offset, inner_req->size);

                if (bytes_written != inner_req->size) { // TODO: ALEA-3038 what to do with repeated filesystem errors?
                    lfs_file_close(&obc_lfs, &file);
                    return (bytes_written < 0) ? (fs_err_t) bytes_written : FS_WRITE_FAILURE_ERR;
                }

                inner_req->flushed = true;
            }
        }

        outer_req->flushed = true;

        err = (fs_err_t) lfs_file_close(&obc_lfs, &file);

        if (err != FS_OK) {
            return err;
        }
    }

    // Reset cache to empty state
    write_cache->q_next_index = 0;
    write_cache->head = write_cache->buf;

    return FS_OK;
}

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
static int32_t bd_read(const struct lfs_config *cnfg, lfs_block_t block, lfs_off_t off_bytes, void *buffer, lfs_size_t size_bytes) {
    return (int32_t) flash_read((block * cnfg->block_size) + off_bytes, size_bytes, (uint8_t *) buffer);
}

/**
 * @brief Program a particular block in the flash.
 *
 * @param cnfg: configuration structure used to initialize the filesystem
 * @param block: block number to write to (blocks are 4KiB size)
 * @param off_bytes: offset in bytes to write to
 * @param buffer: data buffer to write
 * @param size_bytes: size in bytes of data to write
 * @return Negative error code if unsuccessful
 */
static int32_t bd_prog(const struct lfs_config *cnfg, lfs_block_t block, lfs_off_t off_bytes, const void *buffer, lfs_size_t size_bytes) {
    return (int32_t) flash_write((block * cnfg->block_size) + off_bytes, size_bytes, (const uint8_t *) buffer);
}

/**
 * @brief Erase a particular block in the flash.
 *
 * @param cnfg: configuration structure used to initialize the filesystem
 * @param block: block number to erase (blocks are 4KiB size)
 * @return Negative error code if unsuccessful
 */
static int32_t bd_erase(const struct lfs_config *cnfg, lfs_block_t block) {
    return (int32_t) flash_erase(block * cnfg->block_size, SECTOR_4K);
}

/**
 * @brief Sync the state of the underlying block device.
 *
 * We are currently not making use of any caching, so this function is a stub.
 *
 * @param cnfg: configuration structure used to initialize the filesystem
 * @return Always returns no error
 */
static int32_t bd_sync(const struct lfs_config *cnfg) {
    return 0;
}
#endif
