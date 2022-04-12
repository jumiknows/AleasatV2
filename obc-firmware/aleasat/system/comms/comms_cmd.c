/**
 * @file comms_cmd.c
 * @brief High-level OBC to Comms driver
 *
 * @see https://gitlab.com/alea-2020/communications/comms-firmware-openlst-1
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_defs.h"
#include "comms_utils.h"
#include "obc_comms.h"
#include "comms_cmd.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_error.h"

// Logging
#include "logger.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Period to wait after sending/receiving a message to/from Comms before sending another
 */
#define COMMS_NEXT_SEND_DELAY_MS 20

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief Sequence number of messages, a number is assigned to each message to indicate ordering.
 * Seqnum of received message equalling a sent message's seqnum indicates that message was a response.
 * Use inc_seqnum() to incrememt.
 */
static uint16_t comms_seqnum = 1;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Send command based on arguments without waiting for response
 *
 * @warning This function is not re-entrant
 *
 * @param[in] dest_hwid destination HWID
 * @param[in] cmd_num command ID
 * @param[in] cmd_data data to send as part of command, or set to NULL for no data
 * @param[in] cmd_data_len length of cmd_data
 * @param[in] timeout_ms time in milliseconds to wait before timeout of Comms MIBSPI mutex
 *
 * @return value indicating if error occured or not
 */
comms_err_t comms_send_cmd(
    hwid_t dest_hwid,
    uint8_t cmd_num,
    const uint8_t* cmd_data,
    uint8_t cmd_data_len,
    uint32_t timeout_ms
) {
    uint16_t buf[COMMS_TG_SIZE_WORDS] = {0};
    mibspi_err_t mibspi_ret = MIBSPI_UNKNOWN_ERR;
    comms_err_t err = COMMS_UNKNOWN_ERR;

    err = comms_build_buffer(false, dest_hwid, comms_seqnum, cmd_num, cmd_data, cmd_data_len, buf);
    if (err != COMMS_SUCCESS) {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms send arg err %d", err);
        return err;
    }

    if (xSemaphoreTake(xMibspiCommsMutexHandle, pdMS_TO_TICKS(timeout_ms))) {
        mibspi_ret = comms_mibspi_tx(buf);
        xSemaphoreGive(xMibspiCommsMutexHandle);
        if (mibspi_ret == MIBSPI_TIMEOUT) {
            err = COMMS_MIBSPI_TIMEOUT;
        }
        else if (mibspi_ret != MIBSPI_NO_ERR) {
            err = COMMS_MIBSPI_ERR;
        }
        else {
            inc_seqnum(&comms_seqnum);
            err = COMMS_SUCCESS;
        }
    }
    else {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms send mtx t-out");
        err = COMMS_MIBSPI_MUTEX_TIMEOUT;
        return err;
    }

    // check comms_mibspi_tx result
    if (err != COMMS_SUCCESS) {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms send tx err %d", mibspi_ret);
        return err;
    }

    return err;
}

/**
 * @brief Send command based on arguments and waits to receive matching response
 *
 * @warning This function is not re-entrant
 *
 * @param[in] dest_hwid destination HWID
 * @param[in] cmd_num command ID
 * @param[in] cmd_data data to send as part of command, or set to NULL for no data
 * @param[in] cmd_data_len length of cmd_data
 * @param[out] recv_cmd preallocated comms_command_t struct to fill
 * @param[in] timeout_ms time in milliseconds to wait before timeout of Comms MIBSPI mutex
 *
 * @return value indicating if error occured or not
 */
comms_err_t comms_send_recv_cmd(
    hwid_t dest_hwid,
    uint8_t cmd_num,
    const uint8_t* cmd_data,
    uint8_t cmd_data_len,
    comms_command_t* recv_cmd,
    uint32_t timeout_ms
) {
    uint16_t buf[COMMS_TG_SIZE_WORDS] = {0};
    mibspi_err_t mibspi_ret = MIBSPI_UNKNOWN_ERR;
    comms_err_t err = COMMS_UNKNOWN_ERR;
    comms_err_t err2 = COMMS_UNKNOWN_ERR;
    EventBits_t uxBits;

    err = comms_build_buffer(false, dest_hwid, comms_seqnum, cmd_num, cmd_data, cmd_data_len, buf);
    if (err != COMMS_SUCCESS) {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms sdrc arg err %d", err);
        return err;
    }

    // src_hwid to match for response is what we specified as the destination when sending original msg
    err = comms_set_waiter_match(
        COMMS_MATCH_EQUAL,
        1,
        COMMS_MATCH_EQUAL,
        comms_seqnum,
        COMMS_MATCH_EQUAL,
        dest_hwid,
        COMMS_MATCH_ANY,
        0,
        recv_cmd
    );
    if (err != COMMS_SUCCESS) {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms sdrc parm err 1 %d", err);
        return err;
    }

    do {
        if (xSemaphoreTake(xMibspiCommsMutexHandle, pdMS_TO_TICKS(timeout_ms)) == pdTRUE) {
            mibspi_ret = comms_mibspi_tx(buf);
            xSemaphoreGive(xMibspiCommsMutexHandle);
        }
        else {
            log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms sdrc mtx t-out");
            err = COMMS_MIBSPI_MUTEX_TIMEOUT;
            break;
        }

        if (mibspi_ret == MIBSPI_TIMEOUT) {
            err = COMMS_MIBSPI_TIMEOUT;
        }
        else if (mibspi_ret != MIBSPI_NO_ERR) {
            err = COMMS_MIBSPI_ERR;
        }
        else {
            inc_seqnum(&comms_seqnum);
            err = COMMS_SUCCESS;
        }

        // check comms_mibspi_tx result
        if (err != COMMS_SUCCESS) {
            log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms sdrc tx err %d", mibspi_ret);
            break;
        }

        uxBits = xEventGroupWaitBits(xCommsWaiterEventGroupHandle, COMMS_RX_EVENT_UNBLOCK_WAITER_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(timeout_ms));
    } while (0);

    // Always cleanup by removing ourself as waiter
    err2 = comms_clear_waiter_match();
    if (err2 != COMMS_SUCCESS) {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms sdrc parm err 2 %d", err2);
        return err2;
    }

    if (err != COMMS_SUCCESS) {
        return err;
    }

    if ((uxBits & COMMS_RX_EVENT_UNBLOCK_WAITER_BIT) == 0) {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms sdrc evt t-out");
        err = COMMS_WAITER_EVTGRP_TIMEOUT;
        return err;
    }

    return err;
}

/**
 * @brief Wait to receive message from Comms that matches given parameters
 *
 * @param match_params Message parameters to match
 * @param timeout_ms time in milliseconds to wait before timeout of Comms MIBSPI mutex
 *
 * @return value indicating if error occured or not
 */
comms_err_t comms_wait_for_cmd(
    comms_waiter_match_params_t* match_params,
    uint32_t timeout_ms
) {
    comms_err_t err = COMMS_UNKNOWN_ERR;
    EventBits_t uxBits;

    err = comms_set_waiter_match_struct(match_params);
    if (err != COMMS_SUCCESS) {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms sdrc parm err 1 %d", err);
        return err;
    }

    uxBits = xEventGroupWaitBits(xCommsWaiterEventGroupHandle, COMMS_RX_EVENT_UNBLOCK_WAITER_BIT, pdTRUE, pdTRUE, pdMS_TO_TICKS(timeout_ms));

    // Always cleanup by removing ourself as waiter
    err = comms_clear_waiter_match();
    if (err != COMMS_SUCCESS) {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms sdrc parm err 2 %d", err);
        return err;
    }

    if ((uxBits & COMMS_RX_EVENT_UNBLOCK_WAITER_BIT) == 0) {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms sdrc evt t-out");
        err = COMMS_WAITER_EVTGRP_TIMEOUT;
        return err;
    }

    return err;
}
