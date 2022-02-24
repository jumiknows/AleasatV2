/**
 * @file test_ecc_flash.c
 * @brief Runs several test on the ECC flash including:
 * 			- Various sided reads
 * 			- Various sized writes
 * 			- Chip erase and sector erase
 *
 * @author Andrada Zoltan
 * @date June 30, 2020
 */

#include "unit_tests.h"
#include "obc_uart.h"
#include "obc_ecc_flash.h"
#include "logger.h"

bool ecc_flash_write_sequence(uint32_t address, uint32_t test_data_size, uint8_t* test_data);
bool ecc_flash_check_sequence(uint32_t address, uint32_t size, uint8_t* test_data, uint32_t pass_fail);

/**
 * @brief Test the ECC flash basic read / write / erase capabilities
 *
 * @pre @ref ecc_flash_init is called
 * @return True if all the tests pass
 */
uint32_t test_ecc_flash(void) {
    uint32_t result_count     = 0;
    uint8_t test_bytes_4[4]   = {0xC0, 0xFF, 0xEE, 0x11};
    uint8_t test_bytes_16[16] = {0x01, 0x01, 0x00, 0x07, 0x03, 0x05, 0x0F, 0x04, 0x07, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t test_bytes_19[19] = {0x01, 0x01, 0x00, 0x07, 0x03, 0x05, 0x0F, 0x04, 0x07, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x07, 0x0B};
    uint8_t test_bytes_48[48] = {0};

    // Initialize the flash
    ecc_flash_init();

    // Erase so we're at a known state for testing
    ecc_flash_erase(0x00, ECC_FULL_CHIP);

    // Test a write of 16 bytes
    if (ecc_flash_write_sequence(64, 16, test_bytes_16)) {
        result_count++;
    }

    // Test a write of more than 16 bytes
    if (ecc_flash_write_sequence(80, 19, test_bytes_19)) {
        result_count++;
    }

    // Test a write of less than 16 bytes
    if (ecc_flash_write_sequence(128, 4, test_bytes_4)) {
        result_count++;
    }

    // Test a write of many bytes
    if (ecc_flash_write_sequence(1600, 48, test_bytes_48)) {
        result_count++;
    }

    ecc_flash_write_sequence(0, 48, test_bytes_48);      // Write bytes into page 0
    ecc_flash_write_sequence(262144, 48, test_bytes_48); // Write bytes into page 1

    // Erase Sector 0 (addresses 0-262143)
    ecc_flash_erase(0x00, ECC_SECTOR_256K);

    if (ecc_flash_check_sequence(0, 48, test_bytes_48, 0)) { // Should read 1's since the sector was erased
        result_count++;
    }
    if (ecc_flash_check_sequence(262144, 48, test_bytes_48, 1)) { // Should correctly read second set of bytes which are on the next sector
        result_count++;
    }

    if (result_count == 6) {
        log_str(DEBUG, FLASH_LOG, false, "Flash test passed.");
        return true; // passed!
    }

    log_str(DEBUG, FLASH_LOG, false, "Flash test: %d/6 passed.", result_count);
    return false;
}

/**
 * @brief Write a sequence to ECC flash and check if the write was successful.
 *
 * @param address: address to write to
 * @param test_data_size: number of bytes to write
 * @param test_data: data to write
 * @return True if the write was successful
 */
bool ecc_flash_write_sequence(uint32_t address, uint32_t test_data_size, uint8_t* test_data) {
    ecc_flash_write(address, test_data_size, test_data);

    return ecc_flash_check_sequence(address, test_data_size, test_data, true);
}

/**
 * @brief Check that the data at a specific address matches what is expected.
 *
 * @param address: address to check
 * @param size: number of bytes to check
 * @param test_data: data to compare against
 * @param pass_fail: True if expect to pass, False if expect to fail
 * @return True if the check is successful
 */
bool ecc_flash_check_sequence(uint32_t address, uint32_t size, uint8_t* test_data, uint32_t pass_fail) {
    uint32_t idx;
    uint8_t check_buf[48] = {0};

    ecc_flash_read(address, size, check_buf);

    for (idx = 0; idx < size; idx++) {
        if (check_buf[idx] != test_data[idx]) {
            return (false == pass_fail);
        }
    }

    return (true == pass_fail);
}
