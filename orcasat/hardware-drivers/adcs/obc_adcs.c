/**
 * @file obc_adcs.h
 * @brief CubeADCS module driver
 * @author Andres Martinez
 * @date Sep 14, 2020
 */

#include "obc_adcs.h"
#include "obc_hardwaredefs.h"
#include "logger.h"
#include "obc_utils.h"
#include "rtos_timer.h"
#include "obc_task_info.h"
#include "cube_util.h"

#define ADCS_I2C_TIMEOUT_MS 100

// I2C address of the ADCS CubeComputer
#define ADCS_I2C_ADDR 0x57

static adcs_err_t adcs_issue_i2c_submission(const i2c_command_t* command);

/**
 * @brief Creates the FreeRTOS objects required for ADCS CubeComputer module driver. This function must be done
 * before tasks call the ADCS data request functions.
 */
void adcs_create_infra(void) {
}

/**
 * Issue I2C Submission for ADCS CubeComputer
 *
 * @brief Issue an I2C submission for the ADCS CubeComputer
 *
 * @param command[in]: the command to issue to the I2C task
 * @return ADCS_SUCCESS if successful, error code otherwise
 */
static adcs_err_t adcs_issue_i2c_submission(const i2c_command_t* command) {
    result_t result = I2C_QUEUE_FULL;

    send_command_to_i2c_worker(command, I2C_WORKER_DEFAULT_PRIORITY);
    if (xQueueReceive(adcs_rx_q, (void*)&result, pdMS_TO_TICKS(ADCS_I2C_TIMEOUT_MS)) == pdFALSE) {
        log_str(ERROR, ADCS_LOG, true, "I2C queue rx timeout");
        return ADCS_I2C_ERR;
    }
    if (result != I2C_SUCCESS) {
        log_str(ERROR, ADCS_LOG, true, "I2C failure: %d", result);
        return ADCS_I2C_ERR;
    }
    return ADCS_SUCCESS;
}

/**
 * @brief Request and receive individual TLM frame from ADCS CubeComputer
 *
 * @param tlm_id[in]: TLM frame ID #
 * @param ts[out]: Buffer to store returned TLM frame. Size must be at least @ref ADCS_TLM_MAX_SIZE_BYTES
 * @param tlm_len[out]: Returned TLM frame size (bytes)
 *
 * @return Result status - ADCS_SUCCESS if frame obtained, error code otherwise
 */
adcs_err_t adcs_tlm(uint8_t tlm_id, uint8_t* tlm_data, uint16_t* tlm_len) {
    *tlm_len = lookup_tlm_len(tlm_id);
    if (*tlm_len == CUBE_INVALID_MSG_ID) {
        return ADCS_INVALID_REQ;
    }

    const i2c_command_t i2c_command = {tlm_id, ADCS_RECEIVE_QUEUE_ID, ADCS_I2C_ADDR, READ_DATA, *tlm_len, tlm_data};

    const adcs_err_t err = adcs_issue_i2c_submission(&i2c_command);

    return err;
}

/**
 * @brief Send an individual TCM frame to the ADCS CubeComputer
 *
 * @param tlm_id[in]: TLM frame ID #
 * @param ts[in]: Buffer to store returned TLM frame. Size must be at least @ref ADCS_TLM_MAX_SIZE_BYTES
 * @param tlm_len[in]: Returned TLM frame size (bytes)
 *
 * @return Result status - ADCS_SUCCESS if command sent, error code otherwise
 */
adcs_err_t adcs_tcm(uint8_t tcm_id, uint8_t* tcm_data, uint16_t tcm_len) {
    const uint16_t expected_tcm_len = lookup_tcm_len(tcm_id);
    if (expected_tcm_len != tcm_len) {
        log_str(ERROR, ADCS_LOG, true, "tcm_sz ex/rcv: %d/%d", expected_tcm_len, tcm_len);
        return ADCS_INVALID_REQ;
    }

    const i2c_command_t i2c_command = {tcm_id, ADCS_RECEIVE_QUEUE_ID, ADCS_I2C_ADDR, WRITE_DATA, tcm_len, tcm_data};

    const adcs_err_t err = adcs_issue_i2c_submission(&i2c_command);

    return err;
}

/**
 * @brief Request the TCM acknowledgment status from the ADCS CubeComputer
 *
 * @param ack_status[out]: Parsed CubeComputer TCM ACK status
 *
 * @return Result status - ADCS_SUCCESS if frame successfully received and parsed, error code otherwise
 */
adcs_err_t adcs_ack(cube_tcm_ack_t* ack_status) {
    uint8_t rx_data[4] = {0};
    uint16_t tcm_size  = 0;

    const adcs_err_t err = adcs_tlm(CubeTlm_TelecommandAcknowledge, rx_data, &tcm_size);

    if (err != ADCS_SUCCESS) {
        return err;
    }

    const cube_lib_result_t lib_err = cube_parse_tcm_ack(rx_data, ack_status);

    if (lib_err != CUBE_LIB_OK) {
        log_str(ERROR, ADCS_LOG, true, "CubeUtil error %d", ADCS_CUBE_LIB_ERR);
        return ADCS_CUBE_LIB_ERR;
    }

    return ADCS_SUCCESS;
}

/**
 * @brief Request the comms status from the ADCS CubeComputer
 *
 * @param ack_status[out]: Parsed CubeComputer comms status
 *
 * @return Result status - ADCS_SUCCESS if frame successfully received and parsed, error code otherwise
 */
adcs_err_t adcs_comms(cube_comms_status_t* comms_status) {
    uint8_t rx_data[6] = {0};
    uint16_t tcm_size  = 0;

    const adcs_err_t err = adcs_tlm(CubeTlm_CommsStatus, rx_data, &tcm_size);

    if (err != ADCS_SUCCESS) {
        return err;
    }

    const cube_lib_result_t lib_err = cube_parse_comms_status(rx_data, comms_status);

    if (lib_err != CUBE_LIB_OK) {
        log_str(ERROR, ADCS_LOG, true, "CubeUtil error %d", ADCS_CUBE_LIB_ERR);
        return ADCS_CUBE_LIB_ERR;
    }

    return ADCS_SUCCESS;
}
