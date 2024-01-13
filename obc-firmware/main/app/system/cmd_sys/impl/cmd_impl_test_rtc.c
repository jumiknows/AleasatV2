/**
 * @file cmd_impl_test_rtc.c
 * @brief Implementation of RTC test commands
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// OBC
#include "pca2129_rtc.h"
#include "obc_rtc.h"
#include "logger.h"
#include "obc_watchdog.h"

// TMS570
#include "tms_mibspi.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static volatile bool alarm_irq_received = false; 

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void alarm_isr(void *args);
static void delay_seconds_watchdog_friendly(uint8_t n);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

cmd_sys_resp_code_t cmd_impl_TEST_RTC_GET_TIME(
    const cmd_sys_cmd_t *cmd, 
    cmd_TEST_RTC_GET_TIME_args_t *args,
    cmd_TEST_RTC_GET_TIME_resp_t *resp
) {
    real_time_t cur_time, init_time;
    epoch_t cur_epoch, init_epoch;

    init_time = alea_time_init;

    if (rtc_set_current_time(&init_time) != RTC_SUCCESS) {
        return CMD_SYS_RESP_CODE_ERROR;
    }
    
    delay_seconds_watchdog_friendly(args->duration_s);

    if (rtc_get_current_time(&cur_time) != RTC_SUCCESS) {
        return CMD_SYS_RESP_CODE_ERROR;
    }
    // convert times to epoch for easy comparison
    init_epoch = real_time_to_epoch(&init_time);
    cur_epoch = real_time_to_epoch(&cur_time);

    if (cur_epoch != init_epoch + args->duration_s) {
        return CMD_SYS_RESP_CODE_ERROR;
    }
    
    resp->second    = cur_time.second;
    resp->minute    = cur_time.minute;
    resp->hour      = cur_time.hour;
    resp->day       = cur_time.day;
    resp->month     = cur_time.month;
    resp->year      = cur_time.year;
    
    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_TEST_RTC_ALARM_N_SEC(
    const cmd_sys_cmd_t *cmd, 
    cmd_TEST_RTC_ALARM_N_SEC_args_t *args
) {
    real_time_t init_time;
    epoch_t init_time_epoch;

    // 0. Clear alarm flag
    alarm_irq_received = false;

    // 1. Read current time
    if (rtc_get_current_time(&init_time) != RTC_SUCCESS) {
        LOG_PRINTF("Err curr time");
        return CMD_SYS_RESP_CODE_ERROR;
    }

    // 2. Set alarm [args->duration_s] seconds in the future. Minimum duration_s is 1 second; cannot set alarm for current second.
    init_time_epoch = real_time_to_epoch(&init_time);
    if (args->duration_s < 1) {
        args->duration_s = 1;
        LOG_PRINTF("Test duration set to minimum (alarm 1 sec in the future)");
    }
    if (rtc_set_absolute_alarm((uint32_t) (init_time_epoch + args->duration_s), (rtc_alarm_cb_t) &alarm_isr, NULL) != RTC_SUCCESS) {
        LOG_PRINTF("Failed to set alarm");
        return CMD_SYS_RESP_CODE_ERROR;
    }

    // 3. Wait [args->duration_s]+ seconds
    delay_seconds_watchdog_friendly(args->duration_s + 1);

    // 4. Check whether alarm flag has been set
    if (!alarm_irq_received) {
        LOG_PRINTF("Alarm not triggered on time");
        return CMD_SYS_RESP_CODE_ERROR;
    }

    // 5. Clear alarm flag
    alarm_irq_received = false;

    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_TEST_RTC_CAP_AND_GET_TIMESTAMP(
    const cmd_sys_cmd_t *cmd, 
    cmd_TEST_RTC_CAP_AND_GET_TIMESTAMP_args_t *args,
    cmd_TEST_RTC_CAP_AND_GET_TIMESTAMP_resp_t *resp
) {
    real_time_t init_time;
    real_time_t ts_time;

    // 1. Get initial time to compare delayed timestamp with
    if (rtc_get_current_time(&init_time) != RTC_SUCCESS) {
        LOG_PRINTF("Err curr time read");
        return CMD_SYS_RESP_CODE_ERROR;
    }

    // 2. Delay [args->duration_s] seconds
    delay_seconds_watchdog_friendly(args->duration_s);

    // 3. Capture and read timestamp
    if (rtc_capture_timestamp(&ts_time) != RTC_SUCCESS) {
        LOG_PRINTF("Err timestamp capture and read");
        return CMD_SYS_RESP_CODE_ERROR;
    }

    resp->init_second    = init_time.second;
    resp->init_minute    = init_time.minute;
    resp->init_hour      = init_time.hour;
    resp->init_day       = init_time.day;
    resp->init_month     = init_time.month;
    resp->init_year      = init_time.year;

    resp->ts_second    = ts_time.second;
    resp->ts_minute    = ts_time.minute;
    resp->ts_hour      = ts_time.hour;
    resp->ts_day       = ts_time.day;
    resp->ts_month     = ts_time.month;
    resp->ts_year      = ts_time.year;

    // 4. Assert that the timestamp is exactly [args->duration_s] seconds after initial time
    if (real_time_to_epoch(&ts_time) != real_time_to_epoch(&init_time) + args->duration_s) {
        LOG_PRINTF("Incorrect timestamp");
        return CMD_SYS_RESP_CODE_ERROR;
    }

    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_DEBUG_READ_RTC_REG(
    const cmd_sys_cmd_t *cmd, 
    cmd_DEBUG_READ_RTC_REG_args_t *args, 
    cmd_DEBUG_READ_RTC_REG_resp_t *resp
) {
    pca2129_debug_read_single_register(args->reg, &(resp->val));
    return CMD_SYS_RESP_CODE_SUCCESS;
}

cmd_sys_resp_code_t cmd_impl_DEBUG_WRITE_RTC_REG(
    const cmd_sys_cmd_t *cmd, 
    cmd_DEBUG_WRITE_RTC_REG_args_t *args,
    cmd_DEBUG_WRITE_RTC_REG_resp_t *resp
) {
    pca2129_debug_write_single_register(args->reg, &(args->val));
    pca2129_debug_read_single_register(args->reg, &(resp->val_after_write));
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Set the @ref alarm_irq_received flag.
*/
static void alarm_isr(void *args) {
    alarm_irq_received = true;
}

/**
 * @brief Wait @ref n seconds in 1-second increments, petting the watchdog each time.
 */
static void delay_seconds_watchdog_friendly(uint8_t n) {
    for (uint8_t i = 0; i < n; i++) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        obc_watchdog_pet(OBC_TASK_ID_CMD_SYS_EXEC);
    }
}
