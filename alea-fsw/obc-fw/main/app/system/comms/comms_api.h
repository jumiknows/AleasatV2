/**
 * @file comms_api.h
 * @brief External API for comms service
 */

#ifndef COMMS_API_H_
#define COMMS_API_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                              D E F I N E S                                 */
/******************************************************************************/

#define COMMS_MAX_CMD_PAYLOAD_NUM_BYTES 226
#define COMMS_CMD_TIMEOUT_MS_DEFAULT 100

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum comms_err_type {
    /**
     * @brief No error
     */
    COMMS_SUCCESS = 0,

    /**
     * @brief OpenLST message ESP bytes incorrect
     */
    COMMS_PKT_ESP_MISMATCH_ERR = -1,

    /**
     * @brief OpenLST packet length byte out of range
     */
    COMMS_PKT_LEN_ERR = -2,

    /**
     * @brief OpenLST packet seqnum out of range
     */
    COMMS_BAD_SEQNUM_ERR = -3,

    /**
     * @brief is_response field of OpenLST packet struct out of range
     */
    COMMS_BAD_RESPONSE_BIT_FIELD_ERR = -4,

    /**
     * @brief Flashing Comms failed and likely Comms is now in bootloader only
     */
    COMMS_FLASH_FAIL = -5,

    /**
     * @brief Command timed out
     */
    COMMS_ERR_TIMEOUT = -6,

    /**
     * @brief Invalid input arguments
     */
    COMMS_ERR_INVALID_ARG = -7,

    /**
     * @brief Buffer overflow
     */
    COMMS_ERR_BUF_OVERFLOW = -8,

    /**
     * @brief No resources to handle the request
     */
    COMMS_ERR_BUSY = -9,

    /**
     * @brief Serial communication with the RF card failed
     */
    COMMS_ERR_RFC_TXN_FAIL = -10,

    /**
     * @brief Comms session invalid for this operation
     */
    COMMS_ERR_INVALID_SESSION = -11,
} comms_err_t;

/**
 * @brief Enumeration of command results
 */
typedef enum comms_cmd_result {
    COMMS_CMD_RESULT_OK = 0,
    COMMS_CMD_RESULT_ERR = -1,
} comms_cmd_result_t;

/**
 * @brief Enumeration of endpoints for comms commands
 */
typedef enum comms_endpoint_id {
    COMMS_ENDPOINT_RADIO,
    COMMS_ENDPOINT_GROUND,
    COMMS_ENDPOINT_ARO,
    COMMS_ENDPOINT_LOCAL,
    COMMS_ENDPOINT_MAX,
} comms_endpoint_id_t;

/**
 * @brief Enumeration of comms events
 */
typedef enum comms_event_id {
    COMMS_EVENT_CMD_RESP,    // received response to a previous command
    COMMS_EVENT_CMD_FAILURE, // Failed to send a command
    COMMS_EVENT_MSG_RCV,     // received a message from the GS, ARO, or radio
} comms_event_id_t;

/**
 * @brief Enumeration of comms commands
 */
typedef enum comms_command_id {
    COMMS_CMD_PING,
    COMMS_CMD_APP_DATA,
    COMMS_CMD_GET_TELEM,
    COMMS_CMD_REBOOT,
    COMMS_CMD_MAX
} comms_command_id_t;

/**
 * @brief struct containing a command result and response payload
 */
typedef struct comms_cmd_resp {
    comms_cmd_result_t result;
    uint8_t data[COMMS_MAX_CMD_PAYLOAD_NUM_BYTES];
} comms_cmd_resp_t;

typedef int comms_session_handle_t;

/**
 * @brief Function prototype for a comms event callback function
 */
typedef void (*comms_event_notify_cb)(comms_session_handle_t session_handle,
                                      comms_event_id_t ev_id,
                                      void *arg);

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

comms_err_t comms_session_init(
    comms_endpoint_id_t ep_id,
    comms_session_handle_t *session_handle
);

comms_err_t comms_register_events(
    comms_session_handle_t session_handle,
    uint32_t ev_mask,
    comms_event_notify_cb ev_cb
);

comms_err_t comms_send_command(
    comms_session_handle_t session_handle,
    comms_command_id_t cmd_id,
    uint8_t *payload,
    uint16_t payload_len,
    uint32_t timeout_ticks
);

comms_err_t comms_wait_cmd_resp(
    comms_session_handle_t session_handle,
    comms_cmd_resp_t *resp,
    uint32_t timeout_ticks
);

void comms_service_create_infra(void);

#endif
