/*
 * test_filesystem.c
 *
 *  Created on: Nov 19, 2019
 *      Author: Andrada
 */

#include "unit_tests.h"
#include "obc_uart.h"
#include "logger.h"
#include "filesystem.h"
#include "obc_flash.h"

/*
 * @brief Testing function for basic filesystem functionality
 *
 * @returns 0 if an error was encountered, 1 if the test passed
 */
uint32_t test_filesystem(void) {
    fs_err_t err = FS_OK;

    // Create a directory
    err = fs_mkdir("test_dir");
    if (err) {
        log_str(ERROR, FS_LOG, true, "fs_mkdir error: %d", err);
        return 0;
    }

    // Create the file
    err = fs_create("test_dir/test_file");
    if (err) {
        log_str(ERROR, FS_LOG, true, "fs_create error: %d", err);
        return 0;
    }

    // Create buffer to write
    uint16_t i = 0;
    uint8_t buffer[256];
    for (i = 0; i < 256; i++) {
        buffer[i] = i;
    }

    // Write to the end of the file
    err = fs_write("test_dir/test_file", 0, buffer, 256, LFS_SEEK_END);
    if (err) {
        log_str(ERROR, FS_LOG, true, "fs_write error: %d", err);
        return 0;
    }

    // Read back all the data
    uint8_t read_buffer[256] = {0};
    err                      = fs_read("test_dir/test_file", 0, read_buffer, 256);
    if (err) {
        log_str(ERROR, FS_LOG, true, "fs_read error: %d", err);
        return 0;
    }

    // Compare read data
    for (i = 0; i < 256; i++) {
        if (buffer[i] != read_buffer[i]) {
            log_str(ERROR, FS_LOG, true, "Data Written (%d) != Data Read (%d)", buffer[i], read_buffer[i]);
            return 0;
        }
    }

    // Overwrite some random place in the file
    err = fs_write("test_dir/test_file", 50, buffer, 20, LFS_SEEK_SET);
    if (err) {
        log_str(ERROR, FS_LOG, true, "fs_write error: %d", err);
        return 0;
    }

    // Read it back
    err = fs_read("test_dir/test_file", 50, read_buffer, 20);
    if (err) {
        log_str(ERROR, FS_LOG, true, "fs_read error: %d", err);
        return 0;
    }

    // Compare read data
    for (i = 0; i < 20; i++) {
        if (buffer[i] != read_buffer[i]) {
            log_str(ERROR, FS_LOG, true, "Data Written (%d) != Data Read (%d)", buffer[i], read_buffer[i]);
            return 0;
        }
    }

    // Delete the file
    fs_delete("test_dir/test_file");
    if (err) {
        log_str(ERROR, FS_LOG, true, "fs_delete error: %d", err);
        return 0;
    }

    // Delete the directory
    fs_delete("test_dir");
    if (err) {
        log_str(ERROR, FS_LOG, true, "fs_delete error: %d", err);
        return 0;
    }

    log_str(INFO, FS_LOG, false, "Filesystem test passed!");
    return 1;
}
