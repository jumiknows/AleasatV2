/**
 * @file test_commands.c
 * @brief Test commands not to be used in actual flight software
 * 
 * Steps to implement a test command.
 * 1. Add a test struct to the command_table on obc_cmd.c within
 * the SYSTEM_TEST ifndef preprocessor.
 * 2. Add your test function to this file and the header.
 * 3. Add a new log_ids for your test that begins with `TEST_`
 * @see https://docs.google.com/spreadsheets/d/1chvy-mGuV6XV1QmsMAaXOuLLI7Z635QQu-AfypoBUl0/edit
 * 4. Run log_sys_update.py
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "test_commands.h"

// OBC
#include "obc_cmd.h"
#include "obc_comms.h"
#include "obc_magnetorquer.h"
#include "imu_bmx160.h"
#include "obc_gpio.h"
#include "tms_can.h"
#include "obc_utils.h"
#include "tms_spi.h"

// Logging
#include "logger.h"
#include "log_ids.h"

// Standard Library
#include <stdint.h>
#include <stdarg.h>
#include "printf.h"

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void prompt_cmd_response(log_level_t lvl, log_identifier_t id, bool write_to_file, char* format, ...);


/* Test Functions -------------------------------------------------------------*/

/**
 * @brief Acknowledges with a single logger message.
 */
void cmd_test_ack(uint32_t arg_len, void* arg) {
    prompt_cmd_response(INFO, TEST_ACK_CMD, true, "Test Ack!");
}

/**
 * @brief Will send the command arguments to the COMMS board.
 */
void cmd_test_comms_spi(uint32_t arg_len, void* arg) {
    mibspi_err_t err;
    uint16_t data[128] = {0x0000};
    memcpy(data, arg, arg_len);
    if (xSemaphoreTake(xMibspiCommsMutexHandle, pdMS_TO_TICKS(COMMS_MUTEX_TIMEOUT_MS))) {
        err = comms_mibspi_tx(&data);
        xSemaphoreGive(xMibspiCommsMutexHandle);
    }
    prompt_cmd_response(INFO, TEST_COMMS_CMD, false, "tx %d", err);
}

/**
 * @brief Runs a basic test loop over selected magnetorquer.
 *
 * Outputs whole range from 0 to 100 duty in both foreward and backward directions from selected magnetorquer channel
 * 
 * Takes one, 1 character arg:
 *      1: Run test on Magnetorquer 1
 *      2: Run test on Magnetorquer 2
 *      3: Run test on Magnetorquer 3
 * 
 */
void cmd_test_mag(uint32_t arg_len, void* arg) {
    uint8_t i;
    mag_t mag_selected;

    if (arg_len == 0) {
       prompt_cmd_response(INFO, ADCS_LOG, true, "Error Provide Arguments");
    } else {
        if(strcmp((char*)arg, "1") == 0) {
            mag_selected = magnetorquer_1;
            prompt_cmd_response(INFO, ADCS_LOG, true, "Magnetorquer 1 Selected");
        } else if(strcmp((char*)arg, "2") == 0) {
            mag_selected = magnetorquer_2;
            prompt_cmd_response(INFO, ADCS_LOG, true, "Magnetorquer 1 Selected");
        } else if(strcmp((char*)arg, "3") == 0) {
            mag_selected = magnetorquer_3;
            prompt_cmd_response(INFO, ADCS_LOG, true, "Magnetorquer 1 Selected");
        } else {
            prompt_cmd_response(INFO, ADCS_LOG, true, "Unknown Param");
            return;
        }

        prompt_cmd_response(INFO, ADCS_LOG, true, "Magnetorquer Test Starting");

        vTaskDelay(100);

        prompt_cmd_response(INFO, ADCS_LOG, true, "Magnetorquer Starting");
        
        mag_set_dir(&mag_selected, FORWARD);
        mag_start(&mag_selected);
        
        for (i=0; i < 100; i++) {
            mag_set_duty(&mag_selected, i); //set magnetorquer duty
            vTaskDelay(10);
        }

        mag_set_duty(&mag_selected, 0); //set magnetorquer duty to 0
        mag_stop(&mag_selected);

        prompt_cmd_response(INFO, ADCS_LOG, true, "Magnetorquer Stopped");

        mag_set_dir(&mag_selected, BACKWARD);

        vTaskDelay(100);

        mag_start(&mag_selected);
        prompt_cmd_response(INFO, ADCS_LOG, true, "Magnetorquer Started in Reverse");

        for (i=0; i < 100; i++) {
            mag_set_duty(&mag_selected, i); //set magnetorquer duty
            vTaskDelay(10);
        }

        mag_stop(&mag_selected);

        prompt_cmd_response(INFO, ADCS_LOG, true, "Test Completed");
    }
}

/**
 * @brief Runs a basic test loop over magnetorquers 1, 2 and 3 sumultaniously.
 *
 * Outputs 20% duty from all magnetorquer PWM channels
 * 
 * Takes no arguments
 */
void cmd_test_mag_all(uint32_t arg_len, void* arg) {
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
}

/**
 * @brief Runs tests on basic sleep and power on functionality for magnetorquer hardware.
 * 
 *  Step 1: Enables magnetorquer power
 *  Step 2: Takes each magnetorquer out of sleep mode
 *  Step 3: Disables magnetorquer power
 *  Step 4: Sleeps all magnetorquers
 * 
 * Takes no arguments
 * 
 */
void cmd_test_mag_init(uint32_t arg_len, void* arg) {
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
}

/**
 * @brief Command reads data from imu, runs power cycle, then reads again.
 *
 * Can be used to test basic IMU functionality
*/
void cmd_imu_test(uint32_t arg_len, void* arg) {
    bmx160_data_t datap;

    if (bmx160_enable_imu(&bmx160_imu_1) == IMU_SUCCESS) {
        prompt_cmd_response(INFO, ADCS_LOG, true, "IMU initialized!");
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    if (bmx160_get_data(&bmx160_imu_1, &datap) ==  IMU_SUCCESS) {
        prompt_cmd_response(INFO, ADCS_LOG, true, "mag_x: %f ",datap.mag.x_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "mag_y: %f ",datap.mag.y_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "mag_z: %f ",datap.mag.z_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "accel_x: %f ",datap.accel.x_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "accel_y: %f ",datap.accel.y_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "accel_z: %f ",datap.accel.z_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "gyro_x: %f ",datap.gyro.x_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "gyro_y: %f ",datap.gyro.y_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "gyro_z: %f ",datap.gyro.z_proc);

    } else {
        prompt_cmd_response(INFO, ADCS_LOG, true, "first data packet query failed");
    }

    bmx160_set_low_power(&bmx160_imu_1);
    vTaskDelay(pdMS_TO_TICKS(100));
    bmx160_wake_up(&bmx160_imu_1);

    vTaskDelay(pdMS_TO_TICKS(100));
    prompt_cmd_response(INFO, ADCS_LOG, true, "AFTER POWER DOWN");

    if (bmx160_get_data(&bmx160_imu_1, &datap) ==  IMU_SUCCESS) {
        prompt_cmd_response(INFO, ADCS_LOG, true, "mag_x: %f ",datap.mag.x_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "mag_y: %f ",datap.mag.y_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "mag_z: %f ",datap.mag.z_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "accel_x: %f ",datap.accel.x_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "accel_y: %f ",datap.accel.y_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "accel_z: %f ",datap.accel.z_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "gyro_x: %f ",datap.gyro.x_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "gyro_y: %f ",datap.gyro.y_proc);
        prompt_cmd_response(INFO, ADCS_LOG, true, "gyro_z: %f ",datap.gyro.z_proc);

    } else {
        prompt_cmd_response(INFO, ADCS_LOG, true, "second data packet query failed");
    }
    
    if (bmx160_disable_imu(&bmx160_imu_1) == IMU_SUCCESS) {
        prompt_cmd_response(INFO, ADCS_LOG, true, "IMU disabled");
    }
}

/**
 * @brief Write a value to a CAN pin as a GPIO
 * @param[in] arg_len Size of argument array (number of arguments * ARGUMENT_SIZE)
 * @param[in] arg     arg[0] port  : 1,2,3
 *                    arg[1] pin   : 0,1
 *                    arg[2] value : 0,1
 */
void cmd_test_can_gpio(uint32_t arg_len, void* arg) {
    static gpio_port_t const CAN_PORTS[] = {CAN_PORT(canREG1), CAN_PORT(canREG2), CAN_PORT(canREG3)};

    // Check number of arguments
    uint32_t arg_count = num_args(arg_len);
    if (arg_count < 3) {
        prompt_cmd_response(INFO, TEST_CAN_GPIO_CMD, false,
            "Wrong # arguments: %d",
            arg_count
        );
        return;
    }

    // Parse arguments
    cmd_argument_t* args = (cmd_argument_t*)arg;
    uint32_t port_idx = cseq_to_num(args[0]) - 1;
    uint32_t pin = cseq_to_num(args[1]);
    uint32_t value = cseq_to_num(args[2]);

    // Validate arguments
    if (port_idx >= 3) {
        prompt_cmd_response(INFO, TEST_CAN_GPIO_CMD, false,
            "Invalid Port: %d",
            (port_idx + 1)
        );
        return;
    }
    if ((pin != CAN_PIN_RX) && (pin != CAN_PIN_TX)) {
        prompt_cmd_response(INFO, TEST_CAN_GPIO_CMD, false,
            "Invalid Pin: %d",
            pin
        );
        return;
    }

    prompt_cmd_response(INFO, TEST_CAN_GPIO_CMD, false,
        "Writing %d to CAN%d %s",
        (bool)value,
        (port_idx + 1),
        (pin == CAN_PIN_RX ? "RX" : "TX")
    );

    obc_gpio_write(CAN_PORTS[port_idx], pin, value);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Logs the response to a prompt command. The details of the
 * response are saved so that they can be queried with the last_response
 * command.
 *
 * @param[in] lvl           The logging system level
 * @param[in] id            The log ID
 * @param[in] write_to_file Whether or not to log the data to a file
 * @param[in] format        Printf-style format string
 * @param[in] ...           Arguments for format specification
 */
static void prompt_cmd_response(log_level_t lvl, log_identifier_t id, bool write_to_file, char* format, ...) {
    char string[MAX_PAYLOAD_SIZE + 1] = "temp";
    va_list va;
    OBC_MISRA_CHECK_OFF
    va_start(va, format);
    OBC_MISRA_CHECK_ON
    orca_vsnprintf(string, MAX_PAYLOAD_SIZE + 1, format, va);
    va_end(va);
    log_str(lvl, id, write_to_file, "%s", string);
}
