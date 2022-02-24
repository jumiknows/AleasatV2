/**
 * @file obc_settings.h
 * @brief Settings.
 * @author Richard A
 * @date Apr 23, 2020
 *
 * DOC: https://docs.google.com/document/d/1I3yPJc34gQ8eJH_uolSMC9Pvk4jqW2TgWWPtpVroWUY
 *
 * INTRO:
 * The settings module provides a convenient API for adjusting "settings" on the OBC.
 * Settings are variables contained in the settings module. They are also optionally
 * backed by NVCT, and are used for configuring the OBC.
 *
 * The settings that are backed by NVCT are adjustable from the ground,
 * and when an adjustment occurs, the value in NVCT will be reloaded so that the
 * setting stays changed through resets. Once a setting has been updated from the
 * ground, a callback will be called to notify any relevant modules that the setting
 * has been changed.
 *
 * The settings table is contained within @ref settings.h. This defines the setting
 * names, their default values and limits, and hooks up the callbacks to the modules
 * that need to know when a setting value is updated from the ground.
 *
 * IMMUTABLE SETTINGS:
 * Settings can be defined as "settable only", or immutable. These settings can be
 * updated from within firmware, but not by commands. This is intended to allow
 * modules to report status information that may change internally, but doesn't need
 * operator control.
 *
 * COMMANDS:
 * The main goal of the settings API was to provide a consistent command set from the
 * ground to get and update settings. Commands are provided to set a setting by name,
 * to get a setting by name, and to get settings by group. A setting group is a bunch
 * of conceptually-related settings, and the group get commands allow several settings
 * to be queried easily with one command.
 *
 * INTERNAL USE:
 * Within the module that owns the setting, updates to its value must be done via
 * the settings API. This ensures that the module won't try to set an unexpected value
 * that can't also be set from the ground, since sets from the ground always go through
 * the settings API, which checks the min/max values.
 *
 * Ideally, every time a module needs a setting value, it should look up the value with
 * the settings API, just in case the value was changed by an operator. However, a
 * module can also cache a value (if looking up repeatedly would be slow), and can
 * update the cached value when the setting's set() callback is called.
 */

#ifndef OBC_SETTINGS_H_
#define OBC_SETTINGS_H_

#include "sys_common.h"
#include "obc_error.h"
#include "settings_defs.h"

// Public API
void update_settings_from_nvct(void);
setting_err_t set_uint32_from_internal(const char* setting_name, uint32_t value);
setting_err_t get_uint32_from_internal(const char* setting_name, uint32_t* value);

// Command-only API
setting_err_t provision_new_settings_table(uint32_t table_index);
setting_err_t set_uint32_from_command(const char* setting_name, uint32_t value);
setting_err_t get_uint32_from_command(const char* setting_name, uint32_t* value, setting_nvct_t* nvct_info);

#endif /* OBC_SETTINGS_H_ */
