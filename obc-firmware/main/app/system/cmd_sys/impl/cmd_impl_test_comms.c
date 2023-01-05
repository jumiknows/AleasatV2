/**
 * @file cmd_impl_test_comms.c
 * @brief Implementation of test commands for COMMS
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_gen.h"
#include "cmd_sys.h"

// COMMS
#include "obc_comms.h"
#include "comms_cmd.h"
#include "comms_app_image.h"
#include "comms_telem.h"

// OBC
#include "logger.h"
#include "obc_hardwaredefs.h"
#include "obc_error.h"

// Utils
#include "io_stream.h"
#include "obc_utils.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include "assert.h"

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Tests sending the command arguments via raw MIBSPI.
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_RAW(const cmd_sys_cmd_t *cmd, cmd_TEST_COMMS_RAW_resp_t *resp) {
    uint16_t data[COMMS_TG_SIZE_WORDS] = {0x0000};
    memset(data, 0xAB, sizeof(data));

    uint32_t bytes_read = io_stream_read(cmd->input, (uint8_t *)data, cmd->header.data_len, pdMS_TO_TICKS(CMD_SYS_INPUT_READ_TIMEOUT_MS), NULL);
    if (bytes_read != cmd->header.data_len) {
        return CMD_SYS_RESP_CODE_ERROR;
    }

    mibspi_err_t err = comms_mibspi_tx(data);

    resp->mibspi_err = (int8_t)err;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Tests sending command, taking command line argument as params. Does not wait for response.
 *
 * If testing with Comms board, change comms_hwid
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_TX_ONLY(const cmd_sys_cmd_t *cmd, cmd_TEST_COMMS_TX_ONLY_resp_t *resp) {
    uint8_t message[226] = { 0 };
    if (cmd->header.data_len > sizeof(message)) {
        return CMD_SYS_RESP_CODE_ERROR;
    }

    uint32_t bytes_read = io_stream_read(cmd->input, (uint8_t *)message, cmd->header.data_len, pdMS_TO_TICKS(CMD_SYS_INPUT_READ_TIMEOUT_MS), NULL);
    if (bytes_read != cmd->header.data_len) {
        return CMD_SYS_RESP_CODE_ERROR;
    }

    comms_err_t err = comms_send_cmd(comms_hwid, COMMS_COMMON_MSG_ASCII, message, cmd->header.data_len, COMMS_MIBSPI_MUTEX_TIMEOUT_MS);

    resp->comms_err = (int8_t)err;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Tests sending commands and receiving the responses.
 *
 * If testing with Comms board, change comms_hwid
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_TX_RX(const cmd_sys_cmd_t *cmd, cmd_TEST_COMMS_TX_RX_resp_t *resp) {
    comms_err_t err;
    comms_command_t comms_resp = {0};

    err = comms_send_recv_cmd(comms_hwid, COMMS_RADIO_MSG_GET_TELEM, NULL, 0, &comms_resp, COMMS_MIBSPI_MUTEX_TIMEOUT_MS);

    resp->comms_err = err;
    resp->command = comms_resp.header.command;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Tests repeatedly sending and receiving the ack command.
 *
 * If testing with Comms board, change comms_hwid
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_STRESS1(const cmd_sys_cmd_t *cmd, cmd_TEST_COMMS_STRESS1_resp_t *resp) {
    comms_err_t err;
    comms_command_t comms_resp = {0};
    uint32_t num_success = 0;
    uint32_t num_fail = 0;
    uint32_t i;

    for (i = 0; i < 1000; ++i) {
        memset(&comms_resp, 0, sizeof(comms_resp));
        err = comms_send_recv_cmd(comms_hwid, COMMS_COMMON_MSG_ACK, NULL, 0, &comms_resp, COMMS_MIBSPI_MUTEX_TIMEOUT_MS);
        if ((err != COMMS_SUCCESS) || (comms_resp.header.command != COMMS_COMMON_MSG_ACK)) {
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

    resp->pass = num_success;
    resp->fail = num_fail;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Tests flashing Comms with example app image in repo
 *
 * If testing with Comms board, change comms_hwid
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_FLASH_APP(const cmd_sys_cmd_t *cmd, cmd_TEST_COMMS_FLASH_APP_resp_t *resp) {
    comms_err_t err;

    err = comms_flash_image(comms_test_app_image_pages, comms_test_app_image_num_pages);
    
    resp->comms_err = err;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Test sending reboot command to Comms and waits for radio app start message
 *
 * Will fail if Comms doesn't have an app image loaded
 *
 * If testing with Comms board, change comms_hwid
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_REBOOT(const cmd_sys_cmd_t *cmd) {
    comms_err_t err;
    comms_command_t msg = {0};
    comms_waiter_match_params_t app_start_match_spec = {
        .match_cmd_num_spec = COMMS_MATCH_EQUAL,
        .match_cmd_num = COMMS_RADIO_MSG_START,
        .cmd_ptr = &msg
    };

    err = comms_send_cmd(comms_hwid, COMMS_RADIO_MSG_REBOOT, NULL, 0, COMMS_MIBSPI_MUTEX_TIMEOUT_MS);
    if (err != COMMS_SUCCESS) {
        log_str(INFO, LOG_TEST_COMMS_CMD, "reboot fail 1");
        return CMD_SYS_RESP_CODE_ERROR;
    }

    err = comms_wait_for_cmd(&app_start_match_spec, 2000);
    if (err != COMMS_SUCCESS) {
        log_str(INFO, LOG_TEST_COMMS_CMD, "reboot fail 2");
        return CMD_SYS_RESP_CODE_ERROR;
    }

    log_str(INFO, LOG_TEST_COMMS_CMD, "reboot pass");
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Tests sending comms get_telem command.
 *
 * If testing with Comms board, change comms_hwid
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_GET_TELEM(const cmd_sys_cmd_t *cmd, cmd_TEST_COMMS_GET_TELEM_resp_t *resp) {
    comms_err_t err;
    comms_telem_t telem_recv = { 0 };

    err = comms_get_telem(&telem_recv);
    if (err != COMMS_SUCCESS) {
        log_str(ERROR, LOG_TEST_COMMS_CMD, "Unable to get telemetry!");
        return CMD_SYS_RESP_CODE_ERROR;
    }

    resp->uptime         = telem_recv.uptime;
    resp->uart0_rx_count = telem_recv.uart0_rx_count;
    resp->uart1_rx_count = telem_recv.uart1_rx_count;
    resp->rx_mode        = telem_recv.rx_mode;
    resp->tx_mode        = telem_recv.tx_mode;

    CASSERT(sizeof(resp->adc) == sizeof(telem_recv.adc), cmd_impl_test_comms_c);
    memcpy(resp->adc, telem_recv.adc, sizeof(telem_recv.adc));

    resp->rssi                      = telem_recv.rssi;
    resp->last_lqi                  = telem_recv.last_lqi;
    resp->last_frequest             = telem_recv.last_frequest;
    resp->packets_sent              = telem_recv.packets_sent;
    resp->cs_count                  = telem_recv.cs_count;
    resp->packets_good              = telem_recv.packets_good;
    resp->packets_rejected_checksum = telem_recv.packets_rejected_checksum;
    resp->packets_rejected_other    = telem_recv.packets_rejected_other;
    resp->custom0                   = telem_recv.custom0;
    resp->custom1                   = telem_recv.custom1;

    return CMD_SYS_RESP_CODE_SUCCESS;
}
