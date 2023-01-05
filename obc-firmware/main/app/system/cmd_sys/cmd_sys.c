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
 * @brief Bit to set in the data_len field of the header to indicate the message
 * is a response (not a command)
 */
#define CMD_SYS_RESPONSE_BIT         UINT32_BIT(31)

/**
 * @brief Timestamp used to indicate immediate (rather than scheduled) commands
 */
#define CMD_SYS_TIMESTAMP_IMMEDIATE  ((uint32_t)0U)

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void cmd_sys_parse_header(const uint8_t *buf, cmd_sys_msg_header_t *header);
static cmd_sys_err_t cmd_sys_schedule_cmd(const cmd_sys_cmd_t *cmd, uint8_t *buf);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Run the command system. This can be called from any number of FreeRTOS tasks
 * as long as they manage separation / synchronization of the input/output streams.
 * 
 * 1.  Listen for data on the cmd->input stream until CMD_SYS_MSG_HEADER_LEN bytes are received
 * 2.  Parse the message header
 * 3a. For an immediate command, send the command to the cmd_sys_exec task for immediate execution
 * 3b. For a scheduled command, read the rest of the command data and schedule it^
 * 
 * ^If invoke_scheduled is true then scheduled commands will be invoked instead of scheduled.
 * 
 * This function is fully blocking (it will wait indefinitely for enough bytes to handle a single command),
 * but will return after every command so it should be called in a superloop.
 * 
 * @param[in]  cmd              Pointer to the command struct
 * @param[in]  buf              Pointer to a buffer to store the command header. Must be at least CMD_SYS_SCHED_MAX_DATA_SIZE bytes.
 * @param[in]  callback         Function that will be called by cmd_sys_exec after the command is executed
 * @param[in]  invoke_scheduled Set to true to invoke scheduled commands rather than schedule them
 * @param[out] queued_for_exec  Will be set to true if a command is queued for execution during this call
 * 
 * @return Status code:
 *            - CMD_SYS_ERR_INVALID_ARGS if input is NULL or output is NULL
 *            - CMD_SYS_ERR_READ_TIMEOUT if an error occurs reading from the input stream
 *            - otherwise the status from cmd_sys_exec_enqueue (immediate) or cmd_sys_sched_push_cmd (scheduled) is returned
 */
cmd_sys_err_t cmd_sys_run(cmd_sys_cmd_t *cmd, uint8_t *buf, cmd_sys_callback_t callback, bool invoke_scheduled, bool *queued_for_exec) {
    if ((cmd == NULL) && (queued_for_exec == NULL)) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }

    *queued_for_exec = false;

    // Read header
    uint32_t bytes_read = io_stream_read(cmd->input, buf, CMD_SYS_MSG_HEADER_LEN, portMAX_DELAY, NULL);
    if (bytes_read != CMD_SYS_MSG_HEADER_LEN) {
        return CMD_SYS_ERR_READ_TIMEOUT;
    }

    // Parse header
    cmd_sys_parse_header(buf, &cmd->header);

    if ((cmd->header.timestamp == CMD_SYS_TIMESTAMP_IMMEDIATE) || invoke_scheduled) {
        // Immediate command - send to executor right away
        cmd_sys_err_t err = cmd_sys_exec_enqueue(cmd, callback, portMAX_DELAY);
        if (err == CMD_SYS_SUCCESS) {
            *queued_for_exec = true;
        }
        return err;
    } else {
        // Scheduled command
        return cmd_sys_schedule_cmd(cmd, buf);
    }
}

/**
 * @brief Invoke a command. The command will run synchronously in the current task.
 * 
 * @param[in] cmd Pointer to the command struct. The header should already be parsed and populated.
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
    resp_data_len += 1; // one extra byte for resp_code
    if (resp_data_len > (CMD_SYS_RESPONSE_BIT - 1)) {
        return CMD_SYS_ERR_RESP_TOO_LARGE;
    }

    uint8_t buf[CMD_SYS_MSG_HEADER_LEN + 1] = { 0 }; // one extra byte for resp_code
    buf[0] = cmd->header.seq_num;
    buf[1] = cmd->header.cmd_id;
    data_fmt_u32_to_arr_be(cmd->header.timestamp, (buf + 2));
    data_fmt_u16_to_arr_be(cmd->header.flags, (buf + 6));
    resp_data_len |= CMD_SYS_RESPONSE_BIT;
    data_fmt_u32_to_arr_be(resp_data_len, (buf + 8));

    buf[12] = (uint8_t)resp_code;

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

/**
 * @brief Schedule a command for execution later.
 * 
 * @param[in] cmd Pointer to the command struct. The header should already be parsed and populated.
 * @param[in] buf Pointer to a buffer containing the header raw bytes and enough remaining space
 *                to store the data bytes (for a total of up to CMD_SYS_SCHED_MAX_DATA_SIZE bytes)
 * 
 * @return Status code:
 *            - CMD_SYS_ERR_ARGS_TOO_LARGE if command header data_len is too long
 *            - CMD_SYS_ERR_READ_TIMEOUT if an error occurs reading the remaining command data
 *            - otherwise the status from cmd_sys_begin_response or cmd_sys_finish_response is returned
 */
static cmd_sys_err_t cmd_sys_schedule_cmd(const cmd_sys_cmd_t *cmd, uint8_t *buf) {
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

    // TODO schedule the command

    // Send an immediate response
    cmd_sys_resp_code_t resp_code = CMD_SYS_RESP_CODE_ERROR; // For now always send an error since scheduling is not supported
    cmd_sys_err_t err = cmd_sys_begin_response(cmd, resp_code, 0);
    if (err != CMD_SYS_SUCCESS) {
        return err;
    }

    err = cmd_sys_finish_response(cmd);
    return err;
}
