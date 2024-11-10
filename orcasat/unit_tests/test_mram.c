/**
 * @file test_mram.c
 * @brief Tests read and write operations on the MR25H256 MRAM chip.
 *
 * @author Richard Arthurs
 * @date June 30, 2020
 */
#include "string.h"
#include "unit_tests.h"
#include "logger.h"
#include "mr25h256.h"
#include "obc_error.h"

// Private functions
static void test_mram_status_reg(void);
static void mram_write_sequence(uint16_t addr, uint16_t size, uint8_t* write_data, mram_err_t expected_err);
static void mram_read_verify_sequence(uint16_t addr, uint16_t size, uint8_t* data_read, const uint8_t* expected_data);

// Test data
uint8_t data[] = {0x03, 0xFF, 0x77, 0x01, 0x00, 0x24};

/**
 * @brief Initializes and runs several tests on the MRAM.
 */
void test_mram(void) {
    // Initialize the MRAM
    if (mram_mr25h256_init() != MRAM_OK) {
        log_str(ERROR, PRINT_GENERAL, false, "MRAM error");
    } else {
        log_str(INFO, PRINT_GENERAL, false, "MRAM init OK");
    }

    // Data readback buffer
    uint8_t data_read[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Basic read and verify
    mram_write_sequence(0, 6, data, MRAM_OK);
    mram_read_verify_sequence(0, 6, data_read, data);

    // Read and verify a middle address
    mram_write_sequence(5000, 6, data, MRAM_OK);
    mram_read_verify_sequence(5000, 6, data_read, data);

    // Read and verify the last few addresses
    mram_write_sequence(MR25H256_MRAM_MAX_ADDRESS - 6, 6, data, MRAM_OK);
    mram_read_verify_sequence(MR25H256_MRAM_MAX_ADDRESS - 6, 6, data_read, data);
    mram_read_verify_sequence(0, 6, data_read, data); // Read the beginning data again to ensure that the max write didn't wrap

    // Check that a write beyond the MRAM's max size is detected.
    mram_write_sequence(MR25H256_MRAM_MAX_ADDRESS - 4, 6, data, MRAM_INDEX_OUT_OF_BOUND);

    // Check that the out-of-bounds write didn't actually edit any data.
    mram_read_verify_sequence(MR25H256_MRAM_MAX_ADDRESS - 6, 6, data_read, data);

    // Change the test data and do the writes and verifies again to ensure
    // that the test data can be changed in MRAM.
    uint8_t i = 0;
    for (i = 0; i < 6; i++) {
        data[i] = ~data[i];
    }
    mram_write_sequence(0, 6, data, MRAM_OK);
    mram_read_verify_sequence(0, 6, data_read, data);
    mram_write_sequence(5000, 6, data, MRAM_OK);
    mram_read_verify_sequence(5000, 6, data_read, data);
    mram_write_sequence(MR25H256_MRAM_MAX_ADDRESS - 6, 6, data, MRAM_OK);
    mram_read_verify_sequence(MR25H256_MRAM_MAX_ADDRESS - 6, 6, data_read, data);
    mram_read_verify_sequence(0, 6, data_read, data); // Read the beginning data again to ensure that the max write didn't wrap

    // Test getting the status register.
    test_mram_status_reg();
}

/**
 * @brief Tests that the status register can be read.
 */
static void test_mram_status_reg(void) {
    log_str(INFO, PRINT_GENERAL, false, "Testing MRAM status reg");

    // We expect that the driver always resets the status reg to 0x00,
    // which means everything is write protected.
    uint8_t reg_data      = 0xFF;
    mram_err_t mram_error = mram_mr25h256_read_status(&reg_data);
    if (mram_error != MRAM_OK) {
        log_str(ERROR, PRINT_GENERAL, false, "Read reg err: %d", mram_error);
    } else {
        log_str(INFO, PRINT_GENERAL, false, "Reg read succeeded");
    }

    // Check the reg value.
    if (reg_data == 0) {
        log_str(INFO, PRINT_GENERAL, false, "Reg read correct");
    } else {
        log_str(ERROR, PRINT_GENERAL, false, "Reg expect 0 but got %d", reg_data);
    }
}

/**
 * @brief Write data to MRAM and expect a certain return value from the MRAM driver.
 * @param addr The starting address in MRAM to write.
 * @param size The number of bytes to write.
 * @param write_data The data to write to the MRAM.
 * @param expected_err The expected return value from the MRAM driver.
 */
static void mram_write_sequence(uint16_t addr, uint16_t size, uint8_t* write_data, mram_err_t expected_err) {
    mram_err_t mram_error = mram_mr25h256_write(addr, size, write_data);
    if (mram_error != expected_err) {
        log_str(ERROR, PRINT_GENERAL, false, "Write %d exp %d got err %d", addr, expected_err, mram_error);
    } else {
        log_str(INFO, PRINT_GENERAL, false, "Write addr %d %d OK got %d", addr, size, mram_error);
    }
}

/**
 * @brief Read data from MRAM and compare it against a given set of data.
 * @param[in] addr The starting address in MRAM to read from.
 * @param[in] size The number of bytes to read.
 * @param[out] data_read The data read from MRAM.
 * @param[in] expected_data The data we expect to read from MRAM.
 */
static void mram_read_verify_sequence(uint16_t addr, uint16_t size, uint8_t* data_read, const uint8_t* expected_data) {
    // Zero out the read data buffer before trying to read.
    // Since we often call this multiple times with the same expected data and the same
    // read buffer, resetting the read data ensures we actually detect if the MRAM fails
    // to return any data.
    memset(data_read, 0x00, size);

    mram_err_t mram_error = mram_mr25h256_read(addr, size, data_read);

    if (mram_error != MRAM_OK) {
        log_str(ERROR, PRINT_GENERAL, false, "Read addr err %d", addr, mram_error);
    } else {
        log_str(INFO, PRINT_GENERAL, false, "Read addr %d %d OK", addr, size);
    }

    // Check the data.
    if (memcmp(data_read, expected_data, size) != 0) {
        log_str(ERROR, PRINT_GENERAL, false, "Data mismatch");
    } else {
        log_str(INFO, PRINT_GENERAL, false, "Data matches!");
    }
}
