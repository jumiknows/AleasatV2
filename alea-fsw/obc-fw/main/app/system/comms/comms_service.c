/**
 * @file comms_service.c
 *
 * @brief Implementation of the comms user API.
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// COMMS
#include "comms_api.h"
#include "comms_utils.h"
#include "comms_flash.h"

// Logger
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define COMMS_FWUP_STATE_BEGIN       0
#define COMMS_FWUP_STATE_FLASH_ERASE 1
#define COMMS_FWUP_STATE_WRITE_PAGE  2
#define COMMS_FWUP_STATE_WRITE_FINAL 3
#define COMMS_FWUP_STATE_COMPLETE    4

#define COMMS_CMD_PKT_TIMER_PERIOD_MS   2000
#define COMMS_FLASH_REBOOT_WAIT_TIME_MS 500

#define MAX_NUM_SESSIONS 5

#define RETRANSMIT_NUM_PKT      2
#define RETRANSMIT_NUM_ATTEMPTS 2

/******************************************************************************/
/*                               T Y P E D E F S                              */
/******************************************************************************/

typedef struct session {
    // integer identifier
    comms_session_handle_t hdl;

    // logical endpoint associated with this session
    comms_endpoint_id_t ep_id;

    // callback function to notify the client of an incoming command,
    // or the success/failure of a previously issued command
    comms_event_notify_cb ev_cb;

    // bitmask of the events registered by the client
    uint32_t ev_mask;

    // buffer to store command response
    comms_cmd_resp_t resp;

    // enable/disable packet retransmission
    bool retry_en;

    // counts the number of retransmission attempts
    int retry_count;

    // flag indicating if a command is pending on this session
    bool pending;
} session_t;

typedef struct endpoint {
    // destination hardware ID
    uint16_t hwid;

    // counts the number of packets exchanged on this endpoint
    uint16_t seqnum;
} endpoint_t;

typedef struct command_handler {
    int command;
    void (*func)(comms_command_t *cmd_in, comms_command_t *cmd_resp);
} command_handler_t;

/******************************************************************************/
/*          P R I V A T E  F U N C T I O N S  P R O T O T Y P E S             */
/******************************************************************************/

static void handle_ack_nack_command(comms_command_t *cmd_in, comms_command_t *cmd_resp);

static void handle_obc_data_command(comms_command_t *cmd_in, comms_command_t *cmd_resp);

static void handle_timer_expiry(TimerHandle_t timer);

static void dispatch_command_handler(comms_command_t *cmd_in, comms_command_t *cmd_resp);

static comms_err_t session_send_cmd(comms_session_handle_t session_handle, uint8_t command, const uint8_t *payload, uint8_t payload_len);

static void session_notify_event(session_t *sess, comms_event_id_t ev_id, void *arg);

static void session_notify_all(comms_event_id_t ev_id, comms_command_t *cmd);

static void session_handle_command_failure(session_t *sess);

static void session_handle_command_response(session_t *sess, comms_command_t *cmd);

static void session_handle_retransmission(session_t *sess);

/******************************************************************************/
/*              P R I V A T E  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

static uint8_t session_count;
static session_t sessions[MAX_NUM_SESSIONS];
static comms_session_handle_t active_session;

// Triggers a packet failure or retransmission if the timer
// expires before the response is received
static TimerHandle_t cmd_timer;

static SemaphoreHandle_t cmd_sema;
static comms_command_t cmd_buf;

endpoint_t endpoints[COMMS_ENDPOINT_MAX] = {
    [COMMS_ENDPOINT_RADIO]  = {.hwid = COMMS_HWID,  .seqnum = 0},
    [COMMS_ENDPOINT_GROUND] = {.hwid = GROUND_HWID, .seqnum = 0},
    [COMMS_ENDPOINT_ARO]    = {.hwid = 0,           .seqnum = 0},
    [COMMS_ENDPOINT_LOCAL]  = {.hwid = LOCAL_HWID,  .seqnum = 0},
};

static command_handler_t cmd_handlers[] = {
    {.command = COMMS_COMMON_MSG_ACK,      .func = &handle_ack_nack_command},
    {.command = COMMS_COMMON_MSG_NACK,     .func = &handle_ack_nack_command},
    {.command = COMMS_CUSTOM_MSG_OBC_DATA, .func = &handle_obc_data_command},
};

static uint8_t command_table[] = {
    [COMMS_CMD_PING]      = COMMS_COMMON_MSG_ACK,
    [COMMS_CMD_APP_DATA]  = COMMS_CUSTOM_MSG_OBC_DATA,
    [COMMS_CMD_GET_TELEM] = COMMS_RADIO_MSG_GET_TELEM,
    [COMMS_CMD_REBOOT]    = COMMS_RADIO_MSG_REBOOT,
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initialize the comms service
 */
void comms_service_create_infra(void) {
    static StaticTimer_t timer_buffer;
    static StaticSemaphore_t cmd_sema_buf;

    cmd_timer = xTimerCreateStatic("comms_service_timer", pdMS_TO_TICKS(COMMS_CMD_PKT_TIMER_PERIOD_MS), pdFALSE, 0, &handle_timer_expiry, &timer_buffer);

    cmd_sema = xSemaphoreCreateBinaryStatic(&cmd_sema_buf);
    xSemaphoreGive(cmd_sema);

    comms_mngr_create_infra();
}

/**
 * @brief Initialize a comms service session
 *
 * @param[in]  Logical endpoint associated with this session
 * @param[out] pointer to comms session handle
 *
 * @return COMMS_SUCCESS         is no error.
 *         COMMS_ERR_INVALID_ARG if session_handle is null or ep_id is invalid
 *         COMMS_ERR_BUSY        if the maximum number of sessions is exceeded
 */
comms_err_t comms_session_init(comms_endpoint_id_t ep_id, comms_session_handle_t *session_handle) {
    comms_err_t err = COMMS_SUCCESS;
    uint8_t session_id;

    do {
        if ((session_handle == NULL) || (COMMS_ENDPOINT_MAX < (uint8_t)ep_id)) {
            err = COMMS_ERR_INVALID_ARG;
            break;
        }

        taskENTER_CRITICAL();
        session_id = session_count++;
        taskEXIT_CRITICAL();

        if (MAX_NUM_SESSIONS <= session_id) {
            err = COMMS_ERR_BUSY;
            break;
        }

        sessions[session_id].ep_id = ep_id;
        sessions[session_id].hdl   = (comms_session_handle_t)session_id;
    } while (0);

    *session_handle = (comms_session_handle_t)session_id;

    return err;
}

/**
 * @brief Register for events on a comms service session
 *
 * @param[in] session_handle comms service session handle
 * @param[in] ev_mask bitmask of events to be registered
 * @param[in] cb      event callback function pointer
 *
 * @return COMMS_SUCCESS         is no error.
 *         COMMS_ERR_INVALID_ARG if any of the arguments are invalid
 */
comms_err_t comms_register_events(comms_session_handle_t session_handle, uint32_t ev_mask, comms_event_notify_cb ev_cb) {
    comms_err_t err = COMMS_ERR_INVALID_ARG;

    if ((MAX_NUM_SESSIONS > (uint8_t)session_handle) && (ev_cb != NULL)) {
        sessions[session_handle].ev_cb   = ev_cb;
        sessions[session_handle].ev_mask = ev_mask;
        err                              = COMMS_SUCCESS;
    }

    return err;
}

/**
 * @brief Send a command on a given comms session
 *
 * @param[in] command      command to send
 * @param[in] payload      pointer to payload buffer
 * @param[in] payload_len  length of payload buffer
 *
 * @return COMMS_SUCCESS         is no error.
 *         COMMS_ERR_INVALID_ARG if any of the arguments are invalid
 */
static comms_err_t session_send_cmd(comms_session_handle_t session_handle, uint8_t command, const uint8_t *payload, uint8_t payload_len) {
    comms_command_t *cmd;
    comms_err_t err;
    session_t *sess = &sessions[session_handle];
    endpoint_t *ep  = &endpoints[sess->ep_id];

    cmd                   = &cmd_buf;
    cmd->header.command   = command;
    cmd->header.dest_hwid = ep->hwid;
    cmd->header.src_hwid  = OBC_HWID;
    cmd->header.seqnum    = ep->seqnum++;

    if (payload != NULL) {
        cmd->data_len = payload_len;
        memcpy(cmd->data, payload, payload_len);
    }

    active_session = session_handle;
    err            = comms_mngr_send_cmd(cmd);

    if (err != COMMS_SUCCESS) {
        ep->seqnum--;
    } else {
        xTimerReset(cmd_timer, portMAX_DELAY);
        sess->retry_count = 0;
        sess->pending     = true;
    }

    return err;
}

/**
 * @brief Send a command over the comms interface
 *
 * @pre comms_service_create_infra
 *
 * @param[in] session_handle comms service session handle
 * @param[in] cmd_id         command to be sent
 * @param[in] payload        pointer to buffer containing the command payload
 * @param[in] payload_len    number of bytes in the payload buffer
 * @param[in] timeout_ticks  maximum wait time in ticks
 *
 * @return COMMS_SUCCESS             is no error.
 *         COMMS_ERR_INVALID_ARG     if any of the arguments are invalid
 *         COMMS_ERR_BUFFER_OVERFLOW if payload_len exceeds the maximum payload size
 *         COMMS_ERR_BUSY            if command queue is full
 */
comms_err_t comms_send_command(comms_session_handle_t session_handle, comms_command_id_t cmd_id, uint8_t *payload, uint16_t payload_len,
                               uint32_t timeout_ticks) {
    comms_err_t err = COMMS_SUCCESS;

    do {
        if ((MAX_NUM_SESSIONS <= (uint8_t)session_handle) || (COMMS_CMD_MAX <= (uint8_t)cmd_id)) {
            err = COMMS_ERR_INVALID_ARG;
            break;
        }

        if (COMMS_MAX_CMD_PAYLOAD_NUM_BYTES < payload_len) {
            err = COMMS_ERR_BUF_OVERFLOW;
            break;
        }

        if ((COMMS_CMD_APP_DATA == cmd_id) && (((COMMS_MAX_CMD_PAYLOAD_NUM_BYTES * RETRANSMIT_NUM_PKT) < payload_len))) {
            err = COMMS_ERR_BUF_OVERFLOW;
            break;
        }

        if (!xSemaphoreTake(cmd_sema, timeout_ticks)) {
            err = COMMS_ERR_BUSY;
            break;
        }

        err = session_send_cmd(session_handle, command_table[cmd_id], payload, payload_len);

        if (err != COMMS_SUCCESS) {
            xSemaphoreGive(cmd_sema);
        }

    } while (0);

    return err;
}

/**
 * @brief Wait for a response to the last issued command
 *
 * @param[in]  session_handle comms service session handle
 * @param[out] resp           response buffer
 * @param[in]  timeout_ticks  maximum wait time in ticks
 *
 * @return COMMS_SUCCESS             is no error.
 *         COMMS_ERR_INVALID_ARG     if any of the arguments are invalid
 *         COMMS_ERR_BUFFER_OVERFLOW if payload_len exceeds the maximum payload size
 *         COMMS_ERR_BUSY            if command queue is full
 */
comms_err_t comms_wait_cmd_resp(comms_session_handle_t session_handle, comms_cmd_resp_t *resp, uint32_t timeout_ticks) {
    comms_err_t err = COMMS_ERR_INVALID_ARG;

    if (!resp) {
        return err;
    }

    if (session_handle == active_session) {
        if (!xSemaphoreTake(cmd_sema, timeout_ticks)) {
            err = COMMS_ERR_TIMEOUT;
        } else {
            memcpy(resp, &sessions[session_handle].resp, sizeof(comms_cmd_resp_t));
            xSemaphoreGive(cmd_sema);
            err = COMMS_SUCCESS;
        }
    } else {
        err = COMMS_ERR_INVALID_SESSION;
    }

    return err;
}

/**
 * @brief Upload and flash a new firmware image on the comms board
 *
 * @pre comms_service_create_infra
 *
 * @param[in]  session_handle comms service session handle
 * @param[in]  pages          pointer to firmware image
 * @param[in]  num_pages      number of pages in image
 *
 * @return COMMS_SUCCESS             if no error.
 *         COMMS_ERR_INVALID_ARG     if any of the arguments are invalid
 *         COMMS_FLASH_FAIL          if any step of the firmware update failed
 */
comms_err_t comms_flash_image(comms_session_handle_t session_handle, const comms_app_image_page_t *pages, uint8_t num_pages) {
    session_t *sess;
    comms_err_t err = COMMS_SUCCESS;

    uint8_t fwup_state = COMMS_FWUP_STATE_BEGIN;
    uint8_t command_id, payload_len;
    const uint8_t *payload;
    uint8_t page_count = 0, final_page = 255;

    if (pages == NULL) {
        return COMMS_ERR_INVALID_ARG;
    }

    // Remove the client callback function so they do not receive notifications
    // during firmware update process
    sess                            = &sessions[session_handle];
    comms_event_notify_cb client_cb = sess->ev_cb;
    sess->ev_cb                     = NULL;

    // wait for the previous command to finish
    if (!xSemaphoreTake(cmd_sema, 0)) {
        sess->ev_cb = client_cb;
        return COMMS_ERR_BUSY;
    }

    while (fwup_state != COMMS_FWUP_STATE_COMPLETE) {
        switch (fwup_state) {
        case COMMS_FWUP_STATE_BEGIN:
            command_id  = COMMS_RADIO_MSG_REBOOT;
            payload     = NULL;
            payload_len = 0;
            break;

        case COMMS_FWUP_STATE_FLASH_ERASE:
            command_id  = COMMS_BOOTLOADER_MSG_ERASE;
            payload     = NULL;
            payload_len = 0;
            vTaskDelay(pdMS_TO_TICKS(COMMS_FLASH_REBOOT_WAIT_TIME_MS));
            break;

        case COMMS_FWUP_STATE_WRITE_PAGE:
            command_id  = COMMS_BOOTLOADER_MSG_WRITE_PAGE;
            payload     = pages[page_count].page_data;
            payload_len = sizeof(pages[page_count].page_data);
            break;

        case COMMS_FWUP_STATE_WRITE_FINAL:
            command_id = COMMS_BOOTLOADER_MSG_WRITE_PAGE;
            // final_page has the same scope as payload
            payload     = &final_page;
            payload_len = 1;
            break;

        default:
            // should never get here
            err = COMMS_FLASH_FAIL;
            break;
        }

        if (err != COMMS_SUCCESS) {
            break;
        }

        err = session_send_cmd(session_handle, command_id, payload, payload_len);

        // proceed on failure of reboot command in case comms is already in bootloader state
        if ((err != COMMS_SUCCESS) && (fwup_state != COMMS_FWUP_STATE_BEGIN)) {
            err = COMMS_FLASH_FAIL;
            break;
        }

        // semaphore is "given" after receiving a response to the previous command
        if (!xSemaphoreTake(cmd_sema, pdMS_TO_TICKS(1000))) {
            LOG_COMMS__FLASH_TIMEOUT(fwup_state);
            err = COMMS_FLASH_FAIL;
            break;
        }

        if (sess->resp.result != COMMS_CMD_RESULT_OK) {
            LOG_COMMS__ERROR(err);
            err = COMMS_FLASH_FAIL;
            break;
        }

        if (fwup_state != COMMS_FWUP_STATE_WRITE_PAGE) {
            fwup_state++;
        } else {
            // increment the fwupdate state after all pages have been written
            page_count++;

            if (page_count >= num_pages) {
                fwup_state++;
            }
        }
    }

    xSemaphoreGive(cmd_sema);
    sess->ev_cb = client_cb;
    return err;
}

/**
 * @brief Process a packet from the lower lower
 *
 * @param[in] cmd_in pointer to input command
 */
void comms_service_packet_input(comms_command_t *cmd_in) {
    static comms_command_t cmd_resp = {0};
    memset((void *)&cmd_resp, 0xff, sizeof(comms_command_t));

    dispatch_command_handler(cmd_in, &cmd_resp);

    if (comms_check_cmd_struct(&cmd_resp) == COMMS_SUCCESS) {
        comms_mngr_send_cmd(&cmd_resp);
    }

    if (cmd_in->header.command == COMMS_CUSTOM_MSG_OBC_DATA) {
        session_notify_all(COMMS_EVENT_MSG_RCV, cmd_in);
    }
}

/**
 * @brief Process failure of a previously sent command
 *
 * @param[in] cmd pointer to failed command
 */
void comms_service_packet_failure(comms_command_t *cmd) {
    session_t *sess;

    sess = &sessions[active_session];
    session_handle_command_failure(sess);
}

/******************************************************************************/
/*                    P R I V A T E   F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Invoke the handler corresponding to an incoming command request
 *
 * @param[in]  cmd_req  command request packet
 * @param[out] cmd_resp command response packet
 */
static comms_endpoint_id_t hwid_to_endpoint(uint16_t hwid) {
    int i;

    for (i = 0; i < COMMS_ENDPOINT_MAX; i++) {
        if (hwid == endpoints[i].hwid) {
            break;
        }
    }

    return (comms_endpoint_id_t)i;
}

/**
 * @brief Invoke the handler corresponding to an incoming command request
 *
 * @param[in]  cmd_req  command request packet
 * @param[out] cmd_resp command response packet
 */
static void dispatch_command_handler(comms_command_t *cmd_in, comms_command_t *cmd_resp) {
    session_t *sess = &(sessions[active_session]);
    size_t n        = sizeof(cmd_handlers) / sizeof(cmd_handlers[0]);

    if ((cmd_in->header.is_response) && (sess->pending)) {
        session_handle_command_response(sess, cmd_in);
    } else {
        for (uint32_t i = 0; i < n; i++) {
            if (cmd_in->header.command == cmd_handlers[i].command) {
                cmd_handlers[i].func(cmd_in, cmd_resp);
            }
        }
    }
}

/**
 * @brief Handle the packet timer expiry
 *
 * @param[in] timer expired timer handle
 */
static void handle_timer_expiry(TimerHandle_t timer) {
    session_t *sess = &(sessions[active_session]);
    session_handle_command_failure(sess);
}

/**
 * @brief Handle retransmission for an active session
 *
 * @param[in] sess pointer to session with a recently failed transmission
 */
static void session_handle_retransmission(session_t *sess) {
    comms_mngr_send_cmd(&cmd_buf);
    xTimerStart(cmd_timer, portMAX_DELAY);
    sess->retry_count++;
}

/**
 * @brief Notify a session that an event has occurred.
 *
 * @param[in] sess  pointer to session to notify
 * @param[in] ev_id event that occurred
 */
static void session_notify_event(session_t *sess, comms_event_id_t ev_id, void *arg) {
    comms_event_notify_cb ev_cb_func = sess->ev_cb;
    comms_session_handle_t sess_hdl  = sess->hdl;

    if ((ev_cb_func != NULL) && ((sess->ev_mask & (1 << ev_id)) != 0)) {
        ev_cb_func(sess_hdl, ev_id, arg);
    }
}

/**
 * @brief Notify all session that an event has occurred.
 *
 * @param[in] ev_id event that occurred
 */
static void session_notify_all(comms_event_id_t ev_id, comms_command_t *cmd) {
    uint32_t i                = 0;
    comms_endpoint_id_t ep_id = hwid_to_endpoint(cmd->header.src_hwid);

    for (i = 0; i < session_count; i++) {
        if (ep_id == sessions[i].ep_id) {
            session_notify_event(&sessions[i], ev_id, cmd);
        }
    }
}

/**
 * @brief Notify a session of a command failure
 *
 * @param[in] sess  pointer to session to notify
 */
static void session_handle_command_failure(session_t *sess) {
    if (xTimerIsTimerActive(cmd_timer) != pdFALSE) {
        xTimerStop(cmd_timer, portMAX_DELAY);
    }

    if ((sess->retry_en == true) && (sess->retry_count < RETRANSMIT_NUM_ATTEMPTS)) {
        session_handle_retransmission(sess);
    } else {
        LOG_COMMS__CMD_SEND_FAILED();
        sess->resp.result = COMMS_CMD_RESULT_ERR;
        sess->pending     = false;
        xSemaphoreGive(cmd_sema);

        session_notify_event(sess, COMMS_EVENT_CMD_FAILURE, (void *)&sess->resp);
    }
}

/**
 * @brief Process response to a previous command
 *
 * @param[in] sess  pointer to session waiting on this response
 */
static void session_handle_command_response(session_t *sess, comms_command_t *cmd) {
    if (xTimerIsTimerActive(cmd_timer) != pdFALSE) {
        xTimerStop(cmd_timer, portMAX_DELAY);
    }

    if ((cmd->header.seqnum == cmd_buf.header.seqnum) && (cmd->header.command != COMMS_COMMON_MSG_NACK)
            && (cmd->header.command != COMMS_BOOTLOADER_MSG_NACK)) {
        sess->resp.result = COMMS_CMD_RESULT_OK;
    } else {
        LOG_COMMS__CMD_FAILED();
        sess->resp.result = COMMS_CMD_RESULT_ERR;
    }

    memcpy(sess->resp.data, cmd->data, cmd->data_len);

    if ((sess->resp.result == COMMS_CMD_RESULT_ERR) && (sess->retry_en == true)) {
        session_handle_retransmission(sess);
    } else {
        // command was successful or retransmission is disabled, clear pending queue
        // and notify client that the command response is ready
        sess->pending = false;
        xSemaphoreGive(cmd_sema);

        session_notify_event(sess, COMMS_EVENT_CMD_RESP, (void *)&sess->resp);
    }
}

/**
 * @brief Handle an input ACK or NACK command
 *
 * @param[in] cmd_req   pointer to input command
 * @param[in] cmd_resp  pointer to command response
 */
static void handle_ack_nack_command(comms_command_t *cmd_in, comms_command_t *cmd_resp) {
    memset((void *)cmd_resp, 0, sizeof(comms_command_t));

    // if the command is ACK and not a reply, then respond with ACK
    if (cmd_in->header.command == COMMS_COMMON_MSG_ACK) {
        cmd_resp->header.dest_hwid   = cmd_in->header.src_hwid;
        cmd_resp->header.src_hwid    = OBC_HWID;
        cmd_resp->header.command     = COMMS_COMMON_MSG_ACK;
        cmd_resp->header.seqnum      = cmd_in->header.seqnum;
        cmd_resp->header.is_response = 1;
    }
}

/**
 * @brief Handle an OBC data command
 *
 * @param[in] cmd_req   pointer to input command
 * @param[in] cmd_resp  pointer to command response
 */
static void handle_obc_data_command(comms_command_t *cmd_in, comms_command_t *cmd_resp) {
    memset((void *)cmd_resp, 0, sizeof(comms_command_t));

    cmd_resp->header.dest_hwid   = cmd_in->header.src_hwid;
    cmd_resp->header.src_hwid    = OBC_HWID;
    cmd_resp->header.command     = COMMS_COMMON_MSG_ACK;
    cmd_resp->header.seqnum      = cmd_in->header.seqnum;
    cmd_resp->header.is_response = 1;
}
