/**
 * @file cmd_sys.c
 * @brief Command System
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys.h"

// Command System
#include "cmd_sys_exec.h"
#include "cmd_sys_sched.h"
#include "cmd_sys_gen.h"

// Utils
#include "data_fmt.h"
#include "io_stream.h"
#include "obc_utils.h"

// RTOS
#include "rtos.h"

// Standard Library
#include <stddef.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

/**
 * @brief Serialized header size in bytes
 */
#define CMD_SYS_MSG_HEADER_LEN       12U

/**
 * @brief Serialized response data header size in bytes
 */
#define CMD_SYS_RESP_DATA_HEADER_LEN 5U

/**
 * @brief Bit to set in the data_len field of the header to indicate the message
 * is a response (not a command)
 */
#define CMD_SYS_RESPONSE_BIT         UINT32_BIT(31)

#define NOTIFICATION_INDEX 1U // index 0 is used by stream/message buffers
                              // see https://www.freertos.org/RTOS-task-notifications.html

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void cmd_sys_parse_header(const uint8_t *buf, cmd_sys_msg_header_t *header);
static void cmd_sys_exec_callback(cmd_sys_err_t status, void *arg);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Wait to receive a command header on the command input stream.
 * 
 * If a command header is successfully received, the cmd->header struct will be populated
 * with the parsed header data.
 * 
 * @param[in]  cmd                 Pointer to the command struct. This CANNOT be allocated on a task stack as it's passed to another task.
 *                                 The input and output streams must be assigned.
 * @param[in]  buf                 Pointer to a buffer to store the command header. Must be at least CMD_SYS_SCHED_MAX_DATA_SIZE bytes.
 * @param[in]  poll_period_ticks   Maximum amount of time to wait for the start of a command
 * 
 * @return Status code:
 *            - CMD_SYS_SUCCESS if a header is successfully received and parsed
 *            - CMD_SYS_ERR_INVALID_ARGS if cmd, cmd->input or cmd->output is NULL
 *            - CMD_SYS_ERR_NO_HEADER if no data is received before poll_period_ticks elapses
 *            - CMD_SYS_ERR_READ_TIMEOUT if the full header is not received within CMD_SYS_INPUT_READ_TIMEOUT_MS of the start of the header
 */
cmd_sys_err_t cmd_sys_recv_header(cmd_sys_cmd_t *cmd, uint8_t *buf, uint32_t poll_period_ticks) {
    if ((cmd == NULL) || (cmd->input == NULL) || (cmd->output == NULL)) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    // Read a single byte to detect the start of a command
    uint32_t bytes_read = io_stream_read(cmd->input, buf, 1, poll_period_ticks, NULL);
    if (bytes_read != 1) {
        return CMD_SYS_ERR_NO_HEADER;
    }

    // Read rest of the header
    bytes_read += io_stream_read(cmd->input, &buf[1], (CMD_SYS_MSG_HEADER_LEN - 1), pdMS_TO_TICKS(CMD_SYS_INPUT_READ_TIMEOUT_MS), NULL);
    if (bytes_read != CMD_SYS_MSG_HEADER_LEN) {
        return CMD_SYS_ERR_READ_TIMEOUT;
    }

    // Parse header
    cmd_sys_parse_header(buf, &cmd->header);

    return CMD_SYS_SUCCESS;
}

/**
 * @brief Schedule a command for execution later.
 * 
 * @param[in] cmd Pointer to the command struct. The header should already be parsed and populated.
 * @param[in] buf Pointer to a buffer containing the header raw bytes and enough remaining space
 *                to store the data bytes (for a total of up to CMD_SYS_SCHED_MAX_DATA_SIZE bytes)
 * 
 * @return Status code:
 *            - CMD_SYS_ERR_INVALID_ARGS if cmd, cmd->input or cmd->output is NULL
 *            - CMD_SYS_ERR_ARGS_TOO_LARGE if command header data_len is too long
 *            - CMD_SYS_ERR_READ_TIMEOUT if an error occurs reading the remaining command data
 *            - otherwise the status from cmd_sys_begin_response or cmd_sys_finish_response is returned
 */
cmd_sys_err_t cmd_sys_schedule_cmd(const cmd_sys_cmd_t *cmd, uint8_t *buf) {
    if ((cmd == NULL) || (cmd->input == NULL) || (cmd->output == NULL)) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    // Scheduled command - check data_len
    if (cmd->header.data_len > (CMD_SYS_SCHED_MAX_DATA_SIZE - CMD_SYS_MSG_HEADER_LEN)) {
        // TODO: ALEA-863 stream the data to a file or something if it's too big
        return CMD_SYS_ERR_ARGS_TOO_LARGE;
    }

    // Read the rest of the data into the buf
    uint32_t data_len = cmd->header.data_len;
    if (data_len > 0) {
        uint32_t bytes_read = io_stream_read(cmd->input, &(buf[CMD_SYS_MSG_HEADER_LEN]), data_len, portMAX_DELAY, NULL);
        if (bytes_read != data_len) {
            return CMD_SYS_ERR_READ_TIMEOUT;
        }
    }

    cmd_sys_resp_code_t resp_code = CMD_SYS_RESP_CODE_ERROR;

    // Schedule the command
    cmd_sys_err_t err = cmd_sys_sched_push_cmd(&(cmd->header), buf, (CMD_SYS_MSG_HEADER_LEN + data_len));
    if (err == CMD_SYS_SUCCESS) {
        resp_code = CMD_SYS_RESP_CODE_SUCCESS_SCHED;
    }

    // Send an immediate response
    err = cmd_sys_begin_response(cmd, resp_code, 0);
    if (err != CMD_SYS_SUCCESS) {
        return err;
    }
    err = cmd_sys_finish_response(cmd);

    return err;
}

/**
 * @brief Send a command to the cmd_sys_exec task for execution and wait for it to complete.
 * 
 * @param[in] cmd               Pointer to the command struct. The header should already be parsed and populated
 * @param[in] poll_period_ticks Polling period when checking if the command execution has completed
 * @param[in] timeout_ticks     Maximum total amount of time to wait for the command to finish executing
 * @param[in] wait_callback     Optional callback function that will be called every time poll_period_ticks elapses
 * 
 * @return Status code:
 *            - CMD_SYS_ERR_INVALID_ARGS if cmd, cmd->input or cmd->output is NULL
 *            - CMD_SYS_ERR_EXEC_Q_TIMEOUT if there is not enough space in the exec queue
 *            - otherwise the status returned from the execution of the command
 */
cmd_sys_err_t cmd_sys_execute(cmd_sys_cmd_t *cmd, uint32_t poll_period_ticks, uint32_t timeout_ticks, cmd_sys_exec_wait_cb_t wait_callback) {
    if ((cmd == NULL) || (cmd->input == NULL) || (cmd->output == NULL)) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    // Send command to cmd_sys_exec task
    TaskHandle_t task_handle = xTaskGetCurrentTaskHandle();
    cmd_sys_err_t err = cmd_sys_exec_enqueue(cmd, &cmd_sys_exec_callback, task_handle, 0);
    if (err != CMD_SYS_SUCCESS) {
        return err;
    }

    // Wait for a notification from the cmd_sys_exec task
    uint32_t timeout_left_ticks = timeout_ticks;
    uint32_t notification_value = 0;

    err = CMD_SYS_ERR_EXEC_TIMEOUT;
    while (timeout_left_ticks > 0) {
        if (wait_callback != NULL) {
            wait_callback();
        }

        uint32_t loop_timeout_ticks = MIN(timeout_left_ticks, poll_period_ticks);
        if (xTaskNotifyWaitIndexed(NOTIFICATION_INDEX, 0U, 0xFFFFFFFFU, &notification_value, loop_timeout_ticks) == pdTRUE) {
            // Notification received --> command execution complete
            err = (cmd_sys_err_t)notification_value;
            break;
        } else {
            // No notification --> update timeout
            timeout_left_ticks -= loop_timeout_ticks;
        }
    }
    return err;
}

/**
 * @brief Invoke a command. The command will run synchronously in the current task.
 * 
 * @param[in] cmd Pointer to a fully populated command struct.
 * 
 * @return Status code:
 *            - CMD_SYS_ERR_INVALID_CMD if the cmd ID is invalid
 *            - CMD_SYS_ERR_CMD_DNE if the cmd ID does not exist
 *            - otherwise the status from the command function is returned
 */
cmd_sys_err_t cmd_sys_invoke_cmd(const cmd_sys_cmd_t *cmd) {
    if (cmd->header.cmd_id >= LEN(CMD_SPEC_TABLE)) {
        return CMD_SYS_ERR_INVALID_CMD;
    }

    const cmd_sys_cmd_spec_t *cmd_spec = &CMD_SPEC_TABLE[cmd->header.cmd_id];
    if ((cmd_spec == NULL) || (cmd_spec->invoke == NULL)) {
        cmd_sys_begin_response(cmd, CMD_SYS_RESP_CODE_CMD_DNE, 0);
        cmd_sys_finish_response(cmd);
        return CMD_SYS_ERR_CMD_DNE;
    }

    return cmd_spec->invoke(cmd, cmd_spec->args, cmd_spec->resp);
}

/**
 * @brief Send the beginning of a response to a command
 * 
 * @param[in] cmd            Pointer to command struct
 * @param[in] resp_code      Response code to send
 * @param[in] resp_data_len  Length of data to be sent after the header and response code
 * 
 * @return Status code:
 *            - CMD_SYS_SUCCESS if the beginning of the response is written successfully
 *            - CMD_SYS_ERR_INVALID_ARGS if cmd is NULL
 *            - CMD_SYS_ERR_RESP_TOO_LARGE if resp_data_len is too large
 *            - CMD_SYS_ERR_WRITE_TIMEOUT if an error occurs writing to the resp stream
 */
cmd_sys_err_t cmd_sys_begin_response(const cmd_sys_cmd_t *cmd, cmd_sys_resp_code_t resp_code, uint32_t resp_data_len) {
    if (cmd == NULL) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }
    resp_data_len += CMD_SYS_RESP_DATA_HEADER_LEN;
    if (resp_data_len > (CMD_SYS_RESPONSE_BIT - 1)) {
        return CMD_SYS_ERR_RESP_TOO_LARGE;
    }

    uint8_t buf[CMD_SYS_MSG_HEADER_LEN + CMD_SYS_RESP_DATA_HEADER_LEN] = { 0 };
    uint8_t idx = 0;

    // Populate message header
    buf[idx++] = cmd->header.seq_num;
    buf[idx++] = cmd->header.cmd_id;
    data_fmt_u32_to_arr_be(cmd->header.timestamp                 , &buf[idx]); idx += sizeof(cmd->header.timestamp);
    data_fmt_u16_to_arr_be(cmd->header.flags                     , &buf[idx]); idx += sizeof(cmd->header.flags);
    data_fmt_u32_to_arr_be((resp_data_len | CMD_SYS_RESPONSE_BIT), &buf[idx]); idx += sizeof(resp_data_len);

    // Populate response data header
    buf[idx++] = (uint8_t)resp_code;
    data_fmt_u32_to_arr_be(cmd->exec_timestamp, &buf[idx]);

    uint32_t written = io_stream_write(cmd->output, buf, sizeof(buf), pdMS_TO_TICKS(CMD_SYS_OUTPUT_WRITE_TIMEOUT_MS), NULL);
    if (written != sizeof(buf)) {
        return CMD_SYS_ERR_WRITE_TIMEOUT;
    }

    return CMD_SYS_SUCCESS;
}

/**
 * @brief Finish a response by flushing the output stream
 * 
 * @param[in] cmd Pointer to command struct
 * 
 * @return Status code:
 *            - CMD_SYS_SUCCESS if the response is successfully complete
 *            - CMD_SYS_ERR_INVALID_ARGS if cmd is NULL
 */
cmd_sys_err_t cmd_sys_finish_response(const cmd_sys_cmd_t *cmd) {
    if (cmd == NULL) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    io_stream_flush(cmd->output, pdMS_TO_TICKS(CMD_SYS_OUTPUT_WRITE_TIMEOUT_MS), NULL);

    return CMD_SYS_SUCCESS;
}

/**
 * @brief Deserialize arguments for a command
 * 
 * @param[in]  cmd         Pointer to command struct (with input and output streams)
 * @param[out] args_struct Pointer to where parsed arguments will be stored.
 *                         The object pointed to must match the description in args_desc.
 * @param[in]  args_desc   Pointer to description of the argument struct for deserialization using the data_fmt module
 * @param[in]  args_len    Number of bytes for the serialized args
 * @param[in]  buf         Buffer into which arguments will be read. Must be at least as large as args_len.
 * 
 * @return Status code:
 *            - CMD_SYS_SUCCESS if the argument deserialization was successful
 *            - CMD_SYS_ERR_INVALID_ARGS if cmd, args_struct, args_desc or buf are NULL
 *            - CMD_SYS_ERR_READ_TIMEOUT if reading from the input stream time's out
 *            - CMD_SYS_ERR_DATA_FMT if the arguments cannot be properly deserialized
 */
cmd_sys_err_t cmd_sys_handle_args(const cmd_sys_cmd_t *cmd, void *args_struct, const data_fmt_desc_t *args_desc, uint32_t args_len, uint8_t *buf) {
    if ((cmd == NULL) || (args_struct == NULL) || (args_desc == NULL) || (buf == NULL)) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    // Read the args
    uint32_t read_bytes = io_stream_read(cmd->input, buf, args_len, pdMS_TO_TICKS(CMD_SYS_INPUT_READ_TIMEOUT_MS), NULL);
    if (read_bytes != args_len) {
        return CMD_SYS_ERR_READ_TIMEOUT;
    }

    // Deserialize the args
    read_bytes = data_fmt_deserialize_struct(args_struct, args_desc, buf, args_len);
    if (read_bytes != args_len) {
        return CMD_SYS_ERR_DATA_FMT;
    }

    return CMD_SYS_SUCCESS;
}

/**
 * @brief Begin a response, serialize and send response field data and finish the response
 * 
 * @param[in] cmd         Pointer to command struct (with input and output streams)
 * @param[in] resp_struct Pointer struct containing response data
 * @param[in] resp_desc   Pointer to description of the response data struct for serialization using the data_fmt module
 * @param[in] resp_len    Number of bytes for the serialized response data
 * @param[in] buf         Buffer used to store serialized response data. Must be at least as large as resp_len.
 * @param[in] resp_code   Response status code to send
 * 
 * @return Status code:
 *            - CMD_SYS_ERR_INVALID_ARGS if cmd, resp_struct, resp_desc or buf are NULL
 *            - CMD_SYS_ERR_WRITE_TIMEOUT if writing to the output stream time's out
 *            - CMD_SYS_ERR_DATA_FMT if the response cannot be properly serialized
 *            - otherwise the response from cmd_sys_begin_response or cmd_sys_finish_response will be returned
 */
cmd_sys_err_t cmd_sys_handle_resp(const cmd_sys_cmd_t *cmd, const void *resp_struct, const data_fmt_desc_t *resp_desc, uint32_t resp_len, uint8_t *buf, cmd_sys_resp_code_t resp_code) {
    if (cmd == NULL) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    // Send the response headers
    cmd_sys_err_t err = cmd_sys_begin_response(cmd, resp_code, resp_len);
    if (err != CMD_SYS_SUCCESS) {
        return err;
    }

    // Send response data if there is any
    if ((resp_struct != NULL) && (resp_desc != NULL) && (buf != NULL)) {
        err = cmd_sys_handle_resp_fields(cmd, resp_struct, resp_desc, resp_len, buf);
        if (err != CMD_SYS_SUCCESS) {
            return err;
        }
    }

    // Flush output
    err = cmd_sys_finish_response(cmd);
    return err;
}

/**
 * @brief Serialize and send response field data
 * 
 * @param[in] cmd         Pointer to command struct (with input and output streams)
 * @param[in] resp_struct Pointer struct containing response data
 * @param[in] resp_desc   Pointer to description of the response data struct for serialization using the data_fmt module
 * @param[in] resp_len    Number of bytes for the serialized response data
 * @param[in] buf         Buffer used to store serialized response data. Must be at least as large as resp_len.
 * 
 * @return Status code:
 *            - CMD_SYS_ERR_INVALID_ARGS if cmd, resp_struct, resp_desc or buf are NULL
 *            - CMD_SYS_ERR_WRITE_TIMEOUT if writing to the output stream time's out
 *            - CMD_SYS_ERR_DATA_FMT if the response cannot be properly serialized
 *            - otherwise the response from cmd_sys_begin_response or cmd_sys_finish_response will be returned
 */
cmd_sys_err_t cmd_sys_handle_resp_fields(const cmd_sys_cmd_t *cmd, const void *resp_struct, const data_fmt_desc_t *resp_desc, uint32_t resp_len, uint8_t *buf) {
    // Check arguments
    if (cmd == NULL) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }
    if ((resp_struct == NULL) && (resp_desc == NULL) && (buf == NULL)) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    // Serialize the response
    uint32_t written_bytes = data_fmt_serialize_struct(resp_struct, resp_desc, buf, resp_len);
    if (written_bytes != resp_len) {
        return CMD_SYS_ERR_DATA_FMT;
    }

    // Send the response
    written_bytes = io_stream_write(cmd->output, buf, resp_len, pdMS_TO_TICKS(CMD_SYS_OUTPUT_WRITE_TIMEOUT_MS), NULL);
    if (written_bytes != resp_len) {
        return CMD_SYS_ERR_WRITE_TIMEOUT;
    }

    return CMD_SYS_SUCCESS;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Parse the header from a command
 * 
 * @param[in]  buf     Pointer to a buffer containing the command header bytes.
 *                     Must be at least CMD_SYS_MSG_HEADER_LEN bytes.
 * @param[out] header  Pointer to header struct to populate
 * 
 * @return Status code:
 *            - CMD_SYS_SUCCESS if the header was parsed successfully
 *            - CMD_SYS_ERR_BUF_TOO_SMALL if the buffer is too small
 */
static void cmd_sys_parse_header(const uint8_t *buf, cmd_sys_msg_header_t *header) {
    header->seq_num   = buf[0];
    header->cmd_id    = buf[1];
    header->timestamp = data_fmt_arr_be_to_u32((buf + 2));
    header->flags     = data_fmt_arr_be_to_u16((buf + 6));
    header->data_len  = data_fmt_arr_be_to_u32((buf + 8));
    header->data_len  &= ~(CMD_SYS_RESPONSE_BIT);
}

static void cmd_sys_exec_callback(cmd_sys_err_t status, void *arg) {
    TaskHandle_t task_handle = (TaskHandle_t)arg;
    xTaskNotifyIndexed(task_handle, NOTIFICATION_INDEX, (uint32_t)status, eSetValueWithOverwrite);
}
