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
#include "obc_misra.h"
#include "obc_task_info.h"
#include "obc_watchdog.h"

// FreeRTOS
#include "rtos.h"

// Third-Party
OBC_MISRA_CHECK_OFF
#include "lfs.h"
OBC_MISRA_CHECK_ON

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

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/
static void vFileSystemLoggerTask(void* pvParameters);

static int32_t bd_read(const struct lfs_config* cnfg, lfs_block_t block, lfs_off_t off_bytes, void* buffer, lfs_size_t size_bytes);
static int32_t bd_prog(const struct lfs_config* cnfg, lfs_block_t block, lfs_off_t off_bytes, const void* buffer, lfs_size_t size_bytes);
static int32_t bd_erase(const struct lfs_config* cnfg, lfs_block_t block);
static int32_t bd_sync(const struct lfs_config* cnfg);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/
static lfs_t obc_lfs;

static uint8_t lfs_read_buffer[CACHE_SIZE];
static uint8_t lfs_prog_buffer[CACHE_SIZE];
static uint8_t lfs_lookahead_buffer[LOOKAHEAD_SIZE];

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

static TaskHandle_t xFileSystemLoggerTaskHandle;
static StaticTask_t fs_logger_buffer;
static StackType_t fs_logger_stack[FS_LOGGER_TASK_STACK_SIZE];

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the filesystem. Must be called before any other FS functions.
 * 
 * @return FS_OK if successful, error code otherwise
 */
fs_err_t fs_init(void) {
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
        if (err != FS_OK) {
            return err;
        }
    } else if (err != FS_OK) {
        return err; // Isn't corrupted, but is still throwing an error
    }

    // Start tasks
    xFileSystemLoggerTaskHandle = task_create_static(
        &vFileSystemLoggerTask,
        "fs_logger",
        FS_LOGGER_TASK_STACK_SIZE,
        NULL,
        FS_LOGGER_PRIORITY,
        fs_logger_stack,
        &fs_logger_buffer);

    return FS_OK;
}

/**
 * @brief De-initializes the filesystem.
 * 
 * @return FS_OK if successful, error code otherwise
 */
fs_err_t fs_deinit(void) {
    fs_err_t err = FS_OK;

    // Stop tasks
    if (xFileSystemLoggerTaskHandle != NULL) {
        vTaskDelete(xFileSystemLoggerTaskHandle);
        xFileSystemLoggerTaskHandle = NULL;
    }

    // Unmount FS
    err = (fs_err_t) lfs_unmount(&obc_lfs);

    return err;
}

/**
 * @brief Performs a comprehensive test of the file system capabilities
 * 
 * @warning Re-formats FS, erasing all old data
 * 
 * @return FS_OK if all tests passed, error code otherwise
 */
fs_err_t fs_self_test(void) {
    fs_err_t err = FS_OK;

    err = (fs_err_t) lfs_format(&obc_lfs, &cfg);
    if (err != FS_OK) {
        return err;
    }

    err = (fs_err_t) lfs_mount(&obc_lfs, &cfg);
    if (err != FS_OK) {
        return err;
    }

    lfs_file_t test_file = { 0 };
    err = (fs_err_t) lfs_file_open(&obc_lfs, &test_file, "test_file", LFS_O_RDWR | LFS_O_CREAT);
    if (err != FS_OK) {
        return err;
    }

    const char test_str[] = "This is a test string for the test file.";
    lfs_ssize_t bytes_wrote = lfs_file_write(&obc_lfs, &test_file, test_str, sizeof(test_str));
    if (bytes_wrote != sizeof(test_str)) {
        return FS_TEST_WRITE_FAILURE;
    }

    // Rewind back to beginning of file for reading
    err = (fs_err_t) lfs_file_rewind(&obc_lfs, &test_file);
    if (err != FS_OK) {
        return err;
    }

    char read_str[sizeof(test_str)] = { 0 };
    lfs_ssize_t bytes_read = lfs_file_read(&obc_lfs, &test_file, read_str, sizeof(test_str));
    if (bytes_read != sizeof(test_str)) {
        return FS_TEST_READ_FAILURE;
    }

    err = (fs_err_t) lfs_file_close(&obc_lfs, &test_file);
    if (err != FS_OK) {
        return err;
    }

    // Ensure written and read data matches
    if (memcmp(test_str, read_str, sizeof(test_str)) != 0) {
        return FS_TEST_STRINGS_UNEQUAL;
    }

    // Reformat before finishing
    err = (fs_err_t) lfs_format(&obc_lfs, &cfg);
    if (err != FS_OK) {
        return err;
    }

    // If we reach here, all tests passed!
    return FS_OK;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Task that handles all logger-based filesystem activity
 *
 */
static void vFileSystemLoggerTask(void* pvParameters) {
    //fs_request_t req     = {};
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    set_task_status(wd_task_id, task_asleep);
    vTaskSuspend(NULL); // Suspend itself

    while (1) {
#if 0 // TODO: enable as part of logger refactor (ALEA-774)
        // Wait for a request
        set_task_status(wd_task_id, task_asleep);
        xQueueReceive(xFSRequestQueueHandle, (void*)&req, portMAX_DELAY);
        xSemaphoreTake(xBlockEraseMutex, portMAX_DELAY);
        set_task_status(wd_task_id, task_alive);

        // Handle the incoming request
        switch (req.type) {
            case MKDIR:
                *(req.err) = mkdir(req.args.open_args.path);
                break;
            case CREATE:
                *(req.err) = create(req.args.open_args.path);
                break;
            case LS:
                /*
                 * AZ: Warning 12.2 is triggered here, it seems to be a consequence of passing in
                 * more than one argument using items from a struct. It seems unnecessary to break
                 * each argument out to local variables to avoid this warning.
                 */
                OBC_MISRA_CHECK_OFF
                *(req.err) = ls(req.args.ls_args.path, (char(*)[LFS_NAME_MAX])req.args.ls_args.ls_list);
                OBC_MISRA_CHECK_ON
                break;
            case READ:
                OBC_MISRA_CHECK_OFF
                *(req.err) = read(req.args.read_args.path, req.args.read_args.off_bytes, req.args.read_args.buf, req.args.read_args.size_bytes);
                OBC_MISRA_CHECK_ON
                break;
            case WRITE:
                OBC_MISRA_CHECK_OFF
                *(req.err) = write(req.args.write_args.path, req.args.write_args.off_bytes, req.args.write_args.buf, req.args.write_args.size_bytes, req.args.write_args.whence);
                OBC_MISRA_CHECK_ON
                break;
            case DELETE:
                *(req.err) = delete (req.args.open_args.path);
                break;
            case FILE_SIZE:
                OBC_MISRA_CHECK_OFF
                *(req.err) = file_size(req.args.fsize_args.path, req.args.fsize_args.size);
                OBC_MISRA_CHECK_ON
                break;
            default:
                log_str(ERROR, LOG_FS_GENERAL, true, "Unknown Request: %d", req.type);
                *(req.err) = FS_UNKNOWN_REQUEST_ERR;
                break;
        }

        xSemaphoreGive(xBlockEraseMutex);

        // Notify the calling task of the transaction completion, and pass the filesystem
        // error as the notification value
        xTaskNotify(req.task, 0x01, eSetBits);
#endif
    }
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
static int32_t bd_read(const struct lfs_config* cnfg, lfs_block_t block, lfs_off_t off_bytes, void* buffer, lfs_size_t size_bytes) {
    return (int32_t) flash_read((block * cnfg->block_size) + off_bytes, size_bytes, (uint8_t*) buffer);
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
static int32_t bd_prog(const struct lfs_config* cnfg, lfs_block_t block, lfs_off_t off_bytes, const void* buffer, lfs_size_t size_bytes) {
    return (int32_t) flash_write((block * cnfg->block_size) + off_bytes, size_bytes, (const uint8_t*) buffer);
}

/**
 * @brief Erase a particular block in the flash.
 *
 * @param cnfg: configuration structure used to initialize the filesystem
 * @param block: block number to erase (blocks are 4KiB size)
 * @return Negative error code if unsuccessful
 */
static int32_t bd_erase(const struct lfs_config* cnfg, lfs_block_t block) {
#ifdef PLATFORM_LAUNCHPAD_1224
    return (int32_t) flash_erase(block * cnfg->block_size, SECTOR_1K);
#else
    return (int32_t) flash_erase(block * cnfg->block_size, SECTOR_4K);
#endif
}

/**
 * @brief Sync the state of the underlying block device.
 *
 * We are currently not making use of any caching, so this function is a stub.
 *
 * @param cnfg: configuration structure used to initialize the filesystem
 * @return Always returns no error
 */
static int32_t bd_sync(const struct lfs_config* cnfg) {
    return 0;
}
