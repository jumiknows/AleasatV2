/**
 * @file cmd_impl.c
 * @brief Implementation of commands
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// OBC
#include "obc_time.h"
#include "obc_rtc.h"
#include "low_power.h"
#include "obc_time.h"
#include "obc_watchdog.h"
#include "obc_gpio.h"
#include "obc_heartbeat.h"
#include "obc_gps.h"
#include "obc_featuredefs.h"
#include "fw_structs.h"
#include "fw_memmap.h"
#include "backup_epoch.h"

// Utils
#include "obc_utils.h"

// TMS570
#include "tms_i2c.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

cmd_sys_resp_code_t cmd_impl_RESET(const cmd_sys_cmd_t *cmd) {
    CPU_SOFT_RESET();
    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_PING(const cmd_sys_cmd_t *cmd) {
    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_GET_TIME(const cmd_sys_cmd_t *cmd, cmd_GET_TIME_resp_t *resp) {
    epoch_t epoch = rtc_get_epoch_time();

    if (epoch != -1) {
        resp->timestamp = epoch;
        return CMD_SYS_RESP_CODE_SUCCESS;
    } else {
        return CMD_SYS_RESP_CODE_ERROR;
    }
}

cmd_sys_resp_code_t cmd_impl_SET_TIME(const cmd_sys_cmd_t *cmd, cmd_SET_TIME_args_t *args) {
    real_time_t set_time = { 0 };
    bool conv_success = epoch_to_real_time(args->timestamp, &set_time);

    if (!conv_success) {
        return CMD_SYS_RESP_CODE_ERROR;
    }

    rtc_err_t err = rtc_set_current_time(&set_time);

    if (err == RTC_SUCCESS) {
        return CMD_SYS_RESP_CODE_SUCCESS;
    } else {
        return CMD_SYS_RESP_CODE_ERROR;
    }
}

cmd_sys_resp_code_t cmd_impl_LOW_POWER(const cmd_sys_cmd_t *cmd, cmd_LOW_POWER_args_t *args) {
    if (args->enable) {
        idle_sleep_on();
    } else {
        idle_sleep_off();
    }

    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_SYS_TIME(const cmd_sys_cmd_t *cmd, cmd_SYS_TIME_resp_t *resp) {
    resp->sys_time_us = SYSTEM_TIME_US();
    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_CPU_USAGE(const cmd_sys_cmd_t *cmd, cmd_CPU_USAGE_resp_t *resp) {
    resp->sys_time_us = SYSTEM_TIME_US();
    resp->idle_time_us = portRUNTIME_TO_US(xTaskGetIdleRunTimeCounter());
    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_GPIO_EXP_RESET(const cmd_sys_cmd_t *cmd) {
    gpio_expander_reset();
    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_I2C_RESET(const cmd_sys_cmd_t *cmd) {
    tms_i2c_reset(0, 500);
    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_HEARTBEAT(const cmd_sys_cmd_t *cmd, cmd_HEARTBEAT_args_t *args) {
#ifdef FEATURE_HEARTBEAT

    if (args->enable) {
        start_heartbeat();
    } else {
        suspend_heartbeat();
    }

    return CMD_SYS_RESP_CODE_SUCCESS;
#else
    return CMD_SYS_RESP_CODE_NOT_IMPL;
#endif
}

cmd_sys_resp_code_t cmd_impl_GET_UPTIME(const cmd_sys_cmd_t *cmd, cmd_GET_UPTIME_resp_t *resp) {
    resp->seconds = get_uptime();
    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_FW_INFO(const cmd_sys_cmd_t *cmd, cmd_FW_INFO_resp_t *resp) {
    // Copy the FW structs object onto the stack so every access isn't volatile (and later ECC errors
    // only have to be handled once).
    const fw_structs_t fw_structs = *(FW_STRUCTS[CFG_FLASH_SLOT]);

    memcpy(resp->fw_version, fw_structs.info.version, sizeof(fw_structs.info.version));
    resp->fw_hash       = fw_structs.info.githash;
    resp->flash_address = fw_structs.entrypoint.flash_addr;
    resp->platform      = fw_structs.info.platform;
    resp->target        = fw_structs.info.target;
    resp->flags         = fw_structs.info.flags;
    resp->size          = fw_structs.header.size;
    resp->crc32         = fw_structs.header.crc32;

    return CMD_SYS_RESP_CODE_SUCCESS;
}
