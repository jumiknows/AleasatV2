/**
 * @file settings.h
 * @brief Definition of the settings table.
 * @author Richard A
 * @date Apr 23, 2020
 *
 * DO NOT INCLUDE THIS FILE IN ANY MODULES EXCEPT FOR obc_settings.c
 *
 * Settings are defined in a header to allow direct access to them
 * through ceedling
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdbool.h>
#include "sys_common.h"
#include "settings_defs.h"

/**
 * @brief Mutable setting callback.
 * Mutable settings must always provide a callback that is called once the setting
 * is updated from the ground. This is the default callback, suitable for modules
 * that don't need to know when their settings have been changed.
 * @param[in] val Unused, but must be present for API compatibility.
 * @returns set_return_val.
 */
bool default_true_callback(uint32_t val) {
    return true;
}

/**
 * @brief Maximum value a uint32_t can hold.
 */
#define UINT32_T_MAX 4294967295u

/**
 * @brief Value used to indciate that no NVCT is present for this firmware version.
 */
#define NO_NVCT_FOR_FW 99

/**
 * @brief The settings table.
 *
 * See the OBC manual for details of each setting: https://docs.google.com/document/d/1MCSHc5eJ06q6t-leqKfpSGKTcZXLg9m1STLkumOHDPQ/edit#
 *
 * Settings must follow these rules:
 *  1. Setting names must be unique.
 *  2. If .mutable == true, a callback must be defined. @ref default_true_callback can be used.
 *  3. Only @c USE_NVCT or @c NOT_NVCT are permissible values for @c .nvct.
 *  4. Settings must be ordered by their NVCT indices if they use NVCT.
 *  5. If a setting does not use NVCT, its @c nvct_idx must be 0 and its ordering doesn't matter.
 *  6. The fw_version setting must be at index 0.
 */
setting_t settings[] = {
    /**
     * @brief The firmware version.
     */
    {.name = "fw_version", .group = "sys", .setting_value = 1, .mutable = false, .min = 0, .max = 1, .set_callback = NULL, .nvct = USE_NVCT, FIRMWARE_VERSION_NVCT_INDEX},
    /**
     * @brief The revision number of the NVCT. Every time changes are made to
     * settings, the revision count is incremented by firmware.
     */
    {.name = "revision", .group = "sys", .setting_value = 0, .mutable = false, .min = 0, .max = UINT32_T_MAX, .set_callback = NULL, .nvct = USE_NVCT, 1},
    /**
     * @brief A mutable (editable by command) setting for testing. Not used by
     * firmware.
     */
    {.name = "test_mutable", .group = "test", .setting_value = 12345, .mutable = true, .min = 100, .max = 123456, .set_callback = &default_true_callback, .nvct = USE_NVCT, 2},
    /**
     * @brief An immutable (editable only from within firmware) setting for testing.
     * Firmware doesn't actually edit this setting anywhere, it just needs to exist
     * to run tests on.
     */
    {.name = "test_immutable", .group = "test", .setting_value = 6789, .mutable = false, .min = 0, .max = UINT32_T_MAX, .set_callback = NULL, .nvct = USE_NVCT, 3},
    /**
     * @brief Which NVCT table index is being used by firmware.
     * When firmware finds an NVCT that matches its firmware version at boot, this
     * setting holds the index of that NVCT. This is 99 if no NVCT is found for the
     * firmware version.
     */
    {.name = "nvct_table", .group = "sys", .setting_value = NO_NVCT_FOR_FW, .mutable = false, .min = 0, .max = 99, .set_callback = NULL, .nvct = NOT_NVCT, 0}};
/**
 * @brief The number of settings.
 */
const uint32_t num_settings = (sizeof(settings) / sizeof(setting_t));
#endif
