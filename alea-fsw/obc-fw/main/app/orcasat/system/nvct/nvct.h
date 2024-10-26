/** @file nvct.h
 *   @brief Non Volatile Configuration Table
 *   @author Alireza Alidousti, Richard Arthurs
 *   @date Updated Aug 26, 2020, Created Oct 30, 2019
 *
 * NVCT stands for "non-volatile configuration table."
 * The NVCT module controls the at-rest data format of settings stored
 * in MRAM. It provides CRC checks for data integrity and gives an
 * interface to the settings module to read and write settings to MRAM.
 *
 * "Provisioning" refers to the act of writing settings to MRAM for the
 * first time.
 */

#ifndef ORCASAT_SYSTEM_NVCT_NVCT_H
#define ORCASAT_SYSTEM_NVCT_NVCT_H

#include "obc_error.h"

/** @typedef nvct_entry_t
 *   @brief nvct table entry
 */
typedef uint32_t nvct_value_t;

/** @typedef firmware_version_t
 *   @brief obc firmware version
 */
typedef uint32_t firmware_version_t;

// Setting API: for regular use
nvct_err_enum_t init_nvct(firmware_version_t firmware_v);
nvct_err_enum_t get_nvct_value(uint8_t index, nvct_value_t *value);
nvct_err_enum_t set_nvct_value(bool mutable, uint8_t index, nvct_value_t value);

// Provisioning API: for creating new tables
nvct_err_enum_t provision_new_table(uint32_t table_index, firmware_version_t firmware_v);
nvct_err_enum_t provision_nvct_value(uint32_t table_index, bool mutable, uint8_t index, nvct_value_t value);
nvct_err_enum_t erase_nvct_table(uint32_t table_index);

#endif // ORCASAT_SYSTEM_NVCT_NVCT_H
