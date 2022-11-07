/**
 * @file settings_defs.h
 * @brief Typdefs for OBC settings. See @ref obc_settings.h for more information.
 * @author Richard A
 * @date Apr 25, 2020
 */

#ifndef SETTINGS_DEFS_H_
#define SETTINGS_DEFS_H_

/**
 * @brief An enum that is used to track the NVCT handling of each setting.
 *
 * @warning Be careful when updating this enum. Settings that are saved to NVCT
 * can have a value of anything except NOT_NVCT. The != NOT_NVCT logic is used
 * to distinguish NVCT settings vs. non-NVCT settings, so if a state is being
 * added that is possible to have on a setting that will never be in NVCT, all
 * of that logic would need to be updated.
 */
typedef enum {
    /**
     * @brief This setting is not stored in NVCT,
     * and should not be retrieved from NVCT.
     */
    NOT_NVCT = 0,

    /**
     * @brief This setting is stored in NVCT.
     */
    USE_NVCT,

    /**
     * @brief This setting was found in NVCT and
     * was synced.
     */
    NVCT_SYNCED,

    /**
     * @brief This setting cannot be retrieved from NVCT. The
     * default value will be used.
     */
    NVCT_INVALID,

    /**
     * @brief The setting's callback failed during the initial
     * update from NVCT. This likely indicates a programming error.
     */
    NVCT_UPDATE_CALLBACK_FAIL
} setting_nvct_t;

/**
 * @brief A setting.
 */
typedef struct {
    const char* name;        // The string name of the setting.
    const char* group;       // The string name of the setting's group.
    uint32_t setting_value;  // The value of the setting. This is the default unless the setting is updated from NVCT.
    const bool mutable;      // If true, the setting is mutable (editable) by the settings API from the ground. It may still be edited by firmware.
    const uint32_t min;      // The minimum value that the setting can hold.
    const uint32_t max;      // The maximum value that the setting can hold.
    setting_nvct_t nvct;     // Whether or not the setting is stored in NVCT, information about the status of the NVCT copy of the setting.
    const uint32_t nvct_idx; // The index of the setting in the NVCT.

    /**
     * @brief Pointer to a function that takes a uint32_t with the desired value,
     * and returns a boolean indicating if the variable backing the setting could be updated.
     * This is called when settings are "set" and the desired value is within the allowable range.
     */
    bool (*set_callback)(uint32_t the_value);
} setting_t;

/**
 * @brief Location of the firmware version setting in an NVCT table.
 * @note Various loops assume that this is 0. Restructuring will need to occur
 * if it is changed.
 */
#define FIRMWARE_VERSION_NVCT_INDEX 0

#endif /* SETTINGS_DEFS_H_ */
