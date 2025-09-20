/**
 * @file obc_settings.c
 * @brief Settings. See @ref obc_settings.h for more information.
 * @author Richard A
 * @date Apr 23, 2020
 */

#include "obc_settings.h"
#include <string.h>
#include "settings.h"
#include "rtos.h"
#include "logger.h"
#include "nvct.h"

// Private function prototypes
static setting_err_t set_uint32(const char *setting_name, uint32_t value, bool check_mutable);
static int32_t setting_exists(const char *setting_name);
static bool set_value_ok(setting_t *setting, uint32_t val);
static bool increment_revision_count(void);

// Variables
SemaphoreHandle_t settings_mutex;
StaticSemaphore_t settings_mutex_buf;

/**
 * @brief Index of the revision count setting.
 * This is grabbed at initialization in order to prevent the
 * need to look it up every time a setting is changed.
 */
int32_t revision_count_index = -1;

/**
 * @brief Reads settings from NVCT, updating the values in RAM if necessary.
 * Values are updated if the NVCT contains a different value than the firmware
 * does, and the NVCT state tracking information for each setting is also updated.
 *
 * @note This should be called at startup, and right after a new NVCT is provisioned.
 */
void update_settings_from_nvct(void) {
    if (!settings_mutex) {
        settings_mutex = xSemaphoreCreateRecursiveMutexStatic(&settings_mutex_buf);
    }

    // Find the index of the revision count setting.
    revision_count_index = setting_exists("revision");

    if (xSemaphoreTakeRecursive(settings_mutex, portMAX_DELAY)) {
        // Find the correct NVCT for this firmware version so we can update settings from it.
        uint32_t fw_version = 0;
        get_uint32_from_internal("fw_version", &fw_version);
        nvct_err_enum_t nvct_err = init_nvct(fw_version);

        // Update each setting from NVCT.
        if (nvct_err == NVCT_SUCCESS) {
            uint32_t i = 0;

            for (i = 0; i < num_settings; i++) {
                if (settings[i].nvct == USE_NVCT) {
                    uint32_t val_in_nvct         = 0;
                    nvct_err_enum_t get_nvct_err = get_nvct_value(settings[i].nvct_idx, &val_in_nvct);

                    if (get_nvct_err == NVCT_SUCCESS) {
                        // Update the setting in RAM if NVCT contains a different value.
                        setting_nvct_t result = NVCT_SYNCED;

                        if (settings[i].setting_value != val_in_nvct) {
                            settings[i].setting_value = val_in_nvct;

                            // Call the setting callback if there is one.
                            if (settings[i].set_callback != NULL) {
                                result = (settings[i].set_callback(val_in_nvct) == true) ? NVCT_SYNCED : NVCT_UPDATE_CALLBACK_FAIL;
                            }
                        }

                        settings[i].nvct = result;
                    } else {
                        // Setting was not loaded properly from NVCT, log errors and mark as NVCT_INVALID.
                        if (get_nvct_err == NVCT_CRC_ERROR) {
                            // TODO Will we ever use this module?
                            //log_str(ERROR, LOG_SETTINGS, "%s CRC err", settings[i].name);
                        } else {
                            // TODO Will we ever use this module?
                            //log_str(ERROR, LOG_SETTINGS, "%s NVCT err: %d", settings[i].name, get_nvct_err);
                        }

                        settings[i].nvct = NVCT_INVALID;
                    }
                }
            }
        } else {
            // TODO Will we ever use this module?
            //log_str(ERROR, LOG_SETTINGS, "No NVCT: %d", nvct_err);
        }

        xSemaphoreGiveRecursive(settings_mutex);
    }
}

/**
 * @brief Sets a uint32_t setting from firmware.
 *
 * @param[in] setting_name The name of the setting to set.
 * @param[in] value The value to give the setting.
 * @retval SETTING_OK if everything succeeded.
 * @retval SETTING_NVCT_UPDATE_ERR if the setting or the revision count not be updated in NVCT. Note: a callback
 * failure may have occurred in addition to the failure to update the NVCT.
 * @retval SETTING_CALLBACK_FAILED if the setting was set in NVCT or doesn't use NVCT, but the callback failed.
 * @retval INVALID_SETTING_NAME, INVALID_SETTING_VALUE, SETTING_IMMUTABLE, SETTING_MUTEX_ERR, SETTING_NULL_ERR for other errors.
 */
setting_err_t set_uint32_from_internal(const char *setting_name, uint32_t value) {
    return set_uint32(setting_name, value, false);
}

/**
 * @brief Sets a uint32_t setting from a command. This checks the mutability of the
 * setting before attempting to set it, as not all settings can be set from commands.
 *
 * @warning only use this for commands! Use @ref set_uint32_from_internal for adjusting settings within firmware.
 *
 * @param[in] setting_name The name of the setting to set.
 * @param[in] value The value to give the setting.
 * @retval SETTING_OK if everything succeeded.
 * @retval SETTING_NVCT_UPDATE_ERR if the setting or the revision count not be updated in NVCT. Note: a callback
 * failure may have occurred in addition to the failure to update the NVCT.
 * @retval SETTING_CALLBACK_FAILED if the setting was set in NVCT or doesn't use NVCT, but the callback failed.
 * @retval INVALID_SETTING_NAME, INVALID_SETTING_VALUE, SETTING_IMMUTABLE, SETTING_MUTEX_ERR, SETTING_NULL_ERR for other errors.
 */
setting_err_t set_uint32_from_command(const char *setting_name, uint32_t value) {
    return set_uint32(setting_name, value, true);
}

/**
 * @brief Gets the value of a uint32_t setting.
 *
 * @param[in] setting_name The name of the setting to get.
 * @param[out] value The value of the setting, if @ref SETTING_OK is returned. Unchanged otherwise.
 * @return SETTING_OK or an error code.
 */
setting_err_t get_uint32_from_internal(const char *setting_name, uint32_t *value) {
    if ((setting_name == NULL) || (value == NULL)) {
        return SETTING_NULL_ERR;
    }

    int32_t idx = setting_exists(setting_name);

    if (idx != -1) {
        *value = settings[idx].setting_value;
    } else {
        return INVALID_SETTING_NAME;
    }

    return SETTING_OK;
}

/**
 * @brief Gets the value of a uint32_t setting and the NVCT status of the setting.
 *
 * @param[in] setting_name The name of the setting to get.
 * @param[out] value The value of the setting, if @ref SETTING_OK is returned. Unchanged otherwise.
 * @param[out] nvct_info The NVCT information from the setting, if @ref SETTING_OK is returned. Unchanged otherwise.
 * @return SETTING_OK or an error code.
 */
setting_err_t get_uint32_from_command(const char *setting_name, uint32_t *value, setting_nvct_t *nvct_info) {
    if ((setting_name == NULL) || (value == NULL)) {
        return SETTING_NULL_ERR;
    }

    int32_t idx = setting_exists(setting_name);

    if (idx != -1) {
        *value     = settings[idx].setting_value;
        *nvct_info = settings[idx].nvct;
    } else {
        return INVALID_SETTING_NAME;
    }

    return SETTING_OK;
}

/**
 * @brief Provisions a settings table in NVCT.
 *
 * The settings table is written to the desired table index, and then all
 * settings are read back from NVCT to ensure that their NVCT state tracking
 * information is updated after the write.
 *
 * @param[in] table_index the index of the NVCT to write the settings table to.
 * @retval SETTING_OK the settings were written to the new table.
 * @retval SETTING_NVCT_UPDATE_ERR an NVCT error caused table provisioning to fail.
 * @retval INVALID_SETTING_NAME the FW version setting somehow could not be found.
 */
setting_err_t provision_new_settings_table(uint32_t table_index) {
    uint32_t firmware_version = 0;
    setting_err_t ret         = get_uint32_from_internal("fw_version", &firmware_version);

    if (ret == SETTING_OK) {
        // Provision the table (writes the fw_version setting).
        nvct_err_enum_t nvct_err = provision_new_table(table_index, firmware_version);

        // Write each setting that is supposed to be in NVCT. Bail out if writing
        // to NVCT fails, or if initial provisioning above failed.
        // Skip firmware version setting since we already wrote it with @ref provision_new_table();
        uint8_t i = 0;

        for (i = FIRMWARE_VERSION_NVCT_INDEX + 1; i < num_settings; i++) {
            if ((settings[i].nvct != NOT_NVCT) && (nvct_err == NVCT_SUCCESS)) {
                // Split into variables to keep MISRA checker happy (MISRA checker bug).
                bool mutable = settings[i].mutable;
                uint32_t idx = settings[i].nvct_idx;
                uint32_t val = settings[i].setting_value;
                nvct_err     = provision_nvct_value(table_index, mutable, idx, val);
            } else if (nvct_err != NVCT_SUCCESS) {
                // An error was seen. Stop trying to write settings to NVCT.
                break;
            } else {
                // We encountered a non-NVCT setting. Skip it and keep going.
            }
        }

        // Handle errors.
        if (nvct_err != NVCT_SUCCESS) {
            // TODO Will we ever use this module?
            //log_str(ERROR, LOG_SETTINGS, "Provision NVCT err: %d %d", nvct_err, i);
            ret = SETTING_NVCT_UPDATE_ERR;
        }

        // Re-initialize all settings and their NVCT state information by updating settings
        // from NVCT.
        update_settings_from_nvct();
    } else {
        // TODO Will we ever use this module?
        //log_str(ERROR, LOG_SETTINGS, "No FW version");
        ret = INVALID_SETTING_NAME;
    }

    return ret;
}

//--------- Private Functions -----------
/**
 * @brief Sets a uint32_t setting. The setting will be updated if the desired
 * value is within the setting's range and @c check_mutable is set appropriately
 * for the setting.
 *
 * @param[in] setting_name The name of the setting to set.
 * @param[in] value The value to give the setting.
 * @param[in] check_mutable Only set mutable settings. For firmware modules updating
 * settings internally, this can be @c false. For commands from operators, this must
 * be @true to ensure operators can't update immutable settings.
 *
 * @retval SETTING_OK if everything succeeded.
 * @retval SETTING_NVCT_UPDATE_ERR if the setting or the revision count not be updated in NVCT. Note: a callback
 * failure may have occurred in addition to the failure to update the NVCT.
 * @retval SETTING_CALLBACK_FAILED if the setting was set in NVCT or doesn't use NVCT, but the callback failed.
 * @retval INVALID_SETTING_NAME, INVALID_SETTING_VALUE, SETTING_IMMUTABLE, SETTING_MUTEX_ERR, SETTING_NULL_ERR for other errors.
 */
static setting_err_t set_uint32(const char *setting_name, uint32_t value, bool check_mutable) {
    if (setting_name == NULL) {
        return SETTING_NULL_ERR;
    }

    int32_t idx = setting_exists(setting_name);

    if (idx == -1) {
        return INVALID_SETTING_NAME;
    }

    // Stop if the setting is immutable.
    if (check_mutable && (settings[idx].mutable == false)) {
        return SETTING_IMMUTABLE;
    }

    // Check the value and set the setting if it's within range.
    if (set_value_ok(&settings[idx], value) == false) {
        return INVALID_SETTING_VALUE;
    }

    if (xSemaphoreTakeRecursive(settings_mutex, portMAX_DELAY)) {
        // Update the in-RAM value of the setting.
        settings[idx].setting_value = value;

        // If the setting has a set callback, call it to notify any
        // modules that the setting has been updated.
        bool callback_ok = true;

        if (settings[idx].set_callback) {
            callback_ok = settings[idx].set_callback(value);
        }

        // Update the value in NVCT if the setting is backed up there.
        bool nvct_ok = true;

        if (settings[idx].nvct != NOT_NVCT) {
            nvct_err_enum_t nvct_err = set_nvct_value(settings[idx].mutable, idx, value);

            if (nvct_err == NVCT_SUCCESS) {
                nvct_ok = increment_revision_count();
            } else {
                nvct_ok = false;
                // TODO Will we ever use this module?
                //log_str(ERROR, LOG_SETTINGS, "%s NVCT err: %d", settings[idx].name, nvct_err);
            }
        }

        xSemaphoreGiveRecursive(settings_mutex);
        // Failure to write to NVCT is worse than a callback failure, so prioritize
        // that error even if we saw callback failure too.
        return nvct_ok ? (callback_ok ? SETTING_OK : SETTING_CALLBACK_FAILED) : SETTING_NVCT_UPDATE_ERR;
    } else {
        return SETTING_MUTEX_ERR;
    }
}

/**
 * @brief Determines whether or not a desired value is allowable for a setting based on the
 * setting's configuration.
 * @param setting[in] The setting reference to check min/max ranges for.
 * @param val[in] The value of the setting to check.
 * @returns true if the value is within the allowable ranges for the setting, false otherwise.
 */
static bool set_value_ok(setting_t *setting, uint32_t val) {
    return (val <= setting->max) && (val >= setting->min);
}

/**
 * @brief Finds a setting by name and returns the index of the setting.
 * @param[in] setting_name The name of the setting.
 * @return -1 if the setting is not found. Otherwise, the index into the settings table.
 */
static int32_t setting_exists(const char *setting_name) {
    if (setting_name == NULL) {
        return -1;
    }

    uint32_t i = 0;

    for (i = 0; i < num_settings; i++) {
        if (strcmp(settings[i].name, setting_name) == 0) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief Increments the revision setting that indicates how many settings
 * table revisions have been made.
 * @retval True if writing the increment value to NVCT succeeded.
 * @retval False if writing the increment value to NVCT failed or there was an issue with the revision count setting.
 */
static bool increment_revision_count(void) {
    bool ok = false;

    if ((revision_count_index != -1) && ((uint32_t)revision_count_index < num_settings)) {
        setting_t *revision_setting = &settings[revision_count_index];
        revision_setting->setting_value++;

        // If revision count is 0 after the increment, we rolled over. Log it.
        if (revision_setting->setting_value == 0) {
            LOG_SYSTEM__REVISION_ROLLOVER();
        }

        // Update revision count setting in NVCT.
        nvct_err_enum_t nvct_err = set_nvct_value(revision_setting->mutable, revision_setting->nvct_idx, revision_setting->setting_value);

        if (nvct_err == NVCT_SUCCESS) {
            ok = true;
        } else {
            // TODO Will we ever use this module?
            //log_str(ERROR, LOG_SETTINGS, "Revision %d NVCT err: %d", revision_setting->setting_value, nvct_err);
            ok = false;
        }
    }

    return ok;
}
