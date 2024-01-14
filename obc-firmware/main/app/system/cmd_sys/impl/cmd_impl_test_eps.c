/**
 * @file cmd_impl_test_eps.c
 * @brief Implementation of test commands for the EPS
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// OBC
#include "obc_eps.h"
#include "logger.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define ASSERT_EPS_SUCCESS(err)         if ((err) != EPS_SUCCESS) break;
#define ASSERT_VOLTAGE_HIGH(voltage)    if (((voltage) < 2.0f) || ((voltage) > 8.0f)) break; else success_counter++;
#define ASSERT_CURRENT_HIGH(current)    if (((current) < 0.010f) || ((current) > 1.0f)) break; else success_counter++;
#define ASSERT_TEMPERATURE_MEDIUM(temp) if (((temp) < 10.0f) || ((temp) > 35.0f)) break; else success_counter++;
#define ASSERT_COUNTER_LOW(counter)     if ((counter) > 100) break; else success_counter++;
#define ASSERT_VOLTAGE_LOW(voltage)     if ((voltage) > 0.5f) break; else success_counter++;
#define ASSERT_INT_EQUAL(val1, val2)    if ((val1) != (val2)) break; else success_counter++;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Tests the EPS I2C interface: eps_read_float function
 * @brief Sends a test read command to the EPS module
 */
cmd_sys_resp_code_t cmd_impl_TEST_EPS_READ_FLOAT(const cmd_sys_cmd_t *cmd, cmd_TEST_EPS_READ_FLOAT_args_t *args,
        cmd_TEST_EPS_READ_FLOAT_resp_t *resp) {
    // Validate arguments
    if ((args->read_cmd < 1) || (args->read_cmd > 52)) {
        TEST_EPS_READ_CMD__READ_FLOAT_INVALID_CMD(args->read_cmd);
        return CMD_SYS_RESP_CODE_ERROR;
    }

    float32 data = 0;
    eps_err_t err = eps_read_float((eps_cmd_read_float_t) args->read_cmd, &data);

    resp->eps_err = err;
    resp->data = data;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Tests the EPS I2C interface: eps_read_float_raw function
 * @brief Sends a test read command to the EPS module
 */
cmd_sys_resp_code_t cmd_impl_TEST_EPS_READ_FLOAT_RAW(const cmd_sys_cmd_t *cmd, cmd_TEST_EPS_READ_FLOAT_RAW_args_t *args,
        cmd_TEST_EPS_READ_FLOAT_RAW_resp_t *resp) {
    // Validate arguments
    if ((args->read_cmd < 1) || (args->read_cmd > 52)) {
        TEST_EPS_READ_CMD__READ_FLOAT_RAW_INVALID_CMD(args->read_cmd);
        return CMD_SYS_RESP_CODE_ERROR;
    }

    uint16_t data = 0;
    eps_err_t err = eps_read_float_raw((eps_cmd_read_float_t) args->read_cmd, &data);

    resp->eps_err = err;
    resp->data = data;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Tests the EPS I2C interface: eps_read_int function
 * @brief Sends a test read command to the EPS module
 */
cmd_sys_resp_code_t cmd_impl_TEST_EPS_READ_INT(const cmd_sys_cmd_t *cmd, cmd_TEST_EPS_READ_INT_args_t *args, cmd_TEST_EPS_READ_INT_resp_t *resp) {
    // Validate arguments
    if ((args->read_cmd < 1) || (args->read_cmd > 52)) {
        TEST_EPS_READ_CMD__READ_INT_INVALID_CMD(args->read_cmd);
        return CMD_SYS_RESP_CODE_ERROR;
    }

    uint16_t data = 0;
    eps_err_t err = eps_read_int((eps_cmd_read_int_t) args->read_cmd, &data);

    resp->eps_err = err;
    resp->data = data;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Sends a test write command to the EPS module
 */
cmd_sys_resp_code_t cmd_impl_TEST_EPS_WRITE(const cmd_sys_cmd_t *cmd, cmd_TEST_EPS_WRITE_args_t *args, cmd_TEST_EPS_WRITE_resp_t *resp) {
    // Validate arguments
    //   no need to validate write command, as it can be any non-negative integer
    if (args->state > 3) {
        TEST_EPS_WRITE_CMD__INVALID_WRITE_STATE(args->state);
        return CMD_SYS_RESP_CODE_ERROR;
    }

    eps_err_t err = eps_write_state((eps_cmd_write_state_t) args->write_cmd, (eps_write_state_t) args->state);

    resp->eps_err = err;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Perform a series of READ operations on the EPS module, and sanity check their responses
 */
cmd_sys_resp_code_t cmd_impl_TEST_EPS_READ_SANITY(const cmd_sys_cmd_t *cmd, cmd_TEST_EPS_READ_SANITY_resp_t *resp) {
    eps_err_t err = EPS_SUCCESS;
    bool success = false;
    uint8_t success_counter = 0; // Incremented for every succeeded test
    float32 retf = -1.0f;
    uint16_t reti = 9999U;

    // Run test suite: return on failure
    do {
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
    } while (0);

    if (success) {
        resp->success_counter = success_counter;
        return CMD_SYS_RESP_CODE_SUCCESS;
    } else {
        return CMD_SYS_RESP_CODE_ERROR;
    }
}

/**
 * @brief Perform a series of EPS tests involving writing and reading to the MCU
 */
cmd_sys_resp_code_t cmd_impl_TEST_EPS_WRITEREAD_SANITY(const cmd_sys_cmd_t *cmd, cmd_TEST_EPS_WRITEREAD_SANITY_resp_t *resp) {
    eps_err_t err = EPS_SUCCESS;
    bool success = false;
    uint8_t success_counter = 0; // Incremented for every succeeded test
    float32 retf = -1.0f;

    // Run test suite: return on failure
    do {
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
        ASSERT_INT_EQUAL((uint8_t)(retf / 1.497), 0x30)

        success = true; // Every test passed
    } while (0);

    if (success) {
        resp->success_counter = success_counter;
        return CMD_SYS_RESP_CODE_SUCCESS;
    } else {
        return CMD_SYS_RESP_CODE_ERROR;
    }
}
