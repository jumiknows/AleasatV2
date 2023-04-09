/**
 * @file cmd_impl_test_adcs.c
 * @brief Implementation of test commands related to ADCS
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// ADCS
#include "obc_magnetorquer.h"
#include "imu_bmx160.h"
#include "ADIS16260_gyro.h"

// OBC
#include "logger.h"

// Utils
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void run_mag_test(mag_t* mag);
static void run_panel_gyro_test(ADIS16260_t* gyro);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Runs a basic test loop over selected magnetorquer.
 *
 * Outputs whole range from 0 to 100 duty in both foreward and backward directions from selected magnetorquer channel
 */
cmd_sys_resp_code_t cmd_impl_TEST_MAG(const cmd_sys_cmd_t *cmd, cmd_TEST_MAG_args_t *args) {
    static mag_t * const MAGS[3] = { &magnetorquer_1, &magnetorquer_2, &magnetorquer_3 };

    if (args->magnetorquer >= LEN(MAGS)) {
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__INVALID_MAG_INDEX, sizeof(args->magnetorquer), &(args->magnetorquer));
        return CMD_SYS_RESP_CODE_ERROR;
    }

    log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_SELECTED, sizeof(args->magnetorquer), &(args->magnetorquer));

    run_mag_test(MAGS[args->magnetorquer]);

    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Runs a basic test loop over magnetorquers 1, 2 and 3 sumultaniously.
 *
 * Outputs 20% duty from all magnetorquer PWM channels
 */
cmd_sys_resp_code_t cmd_impl_TEST_MAG_ALL(const cmd_sys_cmd_t *cmd) {
    mag_stop(&magnetorquer_1); 
    mag_stop(&magnetorquer_2);
    mag_stop(&magnetorquer_3);

    mag_set_dir(&magnetorquer_1, FORWARD);
    mag_set_dir(&magnetorquer_2, FORWARD);
    mag_set_dir(&magnetorquer_3, FORWARD);

    mag_start(&magnetorquer_1);
    mag_start(&magnetorquer_2);
    mag_start(&magnetorquer_3);

    mag_set_duty(&magnetorquer_1, 20); 
    mag_set_duty(&magnetorquer_2, 20); 
    mag_set_duty(&magnetorquer_3, 20); 

    vTaskDelay(100);

    mag_stop(&magnetorquer_1);
    mag_stop(&magnetorquer_2);
    mag_stop(&magnetorquer_3);

    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Runs tests on basic sleep and power on functionality for magnetorquer hardware.
 * 
 *  Step 1: Enables magnetorquer power
 *  Step 2: Takes each magnetorquer out of sleep mode
 *  Step 3: Disables magnetorquer power
 *  Step 4: Sleeps all magnetorquers
 */
cmd_sys_resp_code_t cmd_impl_TEST_MAG_INIT(const cmd_sys_cmd_t *cmd) {
    mag_power_en();

    vTaskDelay(500);

    mag_set_sleep(&magnetorquer_1, SLEEP_OFF);
    mag_set_sleep(&magnetorquer_2, SLEEP_OFF);
    mag_set_sleep(&magnetorquer_3, SLEEP_OFF);

    vTaskDelay(500);

    mag_power_dis();

    vTaskDelay(500);

    mag_set_sleep(&magnetorquer_1, SLEEP_ON);
    mag_set_sleep(&magnetorquer_2, SLEEP_ON);
    mag_set_sleep(&magnetorquer_3, SLEEP_ON);

    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Command reads data from imu, runs power cycle, then reads again.
 *
 * Can be used to test basic IMU functionality
*/
cmd_sys_resp_code_t cmd_impl_TEST_IMU(const cmd_sys_cmd_t *cmd) {
    bmx160_data_t datap = { 0 };

    if (bmx160_enable_imu(&bmx160_imu_1) == IMU_SUCCESS) {
        log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__IMU_INIT);
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    if (bmx160_get_data(&bmx160_imu_1, &datap) ==  IMU_SUCCESS) {
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_X_VALUE_0,   sizeof(datap.mag.x_proc),   &(datap.mag.x_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_Y_VALUE_0,   sizeof(datap.mag.y_proc),   &(datap.mag.y_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_Z_VALUE_0,   sizeof(datap.mag.z_proc),   &(datap.mag.z_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__ACCEL_X_VALUE_0, sizeof(datap.accel.x_proc), &(datap.accel.x_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__ACCEL_Y_VALUE_0, sizeof(datap.accel.y_proc), &(datap.accel.y_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__ACCEL_Z_VALUE_0, sizeof(datap.accel.z_proc), &(datap.accel.z_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_X_VALUE_0,  sizeof(datap.gyro.x_proc),  &(datap.gyro.x_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_Y_VALUE_0,  sizeof(datap.gyro.y_proc),  &(datap.gyro.y_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_Z_VALUE_0,  sizeof(datap.gyro.z_proc),  &(datap.gyro.z_proc));
    } else {
        log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__FIRST_DATA_PACKET_QUERY_FAILED);
    }

    bmx160_set_low_power(&bmx160_imu_1);
    vTaskDelay(pdMS_TO_TICKS(100));
    bmx160_wake_up(&bmx160_imu_1);

    vTaskDelay(pdMS_TO_TICKS(100));
    log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__AFTER_POWER_DOWN);

    if (bmx160_get_data(&bmx160_imu_1, &datap) ==  IMU_SUCCESS) {
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_X_VALUE_1,   sizeof(datap.mag.x_proc),   &(datap.mag.x_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_Y_VALUE_1,   sizeof(datap.mag.y_proc),   &(datap.mag.y_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_Z_VALUE_1,   sizeof(datap.mag.z_proc),   &(datap.mag.z_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__ACCEL_X_VALUE_1, sizeof(datap.accel.x_proc), &(datap.accel.x_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__ACCEL_Y_VALUE_1, sizeof(datap.accel.y_proc), &(datap.accel.y_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__ACCEL_Z_VALUE_1, sizeof(datap.accel.z_proc), &(datap.accel.z_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_X_VALUE_1,  sizeof(datap.gyro.x_proc),  &(datap.gyro.x_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_Y_VALUE_1,  sizeof(datap.gyro.y_proc),  &(datap.gyro.y_proc));
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_Z_VALUE_1,  sizeof(datap.gyro.z_proc),  &(datap.gyro.z_proc));
    } else {
        log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__SECOND_DATA_PACKET_QUERY_FAILED);
    }
    
    if (bmx160_disable_imu(&bmx160_imu_1) == IMU_SUCCESS) {
        log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__IMU_DISABLED);
    }

    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Run brief sanity test on specified gyro and report value
 */
cmd_sys_resp_code_t cmd_impl_TEST_PANEL_GYRO(const cmd_sys_cmd_t *cmd, cmd_TEST_PANEL_GYRO_args_t *args) {
    static ADIS16260_t* const GYROS[4] = {&panel_gyro_0, &panel_gyro_1, &panel_gyro_2, &panel_gyro_3};

    if (args->gyro >= LEN(GYROS)) {
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__INVALID_GYRO_INDEX, sizeof(args->gyro), &(args->gyro));
        return CMD_SYS_RESP_CODE_ERROR;
    }

    log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_SELECTED, sizeof(args->gyro), &(args->gyro));

    run_panel_gyro_test(GYROS[args->gyro]);

    return CMD_SYS_RESP_CODE_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Runs testing routine on a given magnetorquer
 * 
 * @param[in] mag Magnetorquer to test
 */
static void run_mag_test(mag_t* mag) {
    uint8_t i;

    log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_TEST_STARTING);

    vTaskDelay(100);

    log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_STARTING);
    
    mag_set_dir(mag, FORWARD);
    mag_start(mag);
    
    for (i=0; i < 100; i++) {
        mag_set_duty(mag, i); //set magnetorquer duty
        vTaskDelay(10);
    }

    mag_set_duty(mag, 0); //set magnetorquer duty to 0
    mag_stop(mag);

    log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_STOPPED);

    mag_set_dir(mag, BACKWARD);

    vTaskDelay(100);

    mag_start(mag);
    log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_STARTED_IN_REVERSE);

    for (i=0; i < 100; i++) {
        mag_set_duty(mag, i); //set magnetorquer duty
        vTaskDelay(10);
    }

    mag_stop(mag);

    log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__MAG_TEST_COMPLETED);
}

/**
 * @brief Runs testing routine on given panel gyro
 *
 * @param[in] gyro Gyro device test target
*/
static void run_panel_gyro_test(ADIS16260_t* gyro) {
    ADIS16260_error_t result = ADIS16260_SUCCESS;

    ADIS16260_ID_t whoami = {0};
    ADIS16260_gdata_t gyro_data = {0};
    ADIS16260_tdata_t temp_data = {0};

    do
    {
        /* Initialize gyro */
        result = ADIS16260_init(gyro);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }

        /* Get gyro device ID */
        result = ADIS16260_whoami(gyro, &whoami);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_ID, sizeof(whoami.serial_num), &(whoami.serial_num));

        /* Read gyro data */
        result = ADIS16260_read_gyro(gyro, &gyro_data);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_COMPUTED_RATE_0, sizeof(gyro_data.data), &(gyro_data.data));

        /* Put gyro to sleep for 500ms*/
        result = ADIS16260_set_sleep(gyro, 1);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_SLEEPING);
        vTaskDelay(pdMS_TO_TICKS(1000));

        /* Read on-board temperature sensor*/
        result = ADIS16260_read_temp(gyro, &temp_data);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_TEMP, sizeof(temp_data.data_temp), &(temp_data.data_temp));

        /* Set internal filter */
        result = ADIS16260_set_filter(gyro, ADIS16260_RANGE_160, ADIS16260_BW_330, 2);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_SET_FILTER_TO_330HZ_2TAPS);

        /* Read data with new filter setting */
        result = ADIS16260_read_gyro(gyro, &gyro_data);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_COMPUTED_RATE_1, sizeof(gyro_data.data), &(gyro_data.data));

        /* Set filter settings back to default */
        result = ADIS16260_set_filter(gyro, ADIS16260_RANGE_320, ADIS16260_BW_50, 2);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        log_signal(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_SET_FILTER_TO_DEFAULT);

        /* Read gyro rate one last time*/
        result = ADIS16260_read_gyro(gyro, &gyro_data);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_COMPUTED_RATE_2, sizeof(gyro_data.data), &(gyro_data.data));

    } while(0);

    /* If there was an error anywhere, report this to console */
    if (result != ADIS16260_SUCCESS)
    {
        log_signal_with_data(INFO, LOG_ADCS_CMD, LOG_ADCS_CMD__GYRO_ERROR, sizeof(result), &result);
    }
}
