/*
 * filesystem.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Andrada Zoltan
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "sys_common.h"
#include "obc_error.h"
#include "obc_rtc.h"
#include "FreeRTOS.h"
#include "rtos_task.h"

/**
 * @brief Holds the names of all the files that will be generated
 * every day using the FileSystemLifecycleTask. No other files besides
 * these will exist.
 */
#define MAX_FILES 5
extern const char* files[MAX_FILES];

/**
 * @brief Length of a string that holds the date in the format
 * "YYYY-MM-DD". Used for directory naming. All directories will be
 * named in this format. Includes null terminator.
 */
#define DIR_NAME_SIZE 11

/**
 * @brief Maximum length of a file name, including null terminator.
 */
#define FILE_NAME_SIZE 11

/**
 * @brief Holds names of the first 7 directories stored in the root
 * directory that are currently in the filesystem.
 *
 * Note: We should never surpass this quantity of directories, since
 * directories are only created in @ref vFileSystemLifecycleTask, where
 * this constraint is checked.
 */
#define MAX_DIRS 7
extern char curr_dir[DIR_NAME_SIZE];

/**
 * @brief Indicator for whether or not the filesystem initialization has
 * successfully completed.
 */
extern bool fs_initialized;

/* Task Handle Declarations */
extern TaskHandle_t xBlockEraseTaskHandle;
extern TaskHandle_t xFileSystemMasterTaskHandle;
extern TaskHandle_t xFileSystemLifecycleTaskHandle;

/* User Functions */
void make_file_path(const char* file_name, char* output_path);
fs_err_t fs_init(void);
fs_err_t fs_deinit(void);
fs_err_t fs_mkdir(const char* path);
fs_err_t fs_create(const char* path);
fs_err_t fs_ls(const char* dir_path, char ls_list[][LFS_NAME_MAX]);
fs_err_t fs_read(const char* path, uint32_t off_bytes, uint8_t* buffer, uint32_t size_bytes);
fs_err_t fs_write(const char* path, uint32_t off_bytes, const uint8_t* buffer, uint32_t size_bytes, int32_t whence);
fs_err_t fs_delete(const char* path);
fs_err_t fs_file_size(const char* path, uint32_t* size);
uint32_t fs_size(void);

bool block_bit_set(lfs_block_t block);
void set_block_bit(lfs_block_t block);
void clear_block_bit(lfs_block_t block);

#endif /* FILESYSTEM_H_ */
