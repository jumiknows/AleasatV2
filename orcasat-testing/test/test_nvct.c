/**
 * @file test_nvct.h
 * @brief Unit tests for the NVCT module.
 * @author Richard A
 * @date Aug 26, 2020
 */

#include "unity.h"
#include "nvct.h"

#include "FreeRTOS.h"
#include "mock_rtos_portable.h"
#include "mock_rtos_portmacro.h"
#include "mock_mpu_prototypes.h"
#include "lfs_util.h"
#include "mock_obc_settings.h"
#include "mock_obc_mram.h"

// The tests
void test_write_nvct(void);

/**
 * @brief An array to mock the MRAM storage.
 */
#define MOCK_MRAM_SIZE 1024
uint8_t mram[MOCK_MRAM_SIZE];

/**
 * @brief Size of an NVCT entry (bytes)
 * The struct is packed tightly in OBC firmware with pragmas.
 */
#define NVCT_ENTRY_SIZE_BYTES 9

// Mocks
mram_err_t mock_mram_write(uint16_t addr, uint16_t size_bytes, const uint8_t* data);
mram_err_t mock_mram_read(uint16_t addr, uint16_t size_bytes, uint8_t* data);

// Helpers
void write_mram_entry(bool config, uint32_t value, uint32_t address);
void check_mram_entry(uint32_t mram_addr, uint8_t config, uint32_t value);

/**
 * @brief Runs before each test. Required by Ceedling.
 */
void setUp(void) {
    // Empty out the MRAM array.
    memset(mram, 0xFF, MOCK_MRAM_SIZE);
}

void tearDown(void) {
    // Required by Ceedling.
}

/**
 * @brief Tests that NVCT can be written and that writes don't
 * overlap if they aren't supposed to. Also tests initialization
 * with a pre-provisioned firmware version.
 */
void test_write_nvct(void) {
    application_image_base_mram_address_IgnoreAndReturn(0);
    set_uint32_from_internal_IgnoreAndReturn(SETTING_OK);

    // Setup: get an entry with value 123 into the MRAM.
    // init_nvct() will count this as the firmware version,
    // which unlocks the ability to read and write.
    write_mram_entry(0, 123, 0);

    // Setup: init_nvct and look for the firmware version 123 we magically provisioned above.
    read_mram_StubWithCallback(mock_mram_read);
    TEST_ASSERT_EQUAL_UINT32(NVCT_SUCCESS, init_nvct(123));

    // Set up the expectations.
    write_mram_StubWithCallback(mock_mram_write);

    // Write an entry to NVCT index 1. Check that it's there.
    nvct_err_enum_t res = set_nvct_value(true, 1 /* write index */, 12345);
    TEST_ASSERT_EQUAL_UINT32(NVCT_SUCCESS, res);
    check_mram_entry((1 * NVCT_ENTRY_SIZE_BYTES), 1, 12345);

    // Write another entry into NVCT index 2.
    res = set_nvct_value(false, 2 /* write index */, 67890);
    TEST_ASSERT_EQUAL_UINT32(NVCT_SUCCESS, res);

    // Check that both entries are in the expected places.
    check_mram_entry((2 * NVCT_ENTRY_SIZE_BYTES), 0, 67890);
    check_mram_entry((1 * NVCT_ENTRY_SIZE_BYTES), 1, 12345);
    check_mram_entry((0 * NVCT_ENTRY_SIZE_BYTES), 0, 123);

    // Overwrite the index 1 entry, check that it is updated
    // and the second entry is untouched.
    res = set_nvct_value(false, 1 /* write index */, 983421);
    TEST_ASSERT_EQUAL_UINT32(NVCT_SUCCESS, res);
    check_mram_entry((2 * NVCT_ENTRY_SIZE_BYTES), 0, 67890);
    check_mram_entry((1 * NVCT_ENTRY_SIZE_BYTES), 0, 983421);
    check_mram_entry((0 * NVCT_ENTRY_SIZE_BYTES), 0, 123);
}

//----------- Helpers -----------
/**
 * @brief Checks that the MRAM array contains the expected data.
 * If the data does not match and pass the CRC check, assertions will fail.
 *
 * @param mram_addr the address in MRAM where the entry starts.
 * @param config the expected value of the .config field for the entry.
 * @param value the expected value of the .value field for the entry.
 */
void check_mram_entry(uint32_t mram_addr, uint8_t config, uint32_t value) {
    // Copy the expected data into an array so we can calculate its CRC.
    uint8_t entry[sizeof(config) + sizeof(value)];
    memcpy(entry, &config, sizeof(config));
    memcpy(&entry[sizeof(config)], &value, sizeof(value));

    uint32_t crc = lfs_crc(0, entry, sizeof(entry));

    // Check each portion of the entry.
    TEST_ASSERT_EQUAL_UINT8(mram[mram_addr], config);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(&mram[mram_addr + sizeof(config)], &value, 1);
    TEST_ASSERT_EQUAL_UINT32_ARRAY(&mram[mram_addr + sizeof(config) + sizeof(value)], &crc, 1);
}

/**
 * @brief Creates and writes an NVCT entry to the mock MRAM. This does what
 * the firmware implementation _should_ do, but is used in tests to provision
 * the mock MRAM with values that firmware expects to be there.
 * @param config The configuration value to set in MRAM.
 * @param value The value to write for the entry.
 * @param address The NVCT address at which to begin writing.
 */
void write_mram_entry(bool config, uint32_t value, uint32_t address) {
    TEST_ASSERT_MESSAGE(address % NVCT_ENTRY_SIZE_BYTES == 0, "Writing mock MRAM entries must be done to an address that's a multiple of the entry size.");

    // Stage the fake NVCT entry in a buffer before putting it in the mock MRAM. Makes the math easier.
    uint8_t entry[NVCT_ENTRY_SIZE_BYTES];

    // Fill in the value and config.
    memcpy(entry, &config, sizeof(config));
    memcpy(&entry[sizeof(config)], &value, sizeof(value));

    // Calculate the CRC of the values and write the CRC in.
    uint32_t crc = lfs_crc(0, entry, sizeof(config) + sizeof(value));
    memcpy(&entry[sizeof(config) + sizeof(value)], &crc, sizeof(crc));

    // Copy the NVCT entry to the desired address in mock MRAM.
    memcpy(&mram[address], entry, sizeof(entry));
}

//----------- Mocks -----------
/**
 * @brief Mocks an MRAM write by copying data into the MRAM array.
 * @param[in] addr The MRAM address to write to.
 * @param[in] size_bytes The number of bytes to write.
 * @param[in] data The data to write.
 */
mram_err_t mock_mram_write(uint16_t addr, uint16_t size_bytes, const uint8_t* data) {
    memcpy(&mram[addr], data, size_bytes);
    return MRAM_OK;
}

/**
 * @brief Mocks an MRAM read by copying data out of the MRAM array.
 * @param[in] addr The MRAM address to read from.
 * @param[in] size_bytes The number of bytes to read.
 * @param[out] data Buffer that the read data will be copied to.
 */
mram_err_t mock_mram_read(uint16_t addr, uint16_t size_bytes, uint8_t* data) {
    memcpy(data, &mram[addr], size_bytes);
    return MRAM_OK;
}
