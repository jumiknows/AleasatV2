/**
 * @file filesystem.c
 * @brief Filesystem abstraction library
 *
 * @author Andrada Zoltan
 * @date Nov 4, 2019
 */

#include "filesystem.h"
#include "obc_flash.h"
#include "rtos.h"
#include <string.h>
#include "obc_rtc.h"
#include "obc_time.h"
#include "logger.h"
#include "obc_utils.h"
#include "obc_watchdog.h"
#include "obc_rtos.h"
#include "obc_task_info.h"
#include "obc_misra.h"
#include "obc_featuredefs.h"

OBC_MISRA_CHECK_OFF
#include "lfs.h"
OBC_MISRA_CHECK_ON

/**
 * @brief Minimum number of cfg.block_size-sized blocks remaining in the
 * filesystem that will trigger deletion of the oldest set of files
 * since the filesystem is almost full.
 *
 * TODO: determine what a reasonable value for this is
 * TODO: this value should probably live in the NVCT
 */
#define BLOCK_REMAIN_THRESHOLD 10

/*
 * @brief Maximum number of filesystem requests that can live in the
 * filesystem request queue.
 *
 * TODO: this value should probably live in the NVCT
 */
#define FS_QUEUE 10

/**
 * @brief Period in milliseconds of execution of the filesystem lifecycle
 * task, defined time represents 1 day.
 */
#define FS_LIFECYCLE_PERIOD_MS (24 * 60 * 60 * 1000)

/**
 * @brief Period in milliseconds of the block erase task.
 */
#define BLOCK_ERASE_PERIOD_MS 1000

/* Extern Variable Definitions -------------------------------------------------------*/

/**
 * @brief The file names present in any directory.
 *
 * The file names must contain at most @ref (FILE_NAME_SIZE - 1)
 * visible alphanumeric characters, because FILE_NAME_SIZE includes
 * the null terminator.
 */
const char* files[MAX_FILES] = {"sys_log", "err_log", "obc_fast", "obc_slow", "eps_slow"};

char curr_dir[DIR_NAME_SIZE] = "";

TaskHandle_t xBlockEraseTaskHandle          = NULL;
TaskHandle_t xFileSystemMasterTaskHandle    = NULL;
TaskHandle_t xFileSystemLifecycleTaskHandle = NULL;

bool fs_initialized = false;

/* Private Function Prototypes -------------------------------------------------------*/
static void vFileSystemMasterTask(void* pvParameters);
static void vFileSystemLifecycleTask(void* pvParameters);
static fs_err_t delete_oldest_files(void);
static fs_err_t update_dir_list(void);
static fs_err_t mkdir(const char* path);
static fs_err_t create(const char* path);
static fs_err_t ls(const char* dir_path, char ls_list[][LFS_NAME_MAX]);
static fs_err_t read(const char* path, uint32_t off_bytes, uint8_t* buffer, uint32_t size_bytes);
static fs_err_t write(const char* path, uint32_t off_bytes, const uint8_t* buffer, uint32_t size_bytes, int32_t whence);
static fs_err_t delete (const char* path);
static fs_err_t file_size(const char* path, uint32_t* size);
static int32_t bd_read(const struct lfs_config* cnfg, lfs_block_t block, lfs_off_t off_bytes, void* buffer, lfs_size_t size_bytes);
static int32_t bd_prog(const struct lfs_config* cnfg, lfs_block_t block, lfs_off_t off_bytes, const void* buffer, lfs_size_t size_bytes);
static int32_t bd_erase(const struct lfs_config* cnfg, lfs_block_t block);
static int32_t bd_sync(const struct lfs_config* cnfg);
static void rtc_get_date_to_string(char buf[DIR_NAME_SIZE]);
static void vBlockEraseTask(void* pvParameters);

enum fs_request_type { MKDIR, CREATE, LS, READ, WRITE, DELETE, FILE_SIZE };

typedef struct fs_request {
    TaskHandle_t task;
    enum fs_request_type type;
    fs_err_t* err;

    union {
        struct {
            const char* path;
        } open_args;

        struct {
            const char* path;
            void* ls_list;
        } ls_args;

        struct {
            const char* path;
            uint32_t off_bytes;
            uint8_t* buf;
            uint32_t size_bytes;
        } read_args;

        struct {
            const char* path;
            uint32_t off_bytes;
            const uint8_t* buf;
            uint32_t size_bytes;
            uint32_t whence;
        } write_args;

        struct {
            const char* path;
            uint32_t* size;
        } fsize_args;
    } args;
} fs_request_t;

/* Private Variable Definitions ------------------------------------------------------*/
static lfs_t m_lfs = {};

const struct lfs_config cfg = {.context = NULL,

                               .read  = &bd_read,
                               .prog  = &bd_prog,
                               .erase = &bd_erase,
                               .sync  = &bd_sync,

                               .read_size      = 16,
                               .prog_size      = 16,
                               .block_size     = BLOCK_SIZE,
                               .block_count    = BLOCK_COUNT,
                               .block_cycles   = 500,
                               .cache_size     = 256,
                               .lookahead_size = 64,

                               .read_buffer      = NULL,
                               .prog_buffer      = NULL,
                               .lookahead_buffer = NULL,

                               .name_max = LFS_NAME_MAX,
                               .file_max = LFS_FILE_MAX,
                               .attr_max = LFS_ATTR_MAX};

/**
 * @brief List of directory paths that are currently stored in the
 * filesystem.
 */
static char dir_list[MAX_DIRS][DIR_NAME_SIZE];

// Stores index of the next location in the dir_list to be filled
static uint8_t next_dir;

// Filesystem Request Queue
QueueHandle_t xFSRequestQueueHandle;
StaticQueue_t xFSRequestStaticQueue;
fs_request_t fs_request_queue_buffer[FS_QUEUE];

// Block Erase Mutex
SemaphoreHandle_t xBlockEraseMutex;
StaticSemaphore_t xBlockEraseStaticMutex;

// Task Stacks
StaticTask_t block_erase_buffer;
StackType_t block_erase_stack[BLOCK_ERASE_TASK_STACK_SIZE];
StaticTask_t fs_lifecycle_buffer;
StackType_t fs_lifecycle_stack[FS_LIFECYCLE_TASK_STACK_SIZE];
StaticTask_t fs_master_buffer;
StackType_t fs_master_stack[FS_MASTER_TASK_STACK_SIZE];

/* User Functions --------------------------------------------------------------------*/

/**
 * @brief Creates a file path from a file name. The file path is suitable
 * for passing to the filesystem API.
 *
 * The output buffer is filled with: "curr_dir/file_name" if the file_name is "file_name".
 *
 * @param file_name The name of the file that will be used in the path.
 * @param output_path The buffer that will get the assembled path. Should be @ref LFS_NAME_MAX bytes
 * in size.
 */
void make_file_path(const char* file_name, char* output_path) {
    strncat(output_path, curr_dir, DIR_NAME_SIZE);
    strcat(output_path, "/");
    strncat(output_path, file_name, FILE_NAME_SIZE);
}

/**
 * @brief Mounts the filesystem.
 *
 * @returns Error code if unsuccessful
 */
fs_err_t fs_init(void) {
    // Create the filesystem request queue
    xFSRequestQueueHandle = xQueueCreateStatic(FS_QUEUE, sizeof(fs_request_t), (uint8_t*)fs_request_queue_buffer, &xFSRequestStaticQueue);

    // Create the block erase mutex
    xBlockEraseMutex = xSemaphoreCreateMutexStatic(&xBlockEraseStaticMutex);

    int32_t err = lfs_mount(&m_lfs, &cfg);

    // This will occur on the first mount of the filesystem
    if (err) {
        err = lfs_format(&m_lfs, &cfg);
        if (err) {
            log_str(ERROR, FS_LOG, true, "FS Error: %d", err);
            return (fs_err_t)err;
        }

        err = lfs_mount(&m_lfs, &cfg);
        if (err) {
            log_str(ERROR, FS_LOG, true, "FS Error: %d", err);
            return (fs_err_t)err;
        }
    }

    // Initialize directory list
    err = update_dir_list();
    if (err) {
        log_str(ERROR, FS_LOG, true, "FS Error: %d", err);
        return (fs_err_t)err;
    }

    // Filesystem has been properly initialized. This flag must
    // be set before starting tasks that use the filesystem.
    fs_initialized = true;

// Create the filesystem tasks, the master task must be created in privileged mode so that it can
// access the flash API in @ref flash_mock.c
#ifdef PLATFORM_LAUNCHPAD_1224
    xFileSystemMasterTaskHandle = task_create_static(&vFileSystemMasterTask, "fs_master", FS_MASTER_TASK_STACK_SIZE, NULL, portPRIVILEGE_BIT | FS_MASTER_PRIORITY, fs_master_stack, &fs_master_buffer);
    xBlockEraseTaskHandle       = task_create_periodic_static(&vBlockEraseTask, "block_erase", BLOCK_ERASE_TASK_STACK_SIZE, NULL, portPRIVILEGE_BIT | BLOCK_ERASE_PRIORITY, block_erase_stack,
                                                        &block_erase_buffer, BLOCK_ERASE_PERIOD_MS);
#else
    xFileSystemMasterTaskHandle = task_create_static(&vFileSystemMasterTask, "fs_master", FS_MASTER_TASK_STACK_SIZE, NULL, FS_MASTER_PRIORITY, fs_master_stack, &fs_master_buffer);
    xBlockEraseTaskHandle =
        task_create_periodic_static(&vBlockEraseTask, "block_erase", BLOCK_ERASE_TASK_STACK_SIZE, NULL, BLOCK_ERASE_PRIORITY, block_erase_stack, &block_erase_buffer, BLOCK_ERASE_PERIOD_MS);
#endif
    xFileSystemLifecycleTaskHandle = task_create_periodic_static(&vFileSystemLifecycleTask, "fs_lifecycle", FS_LIFECYCLE_TASK_STACK_SIZE, NULL, FS_LIFECYCLE_PRIORITY, fs_lifecycle_stack,
                                                                 &fs_lifecycle_buffer, FS_LIFECYCLE_PERIOD_MS);

    return FS_OK;
}

/**
 * @brief Unmounts the filesystem.
 *
 * @returns Error code if unsuccessful
 */
fs_err_t fs_deinit(void) {
    fs_initialized = false;
    return (fs_err_t)lfs_unmount(&m_lfs);
}

/**
 * @brief Creates a request to make a new directory
 *
 * @param: absolute path to the directory
 * @returns Negative error code if unsuccessful
 */
fs_err_t fs_mkdir(const char* path) {
    fs_err_t err = FS_OK;

    if (fs_initialized) {
        // Get task handle and watchdog id
        TaskHandle_t task    = xTaskGetCurrentTaskHandle();
        task_id_t wd_task_id = 0;
        if (!get_task_id_from_handle(task, &wd_task_id)) {
            return FS_INVAL_TASK_ERR;
        }

        // Make request
        fs_request_t req = {.task = task, .type = MKDIR, .args.open_args = {.path = path}, .err = &err};

        xQueueSend(xFSRequestQueueHandle, (void*)&req, portMAX_DELAY);

        // Wait for request to be handled, the filesystem notification bit is the
        // first bit in the task notification value
        uint32_t notif_value = 0;
        set_task_status(wd_task_id, task_asleep);
        while ((notif_value & 0x1) == 0) {
            xTaskNotifyWait(0x1,          /* Clears the filesystem notification bit upon calling this function */
                            0x0,          /* Doesn't clear the notification value upon exiting this function */
                            &notif_value, /* Gets notification value */
                            portMAX_DELAY);
        }
        set_task_status(wd_task_id, task_alive);
    } else {
        err = FS_NOT_INITIALIZED;
    }
    return err;
}

/**
 * @brief Creates a request to make a new file
 *
 * @pre If the path consists of a directory, @ref fs_mkdir must
 * be called to have created the directory first.
 *
 * @param path: absolute path to the file
 * @returns Negative error code if unsuccessful
 */
fs_err_t fs_create(const char* path) {
    fs_err_t err = FS_OK;

    if (fs_initialized) {
        // Get task handle and watchdog id
        TaskHandle_t task    = xTaskGetCurrentTaskHandle();
        task_id_t wd_task_id = 0;
        if (!get_task_id_from_handle(task, &wd_task_id)) {
            return FS_INVAL_TASK_ERR;
        }

        // Make request
        fs_request_t req = {.task = task, .type = CREATE, .args.open_args = {.path = path}, .err = &err};

        xQueueSend(xFSRequestQueueHandle, (void*)&req, portMAX_DELAY);

        // Wait for request to be handled, the filesystem notification bit is the
        // first bit in the task notification value
        uint32_t notif_value = 0;
        set_task_status(wd_task_id, task_asleep);
        while ((notif_value & 0x1) == 0) {
            xTaskNotifyWait(0x1,          /* Clears the filesystem notification bit upon calling this function */
                            0x0,          /* Doesn't clear the notification value upon exiting this function */
                            &notif_value, /* Gets notification value */
                            portMAX_DELAY);
        }
        set_task_status(wd_task_id, task_alive);
    } else {
        err = FS_NOT_INITIALIZED;
    }
    return err;
}

/**
 * @brief Creates a request to ls a particular directory
 *
 * @pre ls_list must be of size MAX(MAX_FILES, MAX_DIRS) * LFS_NAME_MAX
 * @pre dir_path must be a path to an existing directory, otherwise an error will be returned
 *
 * @param dir_path Path to directory that is going to be ls'd
 * @param[out] ls_list Array of files/directories stored at the specified path
 *
 * @returns Negative error code if unsuccessful
 */
fs_err_t fs_ls(const char* dir_path, char ls_list[][LFS_NAME_MAX]) {
    fs_err_t err = FS_OK;
    if (fs_initialized) {
        // Get task handle and watchdog id
        TaskHandle_t task    = xTaskGetCurrentTaskHandle();
        task_id_t wd_task_id = 0;
        if (!get_task_id_from_handle(task, &wd_task_id)) {
            return FS_INVAL_TASK_ERR;
        }

        // Make request
        fs_request_t req = {.task = task, .type = LS, .args.ls_args = {.path = dir_path, .ls_list = (void*)ls_list}, .err = &err};

        xQueueSend(xFSRequestQueueHandle, (void*)&req, portMAX_DELAY);

        // Wait for request to be handled, the filesystem notification bit is the
        // first bit in the task notification value
        uint32_t notif_value = 0;
        set_task_status(wd_task_id, task_asleep);
        while ((notif_value & 0x1) == 0) {
            xTaskNotifyWait(0x1,          /* Clears the filesystem notification bit upon calling this function */
                            0x0,          /* Doesn't clear the notification value upon exiting this function */
                            &notif_value, /* Gets notification value */
                            portMAX_DELAY);
        }
        set_task_status(wd_task_id, task_alive);
    } else {
        err = FS_NOT_INITIALIZED;
    }
    return err;
}

/**
 * @brief Make a request to read from an existing file
 *
 * @pre @fs_create must be used to create the file first
 *
 * @param path: absolute path to the file
 * @param off_bytes: offset in bytes of where to read
 * @param[out] buffer: buffer to store the read data
 * @param size: size in bytes to read
 * @returns Negative error code if unsuccessful
 */
fs_err_t fs_read(const char* path, uint32_t off_bytes, uint8_t* buffer, uint32_t size_bytes) {
    fs_err_t err = FS_OK;
    if (fs_initialized) {
        // Get task handle and watchdog id
        TaskHandle_t task    = xTaskGetCurrentTaskHandle();
        task_id_t wd_task_id = 0;
        if (!get_task_id_from_handle(task, &wd_task_id)) {
            return FS_INVAL_TASK_ERR;
        }

        // Make request
        fs_request_t req = {.task = task, .type = READ, .args.read_args = {.path = path, .off_bytes = off_bytes, .buf = buffer, .size_bytes = size_bytes}, .err = &err};

        xQueueSend(xFSRequestQueueHandle, (void*)&req, portMAX_DELAY);

        // Wait for request to be handled, the filesystem notification bit is the
        // first bit in the task notification value
        uint32_t notif_value = 0;
        set_task_status(wd_task_id, task_asleep);
        while ((notif_value & 0x1) == 0) {
            xTaskNotifyWait(0x1,          /* Clears the filesystem notification bit upon calling this function */
                            0x0,          /* Doesn't clear the notification value upon exiting this function */
                            &notif_value, /* Gets notification value */
                            portMAX_DELAY);
        }
        set_task_status(wd_task_id, task_alive);
    } else {
        err = FS_NOT_INITIALIZED;
    }
    return err;
}

/**
 * @brief Make a request to write to an existing file
 *
 * @pre @fs_create must be used to create the file first
 *
 * @param path: absolute path of the file
 * @param off_bytes: offset in bytes of where to write to
 * @param buffer: buffer of data to write
 * @param size: size in bytes to write
 * @param whence: how to interpret off_bytes (LFS_SEEK_SET, LFS_SEEK_CUR, LFS_SEEK_END)
 * @returns Negative error code if unsuccessful
 */
fs_err_t fs_write(const char* path, uint32_t off_bytes, const uint8_t* buffer, uint32_t size_bytes, int32_t whence) {
    fs_err_t err = FS_OK;
    if (fs_initialized) {
        // Get task handle and watchdog id
        TaskHandle_t task    = xTaskGetCurrentTaskHandle();
        task_id_t wd_task_id = 0;
        if (!get_task_id_from_handle(task, &wd_task_id)) {
            return FS_INVAL_TASK_ERR;
        }

        // Make request
        fs_request_t req = {.task = task, .type = WRITE, .args.write_args = {.path = path, .off_bytes = off_bytes, .buf = buffer, .size_bytes = size_bytes, .whence = whence}, .err = &err};

        xQueueSend(xFSRequestQueueHandle, (void*)&req, portMAX_DELAY);

        // Wait for request to be handled, the filesystem notification bit is the
        // first bit in the task notification value
        uint32_t notif_value = 0;
        set_task_status(wd_task_id, task_asleep);
        while ((notif_value & 0x1) == 0) {
            xTaskNotifyWait(0x1,          /* Clears the filesystem notification bit upon calling this function */
                            0x0,          /* Doesn't clear the notification value upon exiting this function */
                            &notif_value, /* Gets notification value */
                            portMAX_DELAY);
        }
        set_task_status(wd_task_id, task_alive);
    } else {
        err = FS_NOT_INITIALIZED;
    }
    return err;
}

/**
 * @brief Makes a request to delete an existing file or directory
 *
 * @pre @fs_create must be used to create the file first
 *
 * @param path: absolute path of the file or directory
 * @returns Negative error code if unsuccessul
 */
fs_err_t fs_delete(const char* path) {
    fs_err_t err = FS_OK;
    if (fs_initialized) {
        // Get task handle and watchdog id
        TaskHandle_t task    = xTaskGetCurrentTaskHandle();
        task_id_t wd_task_id = 0;
        if (!get_task_id_from_handle(task, &wd_task_id)) {
            return FS_INVAL_TASK_ERR;
        }

        // Make request
        fs_request_t req = {.task = task, .type = DELETE, .args.open_args = {.path = path}, .err = &err};

        xQueueSend(xFSRequestQueueHandle, (void*)&req, portMAX_DELAY);

        // Wait for request to be handled, the filesystem notification bit is the
        // first bit in the task notification value
        uint32_t notif_value = 0;
        set_task_status(wd_task_id, task_asleep);
        while ((notif_value & 0x1) == 0) {
            xTaskNotifyWait(0x1,          /* Clears the filesystem notification bit upon calling this function */
                            0x0,          /* Doesn't clear the notification value upon exiting this function */
                            &notif_value, /* Gets notification value */
                            portMAX_DELAY);
        }
        set_task_status(wd_task_id, task_alive);
    } else {
        err = FS_NOT_INITIALIZED;
    }
    return err;
}

/**
 * @brief Makes a request to get the size of a file
 *
 * @pre @create must be used to create the file first
 *
 * @param path: absolute path of the file
 * @param[out] size: stores returned size
 * @returns Negative error code if unsuccessul
 */
fs_err_t fs_file_size(const char* path, uint32_t* size) {
    fs_err_t err = FS_OK;
    if (fs_initialized) {
        // Get task handle and watchdog id
        TaskHandle_t task    = xTaskGetCurrentTaskHandle();
        task_id_t wd_task_id = 0;
        if (!get_task_id_from_handle(task, &wd_task_id)) {
            return FS_INVAL_TASK_ERR;
        }

        // Make request
        fs_request_t req = {.task = task, .type = FILE_SIZE, .args.fsize_args = {.path = path, .size = size}, .err = &err};

        xQueueSend(xFSRequestQueueHandle, (void*)&req, portMAX_DELAY);

        // Wait for request to be handled, the filesystem notification bit is the
        // first bit in the task notification value
        uint32_t notif_value = 0;
        set_task_status(wd_task_id, task_asleep);
        while ((notif_value & 0x1) == 0) {
            xTaskNotifyWait(0x1,          /* Clears the filesystem notification bit upon calling this function */
                            0x0,          /* Doesn't clear the notification value upon exiting this function */
                            &notif_value, /* Gets notification value */
                            portMAX_DELAY);
        }
        set_task_status(wd_task_id, task_alive);
    } else {
        err = FS_NOT_INITIALIZED;
    }
    return err;
}

/**
 * @brief Get size of the filesystem
 *
 * @returns Size in bytes of the filesystem
 */
uint32_t fs_size(void) {
    if (fs_initialized) {
        return lfs_fs_size(&m_lfs) * cfg.block_size;
    } else {
        return 0;
    }
}

/* Private Functions -----------------------------------------------------------------*/

/**
 * @brief Task that handles all filesystem activity by executing incoming requests
 * through the filesystem request queue.
 *
 * @param pvParameters: Any parameters passed to this task when creating it, is NULL
 *                      for this task.
 */
static void vFileSystemMasterTask(void* pvParameters) {
    fs_request_t req     = {};
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    while (1) {
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
                log_str(ERROR, FS_LOG, true, "Unknown Request: %d", req.type);
                *(req.err) = FS_UNKNOWN_REQUEST_ERR;
                break;
        }

        xSemaphoreGive(xBlockEraseMutex);

        // Notify the calling task of the transaction completion, and pass the filesystem
        // error as the notification value
        xTaskNotify(req.task, 0x01, eSetBits);
    }
}

/**
 * @brief Task that runs daily to create a new set of log files for the next day.
 *
 * @param pvParameters: Any parameters passed to this task when creating it, is NULL
 *                      for this task.
 */
static void vFileSystemLifecycleTask(void* pvParameters) {
    fs_err_t err         = FS_OK;
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    while (1) {
        // Task is executing, set its status to aliver
        set_task_status(wd_task_id, task_alive);

        // Check space remaining in the filesystem
        int32_t blocks_remain = cfg.block_count - lfs_fs_size(&m_lfs);

        // Delete oldest set of files if there is no more space in the filesystem or
        // if we have reached the maximum number of directories
        if ((blocks_remain < BLOCK_REMAIN_THRESHOLD) || (next_dir == MAX_DIRS)) {
            delete_oldest_files();
        }

        // Get the current date for the directory path and make the directory
        char dir_path[DIR_NAME_SIZE] = {0};
        rtc_get_date_to_string(dir_path);

        err = fs_mkdir(dir_path);
        if (err) {
            log_str(ERROR, FS_LOG, true, "FS Error: %d", err);
        } else {
            // Update the directory list and current directory
            memcpy(dir_list[next_dir], dir_path, sizeof(char) * DIR_NAME_SIZE);
            memcpy(curr_dir, dir_path, sizeof(char) * DIR_NAME_SIZE);
            next_dir++;

            // Loop through and create another set of log files
            uint8_t i;
            for (i = 0; i < MAX_FILES; i++) {
                char new_file_path[LFS_NAME_MAX] = {};
                make_file_path(files[i], new_file_path);
                err = fs_create(new_file_path);
                if (err) {
                    log_str(ERROR, FS_LOG, true, "Could not create file %i. FS Error: %d", files[i], err);
                    break;
                }
            }
        }

        // Sleep for the configurable period of time
        set_task_status(wd_task_id, task_asleep);
        vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD_MS(pvParameters)));
    }
}

/**
 * @brief Deletes the oldest directory, which is the directory pointed to
 *        by next_dir in the dir_list.
 *
 * @returns Error code if unsuccessful
 */
static fs_err_t delete_oldest_files(void) {
    if (next_dir == MAX_DIRS) {
        next_dir = 0;
    }

    fs_err_t err = fs_delete(dir_list[next_dir]);

    return err;
}

/**
 * @brief Loops through the root directory to find all existing
 *        subdirectories in the filesystem. Updates the global
 *        dir_list with the name of these directories.
 *
 * @returns Error code if unsuccessful
 */
static fs_err_t update_dir_list(void) {
    fs_err_t err;
    char ls_list[MAX_DIRS][LFS_NAME_MAX] = {};

    err = ls("/", ls_list);
    if (err == FS_OK) {
        uint8_t i;
        for (i = 0; i < MAX_DIRS; i++) {
            memcpy(dir_list[i], ls_list[i], sizeof(char) * DIR_NAME_SIZE);

            if (ls_list[i][0] != '\0') {
                memcpy(curr_dir, ls_list[i], sizeof(char) * DIR_NAME_SIZE);
                next_dir = i; // It's okay for this to equal MAX_DIRS, this is dealt with in
                              // fs_delete_oldest_files
            }
        }
    }

    return (fs_err_t)err;
}

/**
 * @brief Creates a new directory
 *
 * @param: absolute path to the directory
 * @returns Negative error code if unsuccessful
 */
static fs_err_t mkdir(const char* path) {
    if (path == NULL) {
        return FS_PARAMISNULL_ERR;
    }

    return (fs_err_t)lfs_mkdir(&m_lfs, path);
}

/**
 * @brief Creates a new file
 *
 * @pre If the path consists of a directory, @ref mkdir must
 * be called to have created the directory first.
 *
 * @param path: absolute path to the file
 * @returns Negative error code if unsuccessful
 */
static fs_err_t create(const char* path) {
    lfs_file_t f = {NULL};

    if (path == NULL) {
        return FS_PARAMISNULL_ERR;
    }

    // Open the file for writing, create it if it doesn't exist, fail if the file already exists
    int32_t err = lfs_file_open(&m_lfs, &f, path, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_EXCL);
    if (err) {
        return (fs_err_t)err;
    }

    // TODO: figure out what the first thing we write to a file should be

    err = lfs_file_close(&m_lfs, &f);

    return (fs_err_t)err;
}

/**
 * @brief Reads the directory at the specified path and fills in an array
 * with the names of files/directories at the specified path.
 *
 * @pre ls_list must be of size MAX(MAX_FILES, MAX_DIRS) * LFS_NAME_MAX
 * @pre dir_path must be a path to an existing directory, otherwise an error will be returned
 *
 * @param dir_path Path to directory that is going to be ls'd
 * @param[out] ls_list Array of files/directories stored at the specified path
 *
 * @returns Negative error code if unsuccessful
 */
static fs_err_t ls(const char* dir_path, char ls_list[][LFS_NAME_MAX]) {
    int32_t err   = 0;
    lfs_dir_t dir = {0};

    if ((dir_path == NULL) || (ls_list == NULL)) {
        return FS_PARAMISNULL_ERR;
    }

    // Open the root directory
    err = lfs_dir_open(&m_lfs, &dir, dir_path);
    if (err) {
        return (fs_err_t)err;
    }

    // The very first two items in any directory are '.' and '..',
    // which are the directory blocks, so skip these.
    dir.pos = 2;

    // Only read up to either the maximum number of files or directories. There should be
    // no more files/directories than the maximum of the two.
    uint8_t num_read = MAX(MAX_FILES, MAX_DIRS);

    // Loop through all of the files/directories in the specified path
    uint8_t i;
    for (i = 0; i < num_read; i++) {
        struct lfs_info info = {0};

        err = lfs_dir_read(&m_lfs, &dir, &info);
        // If err == 0, then the end of the directory tree has been reached, if err
        // is negative an error was encountered so don't continue trying to read the directories
        if (err <= 0) {
            break;
        }

        strncpy(ls_list[i], info.name, LFS_NAME_MAX);
    }

    err = lfs_dir_close(&m_lfs, &dir);

    return (fs_err_t)err;
}

/**
 * @brief Reads from an existing file
 *
 * @pre @create must be used to create the file first
 *
 * @param path: absolute path to the file
 * @param off_bytes: offset in bytes of where to read
 * @param[out] buffer: buffer to store the read data
 * @param size: size in bytes to read
 * @returns Negative error code if unsuccessful
 */
static fs_err_t read(const char* path, uint32_t off_bytes, uint8_t* buffer, uint32_t size_bytes) {
    lfs_file_t f = {NULL};

    if ((path == NULL) || (buffer == NULL)) {
        return FS_PARAMISNULL_ERR;
    }

    // Open the file for reading
    int32_t err = lfs_file_open(&m_lfs, &f, path, LFS_O_RDONLY);
    if (err) {
        return (fs_err_t)err;
    }

    // Seek to the appropriate offset in the file
    err = (uint32_t)lfs_file_seek(&m_lfs, &f, (lfs_soff_t)off_bytes, LFS_SEEK_SET);
    if (err != off_bytes) {
        lfs_file_close(&m_lfs, &f);
        return (fs_err_t)err;
    }

    // Read from the file
    err = lfs_file_read(&m_lfs, &f, buffer, size_bytes);
    if (err != size_bytes) {
        lfs_file_close(&m_lfs, &f);
        return (fs_err_t)err;
    }

    err = lfs_file_close(&m_lfs, &f);

    return (fs_err_t)err;
}

/**
 * @brief Writes to an existing file
 *
 * @pre @create must be used to create the file first
 *
 * @param path: absolute path of the file
 * @param off_bytes: offset in bytes of where to write to
 * @param buffer: buffer of data to write
 * @param size: size in bytes to write
 * @param whence: how to interpret off_bytes (LFS_SEEK_SET, LFS_SEEK_CUR, LFS_SEEK_END)
 * @returns Negative error code if unsuccessful
 */
static fs_err_t write(const char* path, uint32_t off_bytes, const uint8_t* buffer, uint32_t size_bytes, int32_t whence) {
    lfs_file_t f = {NULL};

    if ((path == NULL) || (buffer == NULL)) {
        return FS_PARAMISNULL_ERR;
    }

    // Open the file for writing
    int32_t err = lfs_file_open(&m_lfs, &f, path, LFS_O_WRONLY);
    if (err) {
        return (fs_err_t)err;
    }

    // Seek to the appropriate offset in the file. Function returns a negative error code
    // upon failure, or the offset in bytes that was seeked to
    err = (uint32_t)lfs_file_seek(&m_lfs, &f, (lfs_soff_t)off_bytes, whence);

    // When LFS_SEEK_SET (seek to provided location), we expect the function to return
    // the offset that we passed in, otherwise we care that a negative error code was not returned
    if ((err < 0) || ((whence == LFS_SEEK_SET) && (err != off_bytes))) {
        lfs_file_close(&m_lfs, &f);
        return (fs_err_t)err;
    }

    // Write to the file
    err = lfs_file_write(&m_lfs, &f, buffer, size_bytes);
    if (err != size_bytes) {
        lfs_file_close(&m_lfs, &f);
        return (fs_err_t)err;
    }

    err = lfs_file_close(&m_lfs, &f);

    return (fs_err_t)err;
}

/**
 * @brief Deletes an existing file or directory
 *
 * @pre @create must be used to create the file first
 *
 * @param path: absolute path of the file or directory
 * @returns Negative error code if unsuccessful
 */
static fs_err_t delete (const char* path) {
    if (path == NULL) {
        return FS_PARAMISNULL_ERR;
    }

    return (fs_err_t)lfs_remove(&m_lfs, path);
}

/**
 * @brief Gets the size of an existing file
 *
 * @pre @create must be used to create the file first
 *
 * @param path: absolute path of the file or directory
 * @param[out] size: stores returned size
 * @returns Negative error code if unsuccessful
 */
static fs_err_t file_size(const char* path, uint32_t* size) {
    struct lfs_info info = {};

    if (path == NULL) {
        return FS_PARAMISNULL_ERR;
    }

    int32_t err = lfs_stat(&m_lfs, path, &info);
    *size       = (uint32_t)info.size;

    return (fs_err_t)err;
}

/* Block Erase Handler ---------------------------------------------------------------*/
/**
 * @brief Each bit in this array holds the erase status of all the blocks in
 * the flash. 1 indicates erase, 0 indicates otherwise.
 */
static uint32_t erased_blocks[BLOCK_COUNT / 32] = {0};

/**
 * @brief Task that periodically erases free blocks in the filesystem. This task
 * only looks at blocks that are currently in the lookahead buffer, which is the
 * buffer that LittleFS uses to find new free blocks.
 */
static void vBlockEraseTask(void* pvParameters) {
    uint8_t i            = 0;
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);

    while (1) {
        set_task_status(wd_task_id, task_asleep);
        xSemaphoreTake(xBlockEraseMutex, portMAX_DELAY);
        set_task_status(wd_task_id, task_alive);

        // Find the next free block, method taken from lfs_alloc
        lfs_block_t off = (m_lfs.free.i + i) % BLOCK_COUNT;
        if (!(bool)(m_lfs.free.buffer[off / 32] & (1 << (off % 32)))) {
            lfs_block_t block = (m_lfs.free.off + off) % m_lfs.cfg->block_count;

            // If the free block has not been erased yet, erase it
            if (!block_bit_set(block)) {
#ifdef PLATFORM_LAUNCHPAD_1224
                flash_erase(block * BLOCK_SIZE, SECTOR_1K);
#else
                flash_erase(block * BLOCK_SIZE, SECTOR_4K);
#endif
                set_block_bit(block);
            }
        }

        xSemaphoreGive(xBlockEraseMutex);

        i++;
        vTaskDelay(TASK_PERIOD_MS(pvParameters));
    }
}

/**
 * @brief Checks if the block has been erased.
 *
 * @param block: block to check
 * @return True if block has been erased, False otherwise
 */
bool block_bit_set(lfs_block_t block) {
    return erased_blocks[block / 32] & (1 << (block % 32));
}

/**
 * @brief Marks the block as being erased
 *
 * @param block: block to mark
 */
void set_block_bit(lfs_block_t block) {
    erased_blocks[block / 32] |= (1 << (block % 32));
}

/**
 * @brief Marks the block as not being erased
 *
 * @param block: block to mark
 */
void clear_block_bit(lfs_block_t block) {
    erased_blocks[block / 32] &= ~(1 << (block % 32));
}

/* Functions Called by LittleFS for Device Access ------------------------------------*/

/**
 * @brief Read from a particular block in the flash.
 *
 * @param cnfg: configuration structure used to initialize the filesystem
 * @param block: block number to read from (blocks are 4KB size)
 * @param off_bytes: offset in bytes to read from
 * @param[out] buffer: data buffer to store read data
 * @param size_bytes: size in bytes of data to read
 * @return Negative error code if unsuccessful
 */
static int32_t bd_read(const struct lfs_config* cnfg, lfs_block_t block, lfs_off_t off_bytes, void* buffer, lfs_size_t size_bytes) {
    return (int32_t)flash_read((block * cnfg->block_size) + off_bytes, size_bytes, (uint8_t*)buffer);
}

/**
 * @brief Write to a particular block in the flash.
 *
 * @param cnfg: configuration structure used to initialize the filesystem
 * @param block: block number to write to (blocks are 4KB size)
 * @param off_bytes: offset in bytes to write to
 * @param buffer: data buffer to write
 * @param size_bytes: size in bytes of data to write
 * @return Negative error code if unsuccessful
 */
static int32_t bd_prog(const struct lfs_config* cnfg, lfs_block_t block, lfs_off_t off_bytes, const void* buffer, lfs_size_t size_bytes) {
    clear_block_bit(block); // Clear to indicate that this block no longer has an erased state
    return (int32_t)flash_write((block * cnfg->block_size) + off_bytes, size_bytes, (const uint8_t*)buffer);
}

/**
 * @brief Erase a particular block in the flash.
 *
 * @param cnfg: configuration structure used to initialize the filesystem
 * @param block: block number to erase (blocks are 4KB size)
 * @return Negative error code if unsuccessful
 */
static int32_t bd_erase(const struct lfs_config* cnfg, lfs_block_t block) {
    if (!block_bit_set(block)) {
        set_block_bit(block); // Set to indicate that this block now has an erased state
#ifdef PLATFORM_ORCA_V1 // TODO(ALEA-511): include PLATFORM_ALEA_V1 once flash driver is working (ALEA-174)
        return (int32_t)flash_erase(block * cnfg->block_size, SECTOR_4K);
#else
        return (int32_t)flash_erase(block * cnfg->block_size, SECTOR_1K);
#endif
    } else {
        return 0;
    }
}

/**
 * @brief Sync the state of the underlying block device.
 *
 * We are currently not making use of the caching features of LittleFS,
 * so this function does not need to do anything.
 *
 * @param cnfg: configuration structure used to initialize the filesystem
 * @return Currently returns no error since this function doesn't do anything
 */
static int32_t bd_sync(const struct lfs_config* cnfg) {
    return 0;
}

/**
 * @brief Returns the current date as a formatted string in the form YYYY-MM-DD
 * @param[out] buf A character buffer where the generated string will be placed. Must be at least
 * @ref DIR_NAME_SIZE bytes.
 *
 * @returns Date as a string
 */
static void rtc_get_date_to_string(char buf[DIR_NAME_SIZE]) {
    real_time_t curr_time = orca_time_init;

    // Get the current time
    rtc_err_t err = get_current_time(&curr_time);
    if (err != RTC_NO_ERR) {
        log_str(ERROR, RTC_LOG, true, "RTC ERROR: %d", err);
    }

    time_to_ymd_string(&curr_time, buf);
}
