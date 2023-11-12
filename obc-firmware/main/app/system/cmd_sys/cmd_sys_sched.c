/**
 * @file cmd_sys_sched.h
 * @brief Command System for scheduled commands
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys_sched.h"

// Command System
#include "cmd_sys.h"

// RTC Scheduler
#include "rtc_scheduler.h"

// OBC
#include "obc_rtos.h"
#include "obc_watchdog.h"

// Logger
#include "logger.h"

// Util
#include "buffered_io.h"
#include "rtos_stream.h"
#include "io_stream.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define STREAM_BUF_DEF_TRIG_LVL  1U
#define CMD_SYS_SCHED_TIMEOUT_MS 100U

#define NOTIFICATION_INDEX 1U // index 0 is used by stream/message buffers
                              // see https://www.freertos.org/RTOS-task-notifications.html

#define CMD_SYS_SCHED_POLL_PERIOD_MS  1000U
#define CMD_SYS_SCHED_EXEC_TIMEOUT_MS 60000U

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

static void scheduler_callback(uint8_t *data, uint32_t data_len);
static uint32_t write_log(void *handle, const uint8_t *data, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left);

static void cmd_sys_sched_task(void *pvParameters);
static void exec_wait_callback(void);

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

// Stream buffer for scheduled commands
static StreamBufferHandle_t cmd_sched_buffer                             = NULL;
static StaticMessageBuffer_t cmd_sched_buffer_buf                        = { 0 };
static uint8_t cmd_sched_buffer_storage[CMD_SYS_SCHED_MAX_DATA_SIZE + 1] = { 0 }; // See https://www.freertos.org/xStreamBufferCreateStatic.html for explanation of + 1

// Input Streams
static rtos_stream_istream_t input_rtos_stream = {
    .stream_buf = NULL, // Set in cmd_sys_create_infra
};

static io_istream_t input_stream = {
    .handle = &input_rtos_stream,
    .read = &rtos_stream_read_stream,
};

// Output Streams
static io_ostream_t log_output_stream = {
    .handle = NULL,
    .write = &write_log,
    .flush = NULL,
};

static uint8_t output_buf[MAX_PAYLOAD_SIZE] = { 0 };
static buffered_output_t output_buffered_stream = {
    .size   = sizeof(output_buf),
    .buf    = output_buf,
    .output = &log_output_stream,

    .offset = 0,
};

static io_ostream_t output_stream = {
    .handle = &output_buffered_stream,
    .write  = &buffered_io_write,
    .flush  = &buffered_io_flush,
};

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Create FreeRTOS infrastructure needed by this module
 */
void cmd_sys_sched_create_infra(void) {
    cmd_sched_buffer = xStreamBufferCreateStatic(CMD_SYS_SCHED_MAX_DATA_SIZE, STREAM_BUF_DEF_TRIG_LVL, cmd_sched_buffer_storage, &cmd_sched_buffer_buf);
    input_rtos_stream.stream_buf = cmd_sched_buffer;
}

/**
 * @brief Start the command system task for scheduled commands
 */
void cmd_sys_sched_start_task(void) {
    obc_rtos_create_task(OBC_TASK_ID_CMD_SYS_SCHED, &cmd_sys_sched_task, NULL, OBC_WATCHDOG_ACTION_ALLOW);
}

/**
 * @brief Push a command to the scheduler to be executed later
 * 
 * @param[in] header              Struct resulting from parsing the command header
 * @param[in] header_and_data     Pointer to a buffer containing all the command bytes (header + data)
 * @param[in] header_and_data_len Length of the buffer (header + data)
 * 
 * @return Status code:
 *            - CMD_SYS_SUCCESS if the command is successfully scheduled
 *            - CMD_SYS_ERR_INVALID_ARGS if header or header_and_data are NULL
 *            - CMD_SYS_ERR_TOO_LONG if header_and_data_len is too big
 *            - CMD_SYS_ERR_SCHED if the command cannot be scheduled
 */
cmd_sys_err_t cmd_sys_sched_push_cmd(const cmd_sys_msg_header_t *header, uint8_t *header_and_data, uint32_t header_and_data_len) {
    if ((header == NULL) || (header_and_data == NULL)) {
        return CMD_SYS_ERR_INVALID_ARGS;
    }
    if (header_and_data_len > CMD_SYS_SCHED_MAX_DATA_SIZE) {
        return CMD_SYS_ERR_ARGS_TOO_LARGE;
    }

    cmd_sys_err_t err = CMD_SYS_ERR_SCHED;

    rtc_scheduler_item_t item = {
        .timestamp = header->timestamp,
        .priority = 0, // Unused feature for now
        .cb = &scheduler_callback,
        .data_len = header_and_data_len,
        .data = header_and_data,
    };

    rtc_scheduler_err_t rtc_sched_err = rtc_scheduler_add_item(&item, pdMS_TO_TICKS(CMD_SYS_SCHED_TIMEOUT_MS));
    if (rtc_sched_err == RTC_SCHEDULER_SUCCESS) {
        err = CMD_SYS_SUCCESS;
    }

    return err;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Callback that will be invoked by the RTC scheduler when a command's
 * scheduled time has arrived.
 * 
 * @param[in] data     Raw command header + data
 * @param[in] data_len Number of bytes in data
 */
static void scheduler_callback(uint8_t *data, uint32_t data_len) {
    if ((data != NULL) && (data_len > 0)) {
        // Write the command to the stream buffer to wake up cmd_sys_sched_task
        xStreamBufferSend(cmd_sched_buffer, data, data_len, pdMS_TO_TICKS(CMD_SYS_SCHED_TIMEOUT_MS)); // TODO ALEA-857 handle timeout
    }
}

/**
 * @brief io_ostream_t compatible API to write data to the logs.
 * 
 * The handle is ignored.
 * 
 * See io_ostream_t in io_stream.h for details.
 */
static uint32_t write_log(void *handle, const uint8_t *data, uint32_t num_bytes, uint32_t timeout, uint32_t *timeout_left) {
    // This is a temporary way to send scheduled command responses.
    // In reality the responses should be stored to the filesystem.
    // See ALEA-858
    // TODO ALEA-572 Write this to filesystem
    LOG_CMD_SYS_SCHED_RESP(num_bytes, data);
    if (timeout_left != NULL) {
        *timeout_left = timeout;
    }
    return num_bytes;
}

/**
 * @brief Command system task for scheduled commands
 * 
 * @param pvParameters Task parameters (unused)
 */
static void cmd_sys_sched_task(void* pvParameters) {
    static uint8_t buf[CMD_SYS_SCHED_MAX_DATA_SIZE] = { 0 };

    static cmd_sys_cmd_t cmd = { 0 };
    cmd.input = &input_stream;
    cmd.output = &output_stream;

    while (1) {
        obc_watchdog_pet(OBC_TASK_ID_CMD_SYS_SCHED);

        cmd_sys_err_t err = cmd_sys_recv_header(&cmd, buf, pdMS_TO_TICKS(CMD_SYS_SCHED_POLL_PERIOD_MS));
        if (err == CMD_SYS_SUCCESS) {
            err = cmd_sys_execute(&cmd, pdMS_TO_TICKS(CMD_SYS_SCHED_POLL_PERIOD_MS), pdMS_TO_TICKS(CMD_SYS_SCHED_EXEC_TIMEOUT_MS), &exec_wait_callback);

            // TODO: ALEA-857 do something with err
        }
    }
}

static void exec_wait_callback(void) {
    // Pet the watchdog while we're waiting for the command to finish executing
    obc_watchdog_pet(OBC_TASK_ID_CMD_SYS_SCHED);
}
