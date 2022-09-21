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
#include "obc_hardwaredefs.h"
#include "comms_cmd.h"
#include "comms_flash.h"
#include "comms_app_image.h"
#include "comms_telem.h"
#include "obc_magnetorquer.h"
#include "imu_bmx160.h"
#include "ADIS16260_gyro.h"
#include "obc_gpio.h"
#include "tms_can.h"
#include "obc_eps.h"
#include "obc_utils.h"
#include "tms_spi.h"
#include "obc_flash.h"
#include "obc_filesystem.h"
#include "obc_gps.h"

// Logging
#include "logger.h"
#include "log_ids.h"

// Standard Library
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include "printf.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/* Testing macros */
#define ASSERT_EPS_SUCCESS(err)         if ((err) != EPS_SUCCESS) break;
#define ASSERT_VOLTAGE_HIGH(voltage)    if (((voltage) < 2.0f) || ((voltage) > 8.0f)) break; else success_counter++;
#define ASSERT_CURRENT_HIGH(current)    if (((current) < 0.010f) || ((current) > 1.0f)) break; else success_counter++;
#define ASSERT_TEMPERATURE_MEDIUM(temp) if (((temp) < 10.0f) || ((temp) > 35.0f)) break; else success_counter++;
#define ASSERT_COUNTER_LOW(counter)     if ((counter) > 100) break; else success_counter++;
#define ASSERT_VOLTAGE_LOW(voltage)     if ((voltage) > 0.5f) break; else success_counter++;
#define ASSERT_INT_EQUAL(val1, val2)    if ((val1) != (val2)) break; else success_counter++;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void prompt_cmd_response(log_level_t lvl, log_identifier_t id, bool write_to_file, char* format, ...);
static void run_panel_gyro_test(ADIS16260_t* gyro);

/* Test Functions -------------------------------------------------------------*/

/**
 * @brief Acknowledges with a single logger message.
 */
void cmd_test_ack(uint32_t arg_len, void* arg) {
    prompt_cmd_response(INFO, LOG_TEST_ACK_CMD, true, "Test Ack!");
}

/**
 * @brief Tests sending the command arguments via raw MIBSPI.
 */
void cmd_test_comms_raw(uint32_t arg_len, void* arg) {
    mibspi_err_t err;
    uint16_t data[COMMS_TG_SIZE_WORDS] = {0x0000};
    memset(data, 0xAB, sizeof(data));
    memcpy(data, arg, arg_len);
    err = comms_mibspi_tx(data);
    prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "raw tx %d", err);
}

/**
 * @brief Tests sending command, taking command line argument as params. Does not wait for response.
 *
 * If testing with Comms board, change comms_hwid
 */
void cmd_test_comms_tx_only(uint32_t arg_len, void* arg) {
    comms_err_t err;
    uint8_t arg_str_len = strlen(arg);

    err = comms_send_cmd(comms_hwid, COMMS_COMMON_MSG_ASCII, arg, arg_str_len, COMMS_MIBSPI_MUTEX_TIMEOUT_MS);

    prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "tx %d", err);
}

/**
 * @brief Tests sending commands and receiving the responses.
 *
 * If testing with Comms board, change comms_hwid
 */
void cmd_test_comms_tx_rx(uint32_t arg_len, void* arg) {
    comms_err_t err;
    comms_command_t resp = {0};

    err = comms_send_recv_cmd(comms_hwid, COMMS_RADIO_MSG_GET_TELEM, NULL, 0, &resp, COMMS_MIBSPI_MUTEX_TIMEOUT_MS);

    prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "txrx %d resp %x", err, resp.header.command);
}

/**
 * @brief Tests repeatedly sending and receiving the ack command.
 *
 * If testing with Comms board, change comms_hwid
 */
void cmd_test_comms_stress1(uint32_t arg_len, void* arg) {
    comms_err_t err;
    comms_command_t resp = {0};
    uint32_t num_success = 0;
    uint32_t num_fail = 0;
    uint32_t i;

    for (i = 0; i < 1000; ++i) {
        memset(&resp, 0, sizeof(resp));
        err = comms_send_recv_cmd(comms_hwid, COMMS_COMMON_MSG_ACK, NULL, 0, &resp, COMMS_MIBSPI_MUTEX_TIMEOUT_MS);
        if ((err != COMMS_SUCCESS) || (resp.header.command != COMMS_COMMON_MSG_ACK)) {
            num_fail++;
            if (num_fail >= 5) {
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(80));  // make it easier to see in logic analyzer waveform
        }
        else {
            num_success++;
        }
    }

    if (num_fail == 0) {
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "s1 all pass");
    }
    else {
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "s1 pass %d fail %d", num_success, num_fail);
    }
}

/**
 * @brief Tests flashing Comms with example app image in repo
 *
 * If testing with Comms board, change comms_hwid
 */
void cmd_test_comms_flash_app(uint32_t arg_len, void* arg) {
    comms_err_t err;

    err = comms_flash_image(comms_test_app_image_pages, comms_test_app_image_num_pages);
    if (err == COMMS_SUCCESS) {
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "flash pass");
    }
    else {
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "flash fail");
    }
}

/**
 * @brief Test sending reboot command to Comms and waits for radio app start message
 *
 * Will fail if Comms doesn't have an app image loaded
 *
 * If testing with Comms board, change comms_hwid
 */
void cmd_test_comms_reboot(uint32_t arg_len, void* arg) {
    comms_err_t err;
    comms_command_t msg = {0};
    comms_waiter_match_params_t app_start_match_spec = {
        .match_cmd_num_spec = COMMS_MATCH_EQUAL,
        .match_cmd_num = COMMS_RADIO_MSG_START,
        .cmd_ptr = &msg
    };

    err = comms_send_cmd(comms_hwid, COMMS_RADIO_MSG_REBOOT, NULL, 0, COMMS_MIBSPI_MUTEX_TIMEOUT_MS);
    if (err != COMMS_SUCCESS) {
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "reboot fail 1");
        return;
    }

    err = comms_wait_for_cmd(&app_start_match_spec, 2000);
    if (err != COMMS_SUCCESS) {
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "reboot fail 2");
        return;
    }

    prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "reboot pass");
}

/**
 * @brief Tests sending comms get_telem command.
 *
 * If testing with Comms board, change comms_hwid
 */
void cmd_test_comms_get_telem(uint32_t arg_len, void* arg) {
    comms_err_t err;
    comms_telem_t telem_recv = { 0 };

    err = comms_get_telem(&telem_recv);
    if (err != COMMS_SUCCESS) {
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Unable to get telemetry!");
    }
    else {
        uint8_t ind_adc;

        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-reserved: %d", telem_recv.reserved);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-uptime: %d", telem_recv.uptime);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-uart0_rx_count: %d", telem_recv.uart0_rx_count);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-uart1_rx_count: %d", telem_recv.uart1_rx_count);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-rx_mode: %d", telem_recv.rx_mode);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-tx_mode: %d", telem_recv.tx_mode);

        for (ind_adc = 0; ind_adc < COMMS_ADC_NUM_CHANNELS; ind_adc++) {
            prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-adc[%d]: %d", ind_adc, telem_recv.adc[ind_adc]);

        }

        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-rssi: %d", telem_recv.rssi);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-last_lqi: %d", telem_recv.last_lqi);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-last_frequest: %d", telem_recv.last_frequest);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-packets_sent: %d", telem_recv.packets_sent);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-cs_count: %d", telem_recv.cs_count);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-packets_good: %d", telem_recv.packets_good);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-packets_rej_cs: %d", telem_recv.packets_rejected_checksum);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-packets_rej_rsv: %d", telem_recv.packets_rejected_reserved);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-packets_rej_oth: %d", telem_recv.packets_rejected_other);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-reserved0: %d", telem_recv.reserved0);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-reserved1: %d", telem_recv.reserved1);
        prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-custom0: %d", telem_recv.custom0);
    	prompt_cmd_response(INFO, LOG_TEST_COMMS_CMD, false, "Telem-custom1: %d", telem_recv.custom1);
    }
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
       prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Error Provide Arguments");
    } else {
        if(strcmp((char*)arg, "1") == 0) {
            mag_selected = magnetorquer_1;
            prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Magnetorquer 1 Selected");
        } else if(strcmp((char*)arg, "2") == 0) {
            mag_selected = magnetorquer_2;
            prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Magnetorquer 1 Selected");
        } else if(strcmp((char*)arg, "3") == 0) {
            mag_selected = magnetorquer_3;
            prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Magnetorquer 1 Selected");
        } else {
            prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Unknown Param");
            return;
        }

        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Magnetorquer Test Starting");

        vTaskDelay(100);

        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Magnetorquer Starting");
        
        mag_set_dir(&mag_selected, FORWARD);
        mag_start(&mag_selected);
        
        for (i=0; i < 100; i++) {
            mag_set_duty(&mag_selected, i); //set magnetorquer duty
            vTaskDelay(10);
        }

        mag_set_duty(&mag_selected, 0); //set magnetorquer duty to 0
        mag_stop(&mag_selected);

        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Magnetorquer Stopped");

        mag_set_dir(&mag_selected, BACKWARD);

        vTaskDelay(100);

        mag_start(&mag_selected);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Magnetorquer Started in Reverse");

        for (i=0; i < 100; i++) {
            mag_set_duty(&mag_selected, i); //set magnetorquer duty
            vTaskDelay(10);
        }

        mag_stop(&mag_selected);

        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Test Completed");
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
    bmx160_data_t datap = { 0 };

    if (bmx160_enable_imu(&bmx160_imu_1) == IMU_SUCCESS) {
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "IMU initialized!");
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    if (bmx160_get_data(&bmx160_imu_1, &datap) ==  IMU_SUCCESS) {
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "mag_x: %f ",datap.mag.x_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "mag_y: %f ",datap.mag.y_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "mag_z: %f ",datap.mag.z_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "accel_x: %f ",datap.accel.x_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "accel_y: %f ",datap.accel.y_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "accel_z: %f ",datap.accel.z_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "gyro_x: %f ",datap.gyro.x_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "gyro_y: %f ",datap.gyro.y_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "gyro_z: %f ",datap.gyro.z_proc);

    } else {
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "first data packet query failed");
    }

    bmx160_set_low_power(&bmx160_imu_1);
    vTaskDelay(pdMS_TO_TICKS(100));
    bmx160_wake_up(&bmx160_imu_1);

    vTaskDelay(pdMS_TO_TICKS(100));
    prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "AFTER POWER DOWN");

    if (bmx160_get_data(&bmx160_imu_1, &datap) ==  IMU_SUCCESS) {
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "mag_x: %f ",datap.mag.x_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "mag_y: %f ",datap.mag.y_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "mag_z: %f ",datap.mag.z_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "accel_x: %f ",datap.accel.x_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "accel_y: %f ",datap.accel.y_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "accel_z: %f ",datap.accel.z_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "gyro_x: %f ",datap.gyro.x_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "gyro_y: %f ",datap.gyro.y_proc);
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "gyro_z: %f ",datap.gyro.z_proc);

    } else {
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "second data packet query failed");
    }
    
    if (bmx160_disable_imu(&bmx160_imu_1) == IMU_SUCCESS) {
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "IMU disabled");
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

    char* args[3] = {NULL, NULL, NULL};
    uint8_t num_args    = obc_cmd_read_str_arguments(arg, 3, args);

    // Check number of arguments
    if (num_args < 3) {
        prompt_cmd_response(INFO, LOG_TEST_CAN_GPIO_CMD, false,
            "Wrong # arguments: %d",
			num_args
        );
        return;
    }

    // Parse arguments
    int32_t port_idx = cseq_to_num(args[0]) - 1;
    if (port_idx == -2) { // -2 because we're subtracting 1 from the output
    	log_str(ERROR, LOG_TEST_CAN_GPIO_CMD, true, "Invalid arg %s", args[0]);
        return;
    }
    int32_t pin = cseq_to_num(args[1]);
    if (pin == -1) {
    	log_str(ERROR, LOG_TEST_CAN_GPIO_CMD, true, "Invalid arg %s", args[1]);
        return;
    }
    int32_t value = cseq_to_num(args[2]);
    if (value == -1) {
        log_str(ERROR, LOG_TEST_CAN_GPIO_CMD, true, "Invalid arg %s", args[2]);
        return;
    }

    // Validate arguments
    if (port_idx >= 3) {
        prompt_cmd_response(INFO, LOG_TEST_CAN_GPIO_CMD, false,
            "Invalid Port: %d",
            (port_idx + 1)
        );
        return;
    }
    if ((pin != CAN_PIN_RX) && (pin != CAN_PIN_TX)) {
        prompt_cmd_response(INFO, LOG_TEST_CAN_GPIO_CMD, false,
            "Invalid Pin: %d",
            pin
        );
        return;
    }

    prompt_cmd_response(INFO, LOG_TEST_CAN_GPIO_CMD, false,
        "Writing %d to CAN%d %s",
        (bool)value,
        (port_idx + 1),
        (pin == CAN_PIN_RX ? "RX" : "TX")
    );

    obc_gpio_write(CAN_PORTS[port_idx], pin, value);
}

/**
 * @brief Run brief sanity test on specified gyro and report value
 * @param[in] arg_len Size of argument array (number of arguments * ARGUMENT_SIZE)
 * @param[in] arg     Arguments (gyro number)
 */
void cmd_test_panel_gyro(uint32_t arg_len, void* arg){

    static ADIS16260_t* const GYROS[] = {&panel_gyro_0, &panel_gyro_1, &panel_gyro_2, &panel_gyro_3};
    int32_t gyro_idx = cseq_to_num((char*)arg);
    if (gyro_idx == -1) {
    	log_str(ERROR, LOG_ADCS_GENERAL, true, "Invalid arg %s", (char*)arg);
    	return;
    }
    if (gyro_idx > 3)
    {
        prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "ERROR Unknown Param");
        return;
    }

    prompt_cmd_response(INFO, LOG_ADCS_GENERAL, true, "Gyro %d Selected", gyro_idx);

    run_panel_gyro_test(GYROS[gyro_idx]);
}

/**
 * @brief Tests the EPS I2C interface: eps_read_float function
 * @brief Sends a test read command to the EPS module
 * @param[in] arg_len Size of argument array (number of arguments * ARGUMENT_SIZE)
 * @param[in] arg     arg[0] read_cmd  :  1,2,3,...,51,52
 */
void cmd_test_eps_read_float(uint32_t arg_len, void* arg) {
    char* args[1] = {NULL};
    uint8_t num_args    = obc_cmd_read_str_arguments(arg, 1, args);

    // Check number of arguments
    if (num_args != 1) {
        prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false,
            "Wrong # arguments: %d, expected 1",
            num_args
        );
        return;
    }

    uint8_t read_cmd = (uint8_t) cseq_to_num(args[0]);

    // Validate arguments
    if ((read_cmd < 1) || (read_cmd > 52)) {
        prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false,
            "Invalid read command '%d', must be between 1 and 52",
            read_cmd
        );
        return;
    }

    float32 data = 0;
    eps_err_t err = eps_read_float((eps_cmd_read_float_t) read_cmd, &data);

    prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false, "tx: %d, data: %f", err, data);
}

/**
 * @brief Tests the EPS I2C interface: eps_read_float_raw function
 * @brief Sends a test read command to the EPS module
 * @param[in] arg_len Size of argument array (number of arguments * ARGUMENT_SIZE)
 * @param[in] arg     arg[0] read_cmd  :  1,2,3,...,51,52
 */
void cmd_test_eps_read_float_raw(uint32_t arg_len, void* arg) {
    char* args[1] = {NULL};
    uint8_t num_args    = obc_cmd_read_str_arguments(arg, 1, args);

    // Check number of arguments
    if (num_args != 1) {
        prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false,
            "Wrong # arguments: %d, expected 1",
            num_args
        );
        return;
    }

    uint8_t read_cmd = (uint8_t) cseq_to_num(args[0]);

    // Validate arguments
    if ((read_cmd < 1) || (read_cmd > 52)) {
        prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false,
            "Invalid read command '%d', must be between 1 and 52",
            read_cmd
        );
        return;
    }

    uint16_t data = 0;
    eps_err_t err = eps_read_float_raw((eps_cmd_read_float_t) read_cmd, &data);

    prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false, "tx: %d, data: %f", err, data);
}

/**
 * @brief Tests the EPS I2C interface: eps_read_int function
 * @brief Sends a test read command to the EPS module
 * @param[in] arg_len Size of argument array (number of arguments * ARGUMENT_SIZE)
 * @param[in] arg     arg[0] read_cmd  :  1,2,3,...,51,52
 */
void cmd_test_eps_read_int(uint32_t arg_len, void* arg) {
    char* args[1] = {NULL};
    uint8_t num_args    = obc_cmd_read_str_arguments(arg, 1, args);

    // Check number of arguments
    if (num_args != 1) {
        prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false,
            "Wrong # arguments: %d, expected 1",
            num_args
        );
        return;
    }

    uint8_t read_cmd = (uint8_t) cseq_to_num(args[0]);

    // Validate arguments
    if ((read_cmd < 1) || (read_cmd > 52)) {
        prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false,
            "Invalid read command '%d', must be between 1 and 52",
            read_cmd
        );
        return;
    }

    uint16_t data = 0;
    eps_err_t err = eps_read_int((eps_cmd_read_int_t) read_cmd, &data);

    prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false, "tx: %d, data: %d", err, data);
}

/**
 * @brief Sends a test write command to the EPS module
 * @param[in] arg_len Size of argument array (number of arguments * ARGUMENT_SIZE)
 * @param[in] arg     arg[0] write_cmd  : 0,1,2,...,17,18
 *                    arg[1] state      : 0,1,2,3
 */
void cmd_test_eps_write(uint32_t arg_len, void* arg) {
    char* args[2] = {NULL};
    uint8_t num_args    = obc_cmd_read_str_arguments(arg, 2, args);

    // Check number of arguments
    if (num_args < 2) {
        prompt_cmd_response(INFO, TEST_EPS_WRITE_CMD, false,
            "Wrong # arguments: %d, expected 2",
            num_args
        );
        return;
    }

    uint8_t write_cmd = (uint8_t) cseq_to_num(args[0]);
    uint8_t state     = (uint8_t) cseq_to_num(args[1]);

    // Validate arguments
    //   no need to validate write command, as it can be any non-negative integer
    if (state > 3) {
        prompt_cmd_response(INFO, TEST_EPS_WRITE_CMD, false,
            "Invalid write state '%d', must be one of 0,1,2,3",
            state
        );
        return;
    }

    eps_err_t err = eps_write_state((eps_cmd_write_state_t) write_cmd, (eps_write_state_t) state);

    prompt_cmd_response(INFO, TEST_EPS_WRITE_CMD, false, "tx: %d", err);
}

/**
 * @brief Perform a series of READ operations on the EPS module, and sanity check their responses
 * @param[in] arg_len Size of argument array (number of arguments * ARGUMENT_SIZE)
 * @param[in] arg     No arguments
 */
void cmd_test_eps_read_sanity(uint32_t arg_len, void* arg) {
    eps_err_t err = EPS_SUCCESS;
    bool success = false;
    uint8_t success_counter = 0; // Incremented for every succeeded test
    float32 retf = -1.0f;
    uint16_t reti = 9999U;

    // Run test suite: return on failure
    do {
        OBC_MISRA_CHECK_OFF /* False positive: MISRA-12.4/R: The right-hand operand of a logical && or || operator shall not contain side effects. */

        // Read battery voltage
        err = eps_read_float(EPS_READ_BATTERY_VOLTAGE, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_VOLTAGE_HIGH(retf)
        retf = -1.0f;

        // Read battery current
        err = eps_read_float(EPS_READ_BATTERY_CURRENT, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_CURRENT_HIGH(retf)
        retf = -1.0f;

        // Read BCR voltage
        err = eps_read_float(EPS_READ_BCR_VOLTAGE, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_VOLTAGE_HIGH(retf)
        retf = -1.0f;

        // Read BCR current
        err = eps_read_float(EPS_READ_BCR_CURRENT, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_CURRENT_HIGH(retf)
        retf = -1.0f;

        // Read 3V3 bus current
        err = eps_read_float(EPS_READ_BUS_3V3_CURRENT, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_CURRENT_HIGH(retf)
        retf = -1.0f;

        // Read 5V bus current
        err = eps_read_float(EPS_READ_BUS_5V_CURRENT, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_CURRENT_HIGH(retf)
        retf = -1.0f;

        // Read 3V3 LUP voltage
        err = eps_read_float(EPS_READ_BUS_3V3_VOLTAGE, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_VOLTAGE_HIGH(retf)
        retf = -1.0f;

        // Read 5V LUP voltage
        err = eps_read_float(EPS_READ_BUS_5V_VOLTAGE, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_VOLTAGE_HIGH(retf)
        retf = -1.0f;

        // Read 3V3 LUP voltage
        err = eps_read_float(EPS_READ_LUP_3V3, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_VOLTAGE_HIGH(retf)
        retf = -1.0f;

        // Read 5V LUP voltage
        err = eps_read_float(EPS_READ_LUP_5V, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_VOLTAGE_HIGH(retf)
        retf = -1.0f;

        // Read MCU temperature
        err = eps_read_float(EPS_READ_MCU_TEMP, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_TEMPERATURE_MEDIUM(retf)
        retf = -1.0f;

        // Read battery temp 1
        err = eps_read_float(EPS_READ_BATTERY_TEMP_C1, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_TEMPERATURE_MEDIUM(retf)
        retf = -1.0f;

        // Read battery temp 2
        err = eps_read_float(EPS_READ_BATTERY_TEMP_C2, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_TEMPERATURE_MEDIUM(retf)
        retf = -1.0f;

        // Read battery temp 3
        err = eps_read_float(EPS_READ_BATTERY_TEMP_C3, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_TEMPERATURE_MEDIUM(retf)
        retf = -1.0f;

        // Read battery temp 4
        err = eps_read_float(EPS_READ_BATTERY_TEMP_C4, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_TEMPERATURE_MEDIUM(retf)
        retf = -1.0f;

        // Read Power On Cycles counter
        err = eps_read_int(EPS_READ_POWER_ON_CYCLES, &reti);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_COUNTER_LOW(reti)
        reti = 9999U;

        // Read V Under Voltage counter
        err = eps_read_int(EPS_READ_V_UNDER_VOLTAGE, &reti);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_COUNTER_LOW(reti)
        reti = 9999U;

        // Read V Short Circuit counter
        err = eps_read_int(EPS_READ_V_SHORT_CIRCUIT, &reti);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_COUNTER_LOW(reti)
        reti = 9999U;

        // Read V Over Temperature counter
        err = eps_read_int(EPS_READ_V_OVER_TEMPERATURE, &reti);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_COUNTER_LOW(reti)
        reti = 9999U;

        success = true; // Every test passed

        OBC_MISRA_CHECK_ON
    } while (0);

    if (success) {
        prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false, "PASS: eps read sanity, %d", success_counter);
    } else {
        prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false, "FAIL: eps read sanity, %d", success_counter);
    }
}

/**
 * @brief Perform a series of EPS tests involving writing and reading to the MCU
 * @param[in] arg_len Size of argument array (number of arguments * ARGUMENT_SIZE)
 * @param[in] arg     No arguments
 */
void cmd_test_eps_writeread_sanity(uint32_t arg_len, void* arg) {
    eps_err_t err = EPS_SUCCESS;
    bool success = false;
    uint8_t success_counter = 0; // Incremented for every succeeded test
    float32 retf = -1.0f;

    // Run test suite: return on failure
    do {
        OBC_MISRA_CHECK_OFF /* False positive: MISRA-12.4/R: The right-hand operand of a logical && or || operator shall not contain side effects. */

        /*
         * Test Vbatt en
         */
        // Read battery voltage
        err = eps_read_float(EPS_READ_BATTERY_VOLTAGE, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_VOLTAGE_HIGH(retf)
        retf = -1.0f;

        // Disable raw battery output
        err = eps_write_state(EPS_WRITE_V_BATT_EN, AUTO_OFF);
        ASSERT_EPS_SUCCESS(err)

        // Read battery voltage again
        err = eps_read_float(EPS_READ_BATTERY_VOLTAGE, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_VOLTAGE_LOW(retf)
        retf = -1.0f;

        // Enable raw battery output
        err = eps_write_state(EPS_WRITE_V_BATT_EN, AUTO_ON);
        ASSERT_EPS_SUCCESS(err)

        // Read battery voltage again again
        err = eps_read_float(EPS_READ_BATTERY_VOLTAGE, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_VOLTAGE_HIGH(retf)
        retf = -1.0f;

        /*
         * Test changing Rbat raw initial
         */
        err = eps_write_initial_resistance(0x30);
        ASSERT_EPS_SUCCESS(err)

        err = eps_read_float(EPS_READ_RBAT_INITIAL, &retf);
        ASSERT_EPS_SUCCESS(err)
        ASSERT_INT_EQUAL((uint8_t) (retf / 1.497), 0x30)

        success = true; // Every test passed

        OBC_MISRA_CHECK_ON
    } while (0);

    if (success) {
        prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false, "PASS: eps wr sanity, %d", success_counter);
    } else {
        prompt_cmd_response(INFO, TEST_EPS_READ_CMD, false, "FAIL: eps wr sanity, %d", success_counter);
    }
}

/**
 * @brief Erases full chip, then performs a 1KB chunk write, read, and comparison
 * 
 * @warning Erases the FULL EEPROM memory!
 * 
 * @param[in] arg_len size of argument array (number of arguments * ARGUMENT_SIZE)
 * @param[in] arg     arg[0] addr  : 32-bit address to be tested in hex format
 * @param[in] arg     arg[1] len   : length of test data to write-read, in bytes. Must be <= 1024
 */
void cmd_test_flash_rw(uint32_t arg_len, void* arg) {
    static uint8_t read_data[1024];
    static uint8_t write_data[1024];
    for (uint32_t i = 0; i < sizeof(write_data); i++) {
        write_data[i] = i % 256;
        read_data[i] = 0; // clear any old read data
    }

    char* args[2] = {NULL};
    uint8_t num_args    = obc_cmd_read_str_arguments(arg, 2, args);

    // Check number of arguments
    if (num_args != 2) {
        prompt_cmd_response(INFO, LOG_TEST_FLASH_RW_CMD, false,
            "Wrong # arguments: %d, expected 2",
            num_args
        );
        return;
    }

    // Parse hexadecimal addr, and decimal len
    const uint32_t addr = (uint32_t) strtol(args[0], NULL, 16);
    const uint32_t len  = (uint32_t) strtol(args[1], NULL, 10);

    if (len > sizeof(write_data)) {
        prompt_cmd_response(INFO, LOG_TEST_FLASH_RW_CMD, false,
            "Bad len: %d > %d",
            len,
            sizeof(write_data)
        );
        return;
    }

    prompt_cmd_response(INFO, LOG_TEST_FLASH_RW_CMD, false, "Performing rw test at 0x%08x for %d bytes", addr, len);

    flash_err_t erase_err = flash_erase(addr, FULL_CHIP);
    flash_err_t write_err = flash_write(addr, len, write_data);
    flash_err_t read_err = flash_read(addr, len, read_data);

    /*
     * WARNING: do not change these messages, as hill tests depend on them!
     */
    prompt_cmd_response(INFO, LOG_TEST_FLASH_RW_CMD, false, "E: %d, W: %d, R: %d", erase_err, write_err, read_err);
    if (0 == memcmp(write_data, read_data, len)) {
        prompt_cmd_response(INFO, LOG_TEST_FLASH_RW_CMD, false, "Read and write data match:  PASS");
    } else {
        prompt_cmd_response(ERROR, LOG_TEST_FLASH_RW_CMD, false, "Read and write data do not match:  FAIL");
    }
}

/**
 * @brief Executes the *destructive* filesystem self test
 * 
 * @warning Reformats the FS, losing all data!
 * 
 * @param[in] arg_len unused
 * @param[in] arg     unused
 */
void cmd_test_fs(uint32_t arg_len, void* arg) {
    // First, deinit the FS
    fs_deinit();

    // Run the test
    fs_err_t err = fs_self_test();

    // Finally, reinit the FS
    fs_init();

    if (err == FS_OK) {
        prompt_cmd_response(INFO, LOG_TEST_FILESYSTEM_CMD, false, "FS test:  PASS");
    } else {
        prompt_cmd_response(ERROR, LOG_TEST_FILESYSTEM_CMD, false, "FS test:  FAIL, status %d", (int32_t) err);
    }
}

void cmd_restart_gps(uint32_t arg_len, void* arg) {
    gps_err_t err = gps_restart_receiver(GPS_RST_COLD_START);
    switch (err) {
        case GPS_SUCCESS:
            prompt_cmd_response(INFO, LOG_GPS, false, "gps restarted!");
            break;
        default:
            prompt_cmd_response(ERROR, LOG_GPS, false, "gps unexpected error while restart!");
            break;
    }
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

/**
 * @brief Runs testing routine on given panel gyro
 *
 * @param[in] ADIS16260_t* gyro: Gyro device test target
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
        prompt_cmd_response(INFO, LOG_ADCS_GYRO, true, "ID is %d ", whoami.serial_num);

        /* Read gyro data */
        result = ADIS16260_read_gyro(gyro, &gyro_data);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        prompt_cmd_response(INFO, LOG_ADCS_GYRO, true, "Computed rate %f", gyro_data.data);

        /* Put gyro to sleep for 500ms*/
        result = ADIS16260_set_sleep(gyro, 1);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        prompt_cmd_response(INFO, LOG_ADCS_GYRO, true, "Sleeping...");
        vTaskDelay(pdMS_TO_TICKS(1000));

        /* Read on-board temperature sensor*/
        result = ADIS16260_read_temp(gyro, &temp_data);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        prompt_cmd_response(INFO, LOG_ADCS_GYRO, true, "Temp is %f", temp_data.data_temp);

        /* Set internal filter */
        result = ADIS16260_set_filter(gyro, ADIS16260_RANGE_160, ADIS16260_BW_330, 2);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        prompt_cmd_response(INFO, LOG_ADCS_GYRO, true, "Set filter to 330hz, 2 taps");

        /* Read data with new filter setting */
        result = ADIS16260_read_gyro(gyro, &gyro_data);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        prompt_cmd_response(INFO, LOG_ADCS_GYRO, true, "Rate is %f", gyro_data.data);

        /* Set filter settings back to default */
        result = ADIS16260_set_filter(gyro, ADIS16260_RANGE_320, ADIS16260_BW_50, 2);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        prompt_cmd_response(INFO, LOG_ADCS_GYRO, true, "Set filter to default");

        /* Read gyro rate one last time*/
        result = ADIS16260_read_gyro(gyro, &gyro_data);
        if (result != ADIS16260_SUCCESS)
        {
            break;
        }
        prompt_cmd_response(INFO, LOG_ADCS_GYRO, true, "Rate is %f", gyro_data.data);

    } while(0);

    /* If there was an error anywhere, report this to console */
    if (result != ADIS16260_SUCCESS)
    {
        prompt_cmd_response(INFO, LOG_ADCS_GYRO, true, "ERROR -> %d <-", result);
    }
}
