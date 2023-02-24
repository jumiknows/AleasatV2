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
#include "comms_flash.h"
#include "comms_api.h"
#include "comms_app_image.h"
#include "comms_telem.h"
#include "comms_defs.h"

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
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

static comms_session_handle_t session = -1;
static SemaphoreHandle_t comms_reboot_sem;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void comms_reboot_callback(comms_session_handle_t session_handle, comms_event_id_t ev_id, void* arg);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Tests sending commands and receiving the responses.
 *
 * If testing with Comms board, change comms_hwid
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_TX_RX(const cmd_sys_cmd_t *cmd, cmd_TEST_COMMS_TX_RX_resp_t *resp) {
    comms_err_t err = COMMS_SUCCESS;
    // (MISRA-C:2004 9.2/R) Braces shall be used to indicate and match the structure 
    // in the non-zero initialization of arrays and structures
    //
    // MISRA requires each field to be explicitly initialized when any fields are initialized to a non-zero value
    // Disable the MISRA check here so we don't have to zero out each element of the array in this struct
    OBC_MISRA_CHECK_OFF
    comms_cmd_resp_t comms_resp = {COMMS_CMD_RESULT_OK, {0}};
    OBC_MISRA_CHECK_ON

    // open a comms session to communicate with the radio card
    if(session < 0) {
        err = comms_session_init(COMMS_ENDPOINT_RADIO, &session);
    }

    if (err != COMMS_SUCCESS) {
        log_str(INFO, LOG_TEST_COMMS_CMD, "failed to start a comms session: %d ", err);
        return CMD_SYS_RESP_CODE_ERROR;
    }

    // send a command to request telemtry 
    err = comms_send_command(session, COMMS_CMD_GET_TELEM, NULL, 0, 0);
    if(err != COMMS_SUCCESS) {
        log_str(INFO, LOG_TEST_COMMS_CMD, "failed to send command: %d ", err);
        return CMD_SYS_RESP_CODE_ERROR;
    }

        // block and wait for response
    err = comms_wait_cmd_resp(session, &comms_resp, pdMS_TO_TICKS(500));

    if(err != COMMS_SUCCESS) {
        log_str(INFO, LOG_TEST_COMMS_CMD, "failed to receive command response: %d ", err);
        return CMD_SYS_RESP_CODE_ERROR;
    }

    resp->comms_err = err;
    resp->command = (uint8_t)COMMS_CMD_GET_TELEM;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Tests repeatedly sending and receiving the ack command.
 *
 * If testing with Comms board, change comms_hwid
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_STRESS1(const cmd_sys_cmd_t *cmd, cmd_TEST_COMMS_STRESS1_resp_t *resp) {
    comms_err_t err = COMMS_SUCCESS;
    comms_cmd_resp_t comms_resp;
    uint32_t num_success = 0;
    uint32_t num_fail = 0;
    uint32_t i;

    // open a comms session to communicate with the radio card
    if(session < 0) {
        err = comms_session_init(COMMS_ENDPOINT_RADIO, &session);
    }

    if (err != COMMS_SUCCESS) {
           log_str(INFO, LOG_TEST_COMMS_CMD, "failed to start a comms session: %d ", err);
           return CMD_SYS_RESP_CODE_ERROR;
    }

    for (i = 0; i < 1000; ++i) {
        memset(&comms_resp, 0, sizeof(comms_resp));
        comms_send_command(session, COMMS_CMD_PING, NULL, 0, 0);
        err = comms_wait_cmd_resp(session, &comms_resp, pdMS_TO_TICKS(500));
        if ((err != COMMS_SUCCESS) || (comms_resp.result != COMMS_CMD_RESULT_OK)) {
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
    comms_err_t err = COMMS_SUCCESS;

    // open a comms session to communicate with the radio card
    if(session < 0) {
        err = comms_session_init(COMMS_ENDPOINT_RADIO, &session);
    }

    if (err != COMMS_SUCCESS) {
        log_str(INFO, LOG_TEST_COMMS_CMD, "failed to start a comms session: %d ", err);
        return CMD_SYS_RESP_CODE_ERROR;
    }

    err = comms_flash_image(session, comms_test_app_image_pages,
                               comms_test_app_image_num_pages);

    resp->comms_err = err;
    return CMD_SYS_RESP_CODE_SUCCESS;
}

/**
 * @brief Unblock reboot test thread after receiving radio app start message
 */
static void comms_reboot_callback(comms_session_handle_t session_handle,
                           comms_event_id_t ev_id,
                           void* arg)
{
    comms_command_t* cmd = (comms_command_t*)arg;

    if(cmd->header.command == COMMS_RADIO_MSG_START) {
        xSemaphoreGive(comms_reboot_sem);
    }
}

/**
 * @brief Test sending reboot command to Comms and waits for radio app start message
 *
 * Will fail if Comms doesn't have an app image loaded
 *
 * If testing with Comms board, change comms_hwid
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_REBOOT(const cmd_sys_cmd_t *cmd) {
    comms_err_t err = COMMS_SUCCESS;

    // (MISRA-C:2004 9.2/R) Braces shall be used to indicate and match the structure 
    // in the non-zero initialization of arrays and structures
    //
    // MISRA requires each field to be explicitly initialized when any fields are initialized to a non-zero value
    // Disable the MISRA check here so we don't have to zero out each element of the array in this struct
    OBC_MISRA_CHECK_OFF
    comms_cmd_resp_t comms_resp = {COMMS_CMD_RESULT_OK, {0}};
    OBC_MISRA_CHECK_ON

    static StaticSemaphore_t comms_reboot_sem_buf = {0};

    // open a comms session to communicate with the radio card
    if(session < 0) {
        comms_session_init(COMMS_ENDPOINT_RADIO, &session);
    }

    if(!comms_reboot_sem) {
        comms_reboot_sem = xSemaphoreCreateBinaryStatic(&comms_reboot_sem_buf);
    }

    err = comms_register_events(session, (1<<COMMS_EVENT_MSG_RCV), &comms_reboot_callback);
    if(err != COMMS_SUCCESS) {
        log_str(INFO, LOG_TEST_COMMS_CMD, "failed to register for comms msg events %d", err);
        return CMD_SYS_RESP_CODE_ERROR;
    }

    err = comms_send_command(session, COMMS_CMD_REBOOT, NULL, 0, 0);
    if(err != COMMS_SUCCESS) {
        log_str(INFO, LOG_TEST_COMMS_CMD, "failed to send reboot command %d", err);
        return CMD_SYS_RESP_CODE_ERROR;
    }

    // block and wait for response
     err = comms_wait_cmd_resp(session, &comms_resp, pdMS_TO_TICKS(500));
     if((err != COMMS_SUCCESS) || (comms_resp.result == COMMS_CMD_RESULT_ERR)) {
         log_str(INFO, LOG_TEST_COMMS_CMD, "failed to receive reboot response %d", err);
         return CMD_SYS_RESP_CODE_ERROR;
     }

    if(xSemaphoreTake(comms_reboot_sem, pdMS_TO_TICKS(2000)) == pdTRUE) {
        log_str(INFO, LOG_TEST_COMMS_CMD, "reboot pass");
        return CMD_SYS_RESP_CODE_SUCCESS;
    }
    else {
        log_str(INFO, LOG_TEST_COMMS_CMD, "failed waiting for reboot");
        return CMD_SYS_RESP_CODE_ERROR;
    }
}

/**
 * @brief Tests sending comms get_telem command.
 *
 * If testing with Comms board, change comms_hwid
 */
cmd_sys_resp_code_t cmd_impl_TEST_COMMS_GET_TELEM(const cmd_sys_cmd_t *cmd, cmd_TEST_COMMS_GET_TELEM_resp_t *resp) {
    comms_err_t err = COMMS_SUCCESS;
    comms_telem_t telem_recv = { 0 };

    if(session < 0) {
        comms_session_init(COMMS_ENDPOINT_RADIO, &session);
    }

    err = comms_get_telem(session, &telem_recv);
    if (err != COMMS_SUCCESS) {
        log_str(INFO, LOG_TEST_COMMS_CMD, "Unable to get telemetry!");
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
