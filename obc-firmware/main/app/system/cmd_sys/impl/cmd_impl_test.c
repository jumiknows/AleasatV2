/**
 * @file cmd_impl_test.c
 * @brief Implementation of general test commands
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// OBC
#include "obc_filesystem.h"
#include "obc_flash.h"
#include "obc_gpio.h"
#include "logger.h"

// Utils
#include "io_stream.h"
#include "obc_utils.h"

// TMS570
#include "tms_can.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

cmd_sys_err_t cmd_impl_TEST_ECHO(
    const cmd_sys_cmd_t *cmd,
    cmd_TEST_ECHO_args_t *args, uint32_t args_len,
    cmd_TEST_ECHO_resp_t *resp, const data_fmt_desc_t *resp_desc, uint32_t resp_len, uint8_t *buf
) {
    static uint8_t message[32] = { 0 };

    // Read the message from the input stream
    uint32_t message_len = MIN((sizeof(message) - 1), (cmd->header.data_len - args_len)); // - 1 to keep NULL terminator
    uint32_t bytes_read = io_stream_read(cmd->input, message, message_len, pdMS_TO_TICKS(CMD_SYS_INPUT_READ_TIMEOUT_MS), NULL);
    if (bytes_read != message_len) {
        LOG_TEST_CMD__ECHO_BAD_ARGS();
        return CMD_SYS_ERR_READ_TIMEOUT;
    }

    // Log the message
    LOG_TEST_CMD__ECHO(message);

    // Populate the response
    resp->number = args->number;
    memcpy(resp->array, args->array, sizeof(resp->array));
    memcpy(resp->arrayf, args->arrayf, sizeof(resp->arrayf));

    // Send response
    cmd_sys_err_t err = CMD_SYS_SUCCESS;

    err = cmd_sys_begin_response(cmd, CMD_SYS_RESP_CODE_SUCCESS, (resp_len + message_len));
    if (err != CMD_SYS_SUCCESS) return err;

    err = cmd_sys_handle_resp_fields(cmd, resp, resp_desc, resp_len, buf);
    if (err != CMD_SYS_SUCCESS) return err;

    uint32_t bytes_written = io_stream_write(cmd->output, message, message_len, pdMS_TO_TICKS(CMD_SYS_OUTPUT_WRITE_TIMEOUT_MS), NULL);
    if (bytes_written != message_len) return CMD_SYS_ERR_WRITE_TIMEOUT;

    err = cmd_sys_finish_response(cmd);
    if (err != CMD_SYS_SUCCESS) return err;

    return err;
}

cmd_sys_resp_code_t cmd_impl_TEST_HANG(const cmd_sys_cmd_t *cmd, cmd_TEST_HANG_args_t *args) {
    obc_delay_us(args->duration_us);
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Write a value to a CAN pin as a GPIO
 */
cmd_sys_resp_code_t cmd_impl_TEST_CAN_GPIO(const cmd_sys_cmd_t *cmd, cmd_TEST_CAN_GPIO_args_t *args, cmd_TEST_CAN_GPIO_resp_t *resp) {
    static gpio_port_t CAN_PORTS[] = {CAN_PORT(canREG1), CAN_PORT(canREG2), CAN_PORT(canREG3)};

    // Validate arguments
    if (args->port >= 3) {
        LOG_TEST_CAN_GPIO_CMD__INVALID_PORT(args->port+1);
        return CMD_SYS_RESP_CODE_ERROR;
    }
    if ((args->pin != CAN_PIN_RX) && (args->pin != CAN_PIN_TX)) {
        LOG_TEST_CAN_GPIO_CMD__INVALID_PIN(args->pin);
        return CMD_SYS_RESP_CODE_ERROR;
    }

    LOG_TEST_CAN_GPIO_CMD__WRITING_TO_CAN(args->value, args->port+1, args->pin);

    gpio_err_t err = obc_gpio_write(CAN_PORTS[args->port], args->pin, args->value);

    resp->gpio_err = err;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Erases full chip, then performs a 1KB chunk write, read, and comparison
 * 
 * @warning Erases the FULL flash memory!
 */
cmd_sys_resp_code_t cmd_impl_TEST_FLASH_RW(const cmd_sys_cmd_t *cmd, cmd_TEST_FLASH_RW_args_t *args, cmd_TEST_FLASH_RW_resp_t *resp) {
    static uint8_t read_data[1024];
    static uint8_t write_data[1024];
    for (uint32_t i = 0; i < sizeof(write_data); i++) {
        write_data[i] = i % 256;
        read_data[i] = 0; // clear any old read data
    }

    if (args->len > sizeof(write_data)) {
        LOG_TEST_FLASH_RW_CMD__BAD_LEN(args->len, sizeof(write_data));
        return CMD_SYS_RESP_CODE_ERROR;
    }

    LOG_TEST_FLASH_RW_CMD__PERFORMING_TEST(args->addr, args->len);

    flash_err_t erase_err = flash_erase(args->addr, FULL_CHIP);
    flash_err_t write_err = flash_write(args->addr, args->len, write_data);
    flash_err_t read_err = flash_read(args->addr, args->len, read_data);

    resp->erase_err = erase_err;
    resp->write_err = write_err;
    resp->read_err = read_err;
    resp->data_match = (0 == memcmp(write_data, read_data, args->len));

    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Executes the *destructive* filesystem self test
 * 
 * @warning Reformats the FS, losing all data!
 */
cmd_sys_resp_code_t cmd_impl_TEST_FILESYSTEM(const cmd_sys_cmd_t *cmd, cmd_TEST_FILESYSTEM_resp_t *resp) {
    // First, deinit the FS
    fs_deinit();

    // Run the test
    fs_err_t err = fs_self_test();

    // Finally, reinit the FS
    fs_init();

    resp->fs_err = err;
    return CMD_SYS_RESP_CODE_SUCCESS;
}
