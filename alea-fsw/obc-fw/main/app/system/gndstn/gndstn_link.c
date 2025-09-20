/**
 * @file gndstn_link.c
 * @brief API for the uplink/downlink between the OBC app and the ground station
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "gndstn_link.h"

// API
#include "comms_api.h"
#include "comms_defs.h"

// OBC
#include "obc_watchdog.h"
#include "obc_rtos.h"

// Logging
#include "logger.h"

// Utils
#include "rtos_stream.h"
#include "buffered_io.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Data buffer to store up to 2 datagrams
 */
#define MSG_BUFFER_SIZE (COMMS_MAX_CMD_PAYLOAD_NUM_BYTES * 2U)

/**
 * @brief Timeout for sending comms commands / receiving responses
 * TODO figure out proper value
 */
#define COMMS_TIMEOUT_MS 2000

#define GNDSTN_DOWNLINK_POLL_PERIOD_MS 1000U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void gndstn_uplink_create_infra(void);
static void gndstn_uplink_handle_data(comms_session_handle_t session_handle, comms_event_id_t ev_id, void *arg);

static void gndstn_downlink_create_infra(void);

static void gndstn_link_task(void *pvParameters);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/* UPLINK */

static MessageBufferHandle_t uplink_msg_buffer = NULL;

// FreeRTOS MessageBuffer stream that will be written to by the COMMS stack
static rtos_msg_istream_t uplink_msg_stream = {
    .msg_buf = NULL, // Assigned in gndstn_uplink_create_infra
};

static const buffered_block_istream_t uplink_raw_input_stream = {
    .handle         = &uplink_msg_stream,
    .max_block_size = COMMS_MAX_CMD_PAYLOAD_NUM_BYTES,
    .read_block     = &rtos_stream_read_msg,
};

// Buffered input stream that will be consumed by the OBC App through gndstn_uplink_socket
static uint8_t uplink_input_buf[COMMS_MAX_CMD_PAYLOAD_NUM_BYTES] = { 0 };

static buffered_input_t uplink_buffered_input = {
    .size      = sizeof(uplink_input_buf),
    .buf       = uplink_input_buf,
    .input     = &uplink_raw_input_stream,

    .start     = 0,
    .valid_len = 0,
};

/* DOWNLINK */

static MessageBufferHandle_t downlink_msg_buffer = NULL;

// FreeRTOS MessageBuffer stream that will be consumed by the downlink task
static rtos_msg_ostream_t downlink_msg_stream = {
    .msg_buf = NULL, // Assigned in gndstn_downlink_create_infra
    .mutex   = NULL,
};

static const io_ostream_t downlink_raw_output_stream = {
    .handle = &downlink_msg_stream,
    .write = &rtos_stream_write_msg,
    .flush = NULL,
};

// Buffered output stream that will be written to by the OBC App through gndstn_downlink_socket
static uint8_t downlink_output_buf[COMMS_MAX_CMD_PAYLOAD_NUM_BYTES] = { 0 };

static buffered_output_t downlink_buffered_output = {
    .size   = sizeof(downlink_output_buf),
    .buf    = downlink_output_buf,
    .output = &downlink_raw_output_stream,

    .offset = 0,
};

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

const io_istream_t gndstn_uplink_socket = {
    .handle = &uplink_buffered_input,
    .read   = &buffered_io_read,
};

const io_ostream_t gndstn_downlink_socket = {
    .handle = &downlink_buffered_output,
    .write  = &buffered_io_write,
    .flush  = &buffered_io_flush,
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void gndstn_link_create_infra(void) {
    gndstn_uplink_create_infra();
    gndstn_downlink_create_infra();
}

void gndstn_link_start_task(void) {
    obc_rtos_create_task(OBC_TASK_ID_GNDSTN_LINK, &gndstn_link_task, NULL, OBC_WATCHDOG_ACTION_ALLOW);
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

static void gndstn_uplink_create_infra(void) {
    static StaticMessageBuffer_t msg_buffer_buf            = { 0 };
    static uint8_t msg_buffer_storage[MSG_BUFFER_SIZE + 1] = { 0 }; // See https://www.freertos.org/xMessageBufferCreateStatic.html for explanation of + 1

    uplink_msg_buffer = xMessageBufferCreateStatic(MSG_BUFFER_SIZE, msg_buffer_storage, &msg_buffer_buf);

    uplink_msg_stream.msg_buf = uplink_msg_buffer;
}

static void gndstn_uplink_handle_data(comms_session_handle_t session_handle, comms_event_id_t ev_id, void *arg) {
    if (arg == NULL) {
        return;
    }

    const comms_command_t *cmd_in = (const comms_command_t *)arg;

    if (!((cmd_in->header.src_hwid == GROUND_HWID) || (cmd_in->header.src_hwid == LOCAL_HWID))) {
        // Return if the src is ground or a local pkt (From San Antonio) or from the ground station.
        return;
    }

    if (cmd_in->header.command != COMMS_CUSTOM_MSG_OBC_DATA) {
        return;
    }

    // TODO check errors and don't wait forever
    xMessageBufferSend(uplink_msg_buffer, cmd_in->data, cmd_in->data_len, portMAX_DELAY);
}

static void gndstn_downlink_create_infra(void) {
    static StaticMessageBuffer_t msg_buffer_buf            = { 0 };
    static uint8_t msg_buffer_storage[MSG_BUFFER_SIZE + 1] = { 0 }; // See https://www.freertos.org/xMessageBufferCreateStatic.html for explanation of + 1

    downlink_msg_buffer = xMessageBufferCreateStatic(MSG_BUFFER_SIZE, msg_buffer_storage, &msg_buffer_buf);

    downlink_msg_stream.msg_buf = downlink_msg_buffer;
}

/**
 * @brief Ground station link task
 *
 * @param pvParameters Task parameters (see obc_rtos)
 */
static void gndstn_link_task(void *pvParameters) {
    static uint8_t data_buf[COMMS_MAX_CMD_PAYLOAD_NUM_BYTES];

    // Setup COMMS session
    comms_err_t err;
    comms_session_handle_t comms_session;
    comms_cmd_resp_t comms_resp = {
        .result = COMMS_CMD_RESULT_OK,
        .data   = {0},
    };

    // TODO ALEA-900: We need another flag for RF comms.
#if COMMS_OVER_SERIAL
    err = comms_session_init(COMMS_ENDPOINT_GROUND, &comms_session);
#else
    err = comms_session_init(COMMS_ENDPOINT_LOCAL, &comms_session);
#endif

    if (err != COMMS_SUCCESS) {
        LOG_GNDSTN_DOWNLINK__COMMS_SESS_INIT_ERR(err);
        return;
    }

    // Register uplink listener
    comms_register_events(comms_session, (1U << COMMS_EVENT_MSG_RCV), gndstn_uplink_handle_data);

    // Start listening for downlink
    while (1) {
        obc_watchdog_pet(OBC_TASK_ID_GNDSTN_LINK);

        uint32_t data_len = xMessageBufferReceive(downlink_msg_buffer, data_buf, sizeof(data_buf), pdMS_TO_TICKS(GNDSTN_DOWNLINK_POLL_PERIOD_MS));

        if (data_len > 0) {
            // Send the command
            err = comms_send_command(comms_session, COMMS_CMD_APP_DATA, data_buf, data_len, pdMS_TO_TICKS(COMMS_TIMEOUT_MS));

            if (err != COMMS_SUCCESS) {
                LOG_GNDSTN_DOWNLINK__COMMS_SEND_CMD_ERR(err);
                continue;
            }

            // Wait for a response
            err = comms_wait_cmd_resp(comms_session, &comms_resp, pdMS_TO_TICKS(COMMS_TIMEOUT_MS));

            if (err != COMMS_SUCCESS) {
                LOG_GNDSTN_DOWNLINK__COMMS_WAIT_RESP_ERR(err);
                continue;
            }
        }
    }
}
