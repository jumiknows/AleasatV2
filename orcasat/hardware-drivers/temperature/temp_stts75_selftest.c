/**
 * @file Testing system for temperature sensor.
 * @brief Holds the testing functions for the temperature sensor. Verification is logged and manual
 * right now
 * @author Julie Belleville
 * @date Sep 14, 2019 (edited Oct 10, 2019)
 */

#include "temp_stts75.h"
#include "logger.h"

static void stts75_print_temp_bytes(void);
static void stts75_print_temp_deg_c(void);
static void stts75_print_config(void);
static void stts75_print_hyst(void);
static bool stts75_test_config_rw(uint8_t desired_val);
static bool stts75_test_overtemp_rw(void);

// validation of registers
static void stts75_check_hysteresis(void);
static void stts75_check_overtemp(void);
static void stts75_check_mode(void);
static void stts75_check_resolution(void);
static void stts75_check_faults(void);
static void stts75_sanity_temp(void);

/**
 * @brief confirm that all registers are set to values expected
 */
void stts75_check_registers(void) {
    log_str(DEBUG, TEMP_LOG, false, "STTS75 Register Check");
    stts75_check_hysteresis();
    stts75_check_overtemp();
    stts75_check_mode();
    stts75_check_resolution();
    stts75_check_faults();

    stts75_sanity_temp();
}

/**
 * @brief Executes full suite of temperature sensor self tests. Logs errors in case of failure.
 * 		  Logs number of tests passed (if logging mode is INFO or more), logs success only in DEBUG
 * 		  Currently works based on manual verification
 */
void stts75_self_test(void) {
    uint8_t test_status = 0;

    log_str(DEBUG, TEMP_LOG, false, "STTS75 Self-Test");
    stts75_print_config();
    stts75_print_temp_bytes();

    // Check that the temperature is within 20-30ºC
    int16_t val = 0;
    stts75_temp_get(&val);
    if ((val >= 20) && (val <= 30)) {
        log_str(DEBUG, TEMP_LOG, false, "Temperature %d in range.", val);
        test_status++;
    } else {
        log_str(ERROR, TEMP_LOG, true, "Temperature %d out of range.", val);
    }

    // Print resolution
    stts75_resolution_t resolution = STTS75_9bit;
    stts75_resolution_get(&resolution);
    log_str(DEBUG, TEMP_LOG, false, "Resolution: %d", resolution);

    // Set and test resolution
    stts75_resolution_set(STTS75_11bit);
    stts75_resolution_get(&resolution);
    if (resolution == STTS75_11bit) {
        log_str(DEBUG, TEMP_LOG, false, "Resolution set 11-bit passed.");
        test_status++;
    } else {
        log_str(ERROR, TEMP_LOG, true, "Res set fail. des: 2 , act: %d", resolution);
    }
    stts75_resolution_set(STTS75_9bit);

    stts75_print_hyst();

    if (stts75_test_config_rw(0x08) == 1) {
        test_status++;
    }

    if (stts75_test_config_rw(0x00) == 1) {
        test_status++;
    }

    if (stts75_test_overtemp_rw() == 1) {
        test_status++;
    }
    stts75_print_temp_deg_c();

    log_str(INFO, TEMP_LOG, false, "%d/5 STTS75 tests passed.", test_status);
}

/**
 * @brief Checks that hysteresis threshold is as wanted
 * 		  Logs info if found incorrect, tries to reset it
 * 		  Logs error if still incorrect after reset attempt
 */
static void stts75_check_hysteresis(void) {
    static int16_t expected_hysteresis = 75; // default setting on startup

    int16_t val = 0;
    stts75_hysteresis_get(&val);
    if (val != expected_hysteresis) {
        log_str(INFO, TEMP_LOG, true, "Hyst bytes incorrect: %d", val);
        stts75_hysteresis_set(expected_hysteresis);

        stts75_hysteresis_get(&val);
        if (val != expected_hysteresis) {
            log_str(ERROR, TEMP_LOG, true, "Hyst bytes incorrect, not reset: %d", val);
        }
    }
}

/**
 * @brief Checks that overtemp threshold is as wanted
 * 		  Logs info if found incorrect, tries to reset it
 * 		  Logs error if still incorrect after reset attempt
 */
static void stts75_check_overtemp(void) {
    int16_t expected_overtemp = 80; // default setting on startup

    int16_t val = 0;
    stts75_overtemp_get(&val);
    if (val != expected_overtemp) {
        log_str(INFO, TEMP_LOG, true, "Overtemp bytes incorrect: %d", val);
        stts75_overtemp_set(expected_overtemp);

        stts75_overtemp_get(&val);
        if (val != expected_overtemp) {
            log_str(ERROR, TEMP_LOG, true, "Overtemp bytes incorrect, not reset: %d", val);
        }
    }
}

/**
 * @brief Checks that we are in the correct mode
 * 		  Logs info if found incorrect, tries to reset it
 * 		  Logs error if still incorrect after reset attempt
 */
static void stts75_check_mode(void) {
    stts75_mode_t expected_mode = one_shot;

    stts75_mode_t current_mode = continuous_conversion;
    stts75_mode_get(&current_mode);
    if (current_mode != expected_mode) {
        log_str(INFO, TEMP_LOG, true, "Mode incorrect, is: %d", current_mode);
        stts75_mode_set(expected_mode);

        stts75_mode_get(&current_mode);
        if (current_mode != expected_mode) {
            log_str(ERROR, TEMP_LOG, true, "Mode incorrect, not fixed, is: %d", current_mode);
        }
    }
}

/**
 * @brief Checks that we are using the correct resolution
 * 		  Logs info if found incorrect, tries to reset it
 * 		  Logs error if still incorrect after reset attempt
 */
static void stts75_check_resolution(void) {
    stts75_resolution_t expected_resolution = STTS75_9bit;

    stts75_resolution_t current_res = STTS75_11bit;
    stts75_resolution_get(&current_res);

    if (current_res != expected_resolution) {
        log_str(INFO, TEMP_LOG, true, "Res incorrect, is: %d", current_res);
        stts75_resolution_set(expected_resolution);

        stts75_resolution_get(&current_res);
        if (current_res != expected_resolution) {
            log_str(ERROR, TEMP_LOG, true, "Res incorrect, not fixed, is: %d", current_res);
        }
    }
}

/**
 * @brief Checks that we are using the correct fault tolerance
 * 		  Logs info if found incorrect, tries to reset it
 * 		  Logs error if still incorrect after reset attempt
 */
static void stts75_check_faults(void) {
    stts75_fault_t expected_fault = two_fault;

    stts75_fault_t current_fault = one_fault;
    stts75_fault_get(&current_fault);

    if (current_fault != expected_fault) {
        log_str(INFO, TEMP_LOG, true, "Faults incorrect, is: %d", current_fault);
        stts75_fault_set(expected_fault);

        stts75_fault_get(&current_fault);
        if (current_fault != expected_fault) {
            log_str(ERROR, TEMP_LOG, true, "Faults incorrect, not fixed, is: %d", current_fault);
        }
    }
}

/**
 * @brief Tests reading and writing to configs. Logs failure. Logs successes if if logging mode is
 * DEBUG
 *
 * @param[in] desired_val The value to set in the config register. 0x08 and 0x00 (default) are good.
 * This function does not validate the setting.
 *
 * @return True if read and write succeed. False otherwise
 */
static bool stts75_test_config_rw(uint8_t desired_val) {
    uint8_t config_read = 0;

    stts75_write_config_reg(desired_val);
    stts75_read_config_reg(&config_read);
    if (config_read == desired_val) {
        log_str(DEBUG, TEMP_LOG, false, "Config set 0x%02X passed.", desired_val);
        return true;
    } else {
        log_str(ERROR, TEMP_LOG, true, "Config set 0x%02X failed: 0x%02X.", desired_val, config_read);
    }
    return false;
}

/**
 * @brief Checks that the temperature read by the sensor is reasonable
 * 		  Logs an error if temperature is unreasonable
 */
static void stts75_sanity_temp(void) {
    int16_t temp = 0;
    stts75_temp_get(&temp);

    // TODO: confirm temperature ranges
    bool temp_too_low  = 0 > temp;
    bool temp_too_high = temp > 90;
    if (temp_too_low || temp_too_high) {
        log_str(ERROR, TEMP_LOG, true, "Temp outside expected bounds: %d C", temp);
    }
}

/**
 * @brief Reads temperature bytes. Logs value, if logging mode is DEBUG
 */
static void stts75_print_temp_bytes(void) {
    uint8_t data_rcv[2] = {'\0'};
    stts75_read_reg(STTS75_TEMPERATURE, data_rcv, 2);
    log_str(DEBUG, TEMP_LOG, false, "Temp bytes: 0x%02X 0x%02X", data_rcv[0], data_rcv[1]);
}

/**
 * @brief Logs the temperature in Celsius, if in DEBUG
 */
static void stts75_print_temp_deg_c(void) {
    int16_t val = 0;
    stts75_temp_get(&val);
    log_str(DEBUG, TEMP_LOG, false, "Temp: %d C", val);
}

/**
 * @brief Logs configs, if logging mode is DEBUG
 */
static void stts75_print_config(void) {
    uint8_t data_rcv = 0;
    stts75_read_config_reg(&data_rcv);
    log_str(DEBUG, TEMP_LOG, false, "Config: 0x%02X", data_rcv);
}

/**
 * @brief Logs hysteresis temperature, if logging mode is DEBUG
 */
static void stts75_print_hyst(void) {
    uint8_t data_rcv[2] = {0x00, 0x00};
    stts75_read_reg(STTS75_HYSTERESIS, data_rcv, 2);
    log_str(DEBUG, TEMP_LOG, false, "Hyst bytes: 0x%02X 0x%02X", data_rcv[0], data_rcv[1]);
}

/**
 * @brief Tests setting of overtemp registers at different values
 *
 * @return True if all tests passed, False otherwise
 */
static bool stts75_test_overtemp_rw(void) {
    int16_t val          = 0;
    uint8_t tests_passed = 0;

    // Check overtemp 70
    stts75_overtemp_set(70);
    stts75_overtemp_get(&val);
    if (val == 70) {
        log_str(DEBUG, TEMP_LOG, false, "OT set 70 passed.");
        tests_passed++;
    } else {
        log_str(ERROR, TEMP_LOG, true, "OT set 70 failed.");
    }

    // Check overtemp 75
    stts75_overtemp_set(75);
    stts75_overtemp_get(&val);
    if (val == 75) {
        log_str(DEBUG, TEMP_LOG, false, "OT set 75 passed.");
        tests_passed++;
    } else {
        log_str(ERROR, TEMP_LOG, true, "OT set 75 failed.");
    }

    // Check overtemp 80
    stts75_overtemp_set(80);
    stts75_overtemp_get(&val);
    if (val == 80) {
        log_str(DEBUG, TEMP_LOG, false, "OT set 80 passed.");
        tests_passed++;
    } else {
        log_str(ERROR, TEMP_LOG, true, "OT set 80 failed.");
    }

    return (tests_passed == 3);
}
