/** @file nvct.c
 *   @brief Non Volatile Configuration Table
 *   @author Alireza Alidousti, Richard Arthurs
 *   @date Updated Aug 26, 2020, Created Oct 30, 2019
 */

#include "nvct.h"
#include "FreeRTOS.h"
#include "rtos_semphr.h"
#include "stdbool.h"
#include "obc_mram.h"
#include "lfs_util.h"
#include "logger.h"
#include "obc_misra.h"
#include "settings_defs.h"
#include "obc_settings.h"

#define NVCT_MAX_NUM_ENTRIES 100
#define NVCT_MAX_NUM_TABLES  3
#define NVCT_MRAM_OFFSET     application_image_base_mram_address()

typedef uint32_t nvct_crc_t;
typedef uint8_t nvct_config_t;

#pragma pack(push, 1) // Ensure all fields are byte-aligned. Default alignment is word (32 bits).
typedef struct {
    nvct_config_t config;
    nvct_value_t value;
    nvct_crc_t crc;
} _nvct_entry_t;
#pragma pack(pop)

#define CRC_BUFFER_LEN       (sizeof(nvct_config_t) + sizeof(nvct_value_t))
#define TABLE_ENTRY_SIZE     sizeof(_nvct_entry_t)
#define TABLE_VERSION_OFFSET sizeof(firmware_version_t)
#define TABLE_DATA_SIZE      (TABLE_ENTRY_SIZE * NVCT_MAX_NUM_ENTRIES)
#define TABLE_SIZE           (TABLE_DATA_SIZE)

/**
 * @brief Whether or not @ref table_offset is valid.
 * Table offset is valid if a table was found for the current
 * firmware version, and invalid otherwise.
 *
 * If we don't have a valid table in NVCT, we don't bother
 * trying to update or read from NVCT.
 */
static bool table_offset_valid = false;

/**
 * @brief The offset for the table of this FW version.
 * Only used if @ref table_offset_valid is true.
 */
static uint32_t table_offset = 0;
// ---------------------------------- Fun Def ---------------------------------

static nvct_err_enum_t write_to_store(uint32_t table_idx, uint8_t index, _nvct_entry_t entry);
static uint32_t mram_offset(uint32_t table_idx, uint8_t offset);
static uint32_t entry_offset(uint32_t entry_idx);
static nvct_crc_t calculate_crc(nvct_config_t config, nvct_value_t value);
static _nvct_entry_t create_nvct_entry(bool mutable, nvct_value_t value);

// ------------------------------------- Helpers ------------------------------

/**
 * @brief Writes an NVCT entry update to the store.
 *
 * @param table_idx: The table offset to use.
 * @param index: An index into an NVCT.
 * @param entry: Updated NVCT entry to be stored at the given index.
 * @retval NVCT_SUCCESS the entry is successfully updated in the store.
 * @retval NVCT_FAILED_TO_WRITE_TO_MRAM_ERROR if MRAM errors occurred.
 */
static nvct_err_enum_t write_to_store(uint32_t table_idx, uint8_t index, _nvct_entry_t entry) {
    uint32_t addr    = mram_offset(table_idx, TABLE_ENTRY_SIZE * index);
    mram_err_t error = write_mram(addr, TABLE_ENTRY_SIZE, (const uint8_t*)&entry);
    if (error != MRAM_OK) {
        return NVCT_FAILED_TO_WRITE_TO_MRAM_ERROR;
    }
    // TODO: read back
    return NVCT_SUCCESS;
}

/**
 * @brief translates an offset within a table to an offset within MRAM
 *
 * @param table_idx: index of the NVCT table
 * @param offset: offset within an NVCT table
 * @return offset within the MARM
 */
static uint32_t mram_offset(uint32_t table_idx, uint8_t offset) {
    return NVCT_MRAM_OFFSET + (table_idx * TABLE_SIZE) + offset;
}

/**
 * @brief Translates an entry index into an offset within a table.

 * @param entry_idx The index of the entry.
 * @return offset within the table
 */
static uint32_t entry_offset(uint32_t entry_idx) {
    return TABLE_ENTRY_SIZE * entry_idx;
}

/**
 * @brief Calculates a CRC over NVCT-entry config and value fields.
 *
 * @param config: config field of an nvct entry
 * @param value: value field of an nvct entry
 * @return  CRC over the concatinated bits of config  and value fields of entry
 */
static nvct_crc_t calculate_crc(nvct_config_t config, nvct_value_t value) {
    uint8_t buffer[CRC_BUFFER_LEN] = {0};
    memcpy(buffer, &config, sizeof(config));
    memcpy(&buffer[sizeof(config)], &value, sizeof(value));
    return lfs_crc(0, buffer, CRC_BUFFER_LEN);
}

/**
 * @brief Creates an NVCT entry by calculating the CRC of the configuration
 * and the value.
 * @param[in] mutable The mutable bit to write in the NVCT line.
 * @param[in] value The NVCT value to configure.
 * @return An NVCT entry with config, value, and CRC set.
 */
static _nvct_entry_t create_nvct_entry(bool mutable, nvct_value_t value) {
    nvct_config_t config = mutable & 1UL;
    nvct_crc_t crc       = calculate_crc(config, value);

    _nvct_entry_t entry = {
        .config = config,
        .value  = value,
        .crc    = crc,
    };
    return entry;
}

// --------------------------------------- API --------------------------------

/**
 * @brief Initializes NVCT and checks for a table with the
 * correct firmware version.
 *
 * If a table with the correct firmware version is found,
 * reading and writing NVCT values will operate correctly.
 *
 * @param[in] firmware_v: OBC Firmware Version.
 * @retval  NVCT_SUCCESS when nvct is initialized successfully.
 * @retval NVCT_NO_TABLE_WITH_MATCHING_FIRMWARE_VERSION if no table with the firmware version is found.
 */
nvct_err_enum_t init_nvct(firmware_version_t firmware_v) {
    // Look through each table in the NVCT and try to find one with the
    // correct firmware version.
    uint8_t test_table_offset = 0;
    for (test_table_offset = 0; test_table_offset < NVCT_MAX_NUM_TABLES; test_table_offset++) {
        // Read setting, check CRC and check against FW version.
        _nvct_entry_t entry = {};
        if (read_mram(mram_offset(test_table_offset, FIRMWARE_VERSION_NVCT_INDEX), TABLE_ENTRY_SIZE, (uint8_t*)&entry) == MRAM_OK) {
            if (calculate_crc(entry.config, entry.value) == entry.crc) {
                if (entry.value == firmware_v) {
                    table_offset       = test_table_offset;
                    table_offset_valid = true;
                    set_uint32_from_internal("nvct_table", table_offset);
                    break;
                }
            }
        }
    }
    return table_offset_valid ? NVCT_SUCCESS : NVCT_NO_TABLE_WITH_MATCHING_FIRMWARE_VERSION;
}

/**
 * @brief Writes an NVCT entry to storage.
 *
 * @param mutable: Whether the entry should be configured as mutable or not.
 * @param index: Index into the NVCT.
 * @param value: Value to be stored at the given index.
 * @retval  NVCT_SUCCESS when the entry is successfully updated.
 * @retval  NVCT_INDEX_OUT_OF_BOUND for an invalid index.
 * @retval NVCT_FAILED_TO_WRITE_TO_MRAM_ERROR if writing or readback experienced an MRAM error.
 * @retval NVCT_NO_TABLE_WITH_MATCHING_FIRMWARE_VERSION if the table was not initialized.
 */
nvct_err_enum_t set_nvct_value(bool mutable, uint8_t index, nvct_value_t value) {
    if (!table_offset_valid) {
        return NVCT_NO_TABLE_WITH_MATCHING_FIRMWARE_VERSION;
    }

    if (!(index < NVCT_MAX_NUM_ENTRIES)) {
        return NVCT_INDEX_OUT_OF_BOUND;
    }

    _nvct_entry_t entry   = create_nvct_entry(mutable, value);
    nvct_err_enum_t error = write_to_store(table_offset, index, entry);
    return error;
}

/**
 * @brief Read an NVCT value by index from the storage.
 *
 * @param index: Index into the NVCT cache.
 * @param [out] value: pointer to nvct_value_t to store the indexed value.
 * @retval NVCT_SUCCESS when the value is successfully read.
 * @retval NVCT_INDEX_OUT_OF_BOUND for an invalid index.
 * @retval NVCT_CRC_ERROR if the value was read but the CRC does not match.
 * @retval NVCT_FAILED_TO_READ_FROM_MRAM_ERROR if the value could not be read.
 * @retval NVCT_NO_TABLE_WITH_MATCHING_FIRMWARE_VERSION if the table was not initialized.
 */
nvct_err_enum_t get_nvct_value(uint8_t index, nvct_value_t* value) {
    if (!table_offset_valid) {
        return NVCT_NO_TABLE_WITH_MATCHING_FIRMWARE_VERSION;
    }

    if (!(index < NVCT_MAX_NUM_ENTRIES)) {
        return NVCT_INDEX_OUT_OF_BOUND;
    }

    // Read the entry from storage.
    _nvct_entry_t entry = {};
    if (read_mram(mram_offset(table_offset, entry_offset(index)), TABLE_ENTRY_SIZE, (uint8_t*)&entry) != MRAM_OK) {
        return NVCT_FAILED_TO_READ_FROM_MRAM_ERROR;
    }

    // Check the CRC.
    if (calculate_crc(entry.config, entry.value) == entry.crc) {
        *value = entry.value;
        return NVCT_SUCCESS;
    } else {
        return NVCT_CRC_ERROR;
    }
}

/**
 * @brief Creates an NVCT at a table index by writing the firmware version entry.
 * @param[in] table_index The table index to write the value to.
 * @param[in] firmware_v The firmware version to write.
 * @retval NVCT_SUCCESS if the value was written and verified.
 * @retval NVCT_INDEX_OUT_OF_BOUND if the desired NVCT index is invalid.
 * @retval NVCT_TABLE_INDEX_ERROR if an invalid table index was given.
 */
nvct_err_enum_t provision_new_table(uint32_t table_index, firmware_version_t firmware_v) {
    if (!(table_index < NVCT_MAX_NUM_TABLES)) {
        return NVCT_TABLE_INDEX_ERROR;
    }
    return provision_nvct_value(table_index, false, FIRMWARE_VERSION_NVCT_INDEX, firmware_v);
}

/**
 * @brief Writes an NVCT value into storage for the first time.
 * @param[in] table_index The table index to write the value to.
 * @param[in] mutable Whether or not the entry should be marked mutable.
 * @param[in] index The index within the table for the NVCT entry.
 * @param[in] value The value to write.
 * @retval NVCT_SUCCESS if the value was written and verified.
 * @retval NVCT_INDEX_OUT_OF_BOUND if the desired NVCT index is invalid.
 * @retval NVCT_TABLE_INDEX_ERROR if an invalid table index was given.
 */
nvct_err_enum_t provision_nvct_value(uint32_t table_index, bool mutable, uint8_t index, nvct_value_t value) {
    if (!(table_index < NVCT_MAX_NUM_TABLES)) {
        return NVCT_TABLE_INDEX_ERROR;
    }
    if (!(index < NVCT_MAX_NUM_ENTRIES)) {
        return NVCT_INDEX_OUT_OF_BOUND;
    }

    _nvct_entry_t entry = create_nvct_entry(mutable, value);
    return write_to_store(table_index, index, entry);
}

/**
 * @brief Erases an entire NVCT table.
 * @param[in] table_index The table index to erase.
 * @retval NVCT_SUCCESS if the value was written and verified.
 * @retval NVCT_TABLE_INDEX_ERROR if the desired NVCT index is invalid.
 * @retval NVCT_FAILED_TO_WRITE_TO_MRAM_ERROR if MRAM errors were seen.
 */
nvct_err_enum_t erase_nvct_table(uint32_t table_index) {
    if (!(table_index < NVCT_MAX_NUM_TABLES)) {
        return NVCT_TABLE_INDEX_ERROR;
    }

    // If we are erasing the table we are currently using, invalidate the
    // index so that we won't try to access a deleted table in the future.
    if ((table_offset_valid == true) && (table_index == table_offset)) {
        table_offset_valid = false;
        set_uint32_from_internal("nvct_table", 99);
    }

    uint32_t idx                           = 0;
    bool had_mram_errors                   = false;
    uint8_t erased_entry[TABLE_ENTRY_SIZE] = {0};
    memset(erased_entry, 0xFF, TABLE_ENTRY_SIZE); // Erased MRAM has value 0xFF.

    // Try to write the erased entry to each possible slot in the table.
    // Keep going if writes fail since a bad write still probably means an invalidated table.
    for (idx = 0; idx < NVCT_MAX_NUM_ENTRIES; idx++) {
        uint32_t addr = mram_offset(table_index, TABLE_ENTRY_SIZE * idx);
        if (write_mram(addr, TABLE_ENTRY_SIZE, (const uint8_t*)&erased_entry) != MRAM_OK) {
            had_mram_errors = true;
        }
    }
    return !had_mram_errors ? NVCT_SUCCESS : NVCT_FAILED_TO_WRITE_TO_MRAM_ERROR;
}
