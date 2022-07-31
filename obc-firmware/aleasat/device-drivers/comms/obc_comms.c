/**
 * @file obc_comms.c
 * @brief Low-level OBC to Comms driver using MIBSPI
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_defs.h"
#include "comms_utils.h"
#include "obc_comms.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_watchdog.h"
#include "obc_task_info.h"

// Logging
#include "logger.h"

// FreeRTOS
#include "rtos.h"

// HAL
#include "het.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// Any MIBSPI transaction should not take more than this value
#define COMMS_MIBSPI_TIMEOUT_MS 30

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

/**
 * @brief Definition of the MIBSPI transfer groups available for use when
 * communicating with the comms board.
 */
static const mibspi_tg_t COMMS_256_BYTES_TG = {
    .reg = COMMS_MIBSPI_REG,
    .transfer_group = COMMS_TRANSFER_GROUP,
    .cs_port = COMMS_CS_PORT,
    .cs_pin = COMMS_CS_PIN
};

static comms_waiter_match_params_t comms_waiter_match_params = {0};
// Mutex to protect comms_waiter_match_params
static SemaphoreHandle_t xCommsWaiterMutexHandle;

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

// Comms event group handle
EventGroupHandle_t xCommsWaiterEventGroupHandle;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

bool comms_try_waiter_match(comms_command_t* comms_cmd, comms_waiter_match_params_t* match_params);

void vCommsInterruptServiceTask(void* pvParameters);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the mibSPI peripheral and enables the appropriate transfer group interrupts.
 */
void comms_create_infra(void) {
    static StaticEventGroup_t xCommsWaiterStaticEventGroup;
    static StaticSemaphore_t xCommsWaiterStaticMutex;

    // Initialize RTOS event group
    xCommsWaiterEventGroupHandle = xEventGroupCreateStatic(&xCommsWaiterStaticEventGroup);

    // Initialize comms waiter mutex
    xCommsWaiterMutexHandle = xSemaphoreCreateMutexStatic(&xCommsWaiterStaticMutex);
}

/**
 * @brief Enables MIBSPI and COMMS_INT interrupts.
 */
void comms_init_irq(void) {
    mibspiEnableGroupNotification(COMMS_MIBSPI_REG, COMMS_TRANSFER_GROUP, COMMS_DATA_FORMAT);
    edgeEnableNotification(COMMS_INT_REG, COMMS_INT_EDGE);
}

/**
 * @brief Starts the Comms interrupt processing task.
 */
void comms_interrupt_start_task(void) {
    static StackType_t xCommsInterruptTaskStack[COMMS_IRQ_TASK_STACK_SIZE];
    static StaticTask_t xCommsInterruptTaskBuffer;
    task_create_static(
        &vCommsInterruptServiceTask,
        "comms_irq",
        COMMS_IRQ_TASK_STACK_SIZE,
        NULL,
        COMMS_IRQ_SERVICE_PRIORITY,
        xCommsInterruptTaskStack,
        &xCommsInterruptTaskBuffer
    );
}

/**
 * @brief Unblocks Comms interrupt task
 *
 * @warning This is called in ISR context, must use ISR versions of RTOS functions
 */
void notify_comms_irq(void) {
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    
    OBC_MISRA_CHECK_OFF
    /* Removing MISRA error 16.9 because it asks to modify FreeRTOS code 
     *  (function "vEventGroupSetBitsCallback" in event_groups.h)"
     */
    xEventGroupSetBitsFromISR(xCommsWaiterEventGroupHandle, COMMS_RX_EVENT_UNBLOCK_INT_TASK_BIT, &xHigherPriorityTaskWoken);
    OBC_MISRA_CHECK_ON

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Transmit 256 bytes to Comms.
 *
 * @pre @ref tms_mibspi_init_hw
 * @pre comms_init_irq
 * @pre comms_create_infra
 *
 * @param[in] tx_buffer pointer to the data to transmit, MUST be 256 bytes long
 *
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t comms_mibspi_tx(uint16_t* tx_buffer) {
    return tms_mibspi_tx(&COMMS_256_BYTES_TG, tx_buffer, COMMS_MIBSPI_TIMEOUT_MS);
}

/**
 * @brief Receive 256 bytes from Comms.
 *
 * @pre @ref tms_mibspi_init_hw
 * @pre comms_init_irq
 * @pre comms_create_infra
 *
 * @param[out] rx_buffer pointer to the data to receive, MUST be 256 bytes long
 *
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t comms_mibspi_rx(uint16_t* rx_buffer) {
    return tms_mibspi_rx(&COMMS_256_BYTES_TG, rx_buffer, COMMS_MIBSPI_TIMEOUT_MS);
}

/**
 * @brief Sets flag indicating Comms waiter is active with given parameters
 *
 * @return COMMS_SUCCESS on success, COMMS_WAITER_MUTEX_TIMEOUT if taking mutex timed out
 */
comms_err_t comms_set_waiter_match(
    comms_match_spec_t match_is_response_spec,
    uint8_t match_is_response,
    comms_match_spec_t match_seqnum_spec,
    uint16_t match_seqnum,
    comms_match_spec_t match_src_hwid_spec,
    hwid_t match_src_hwid,
    comms_match_spec_t match_cmd_num_spec,
    uint8_t match_cmd_num,
    comms_command_t* cmd_ptr
) {
    if (match_seqnum > 0x7FFF) {
        return COMMS_BAD_SEQNUM_ERR;
    }

    if (match_is_response > 1) {
        return COMMS_BAD_RESPONSE_BIT_FIELD_ERR;
    }

    if (xSemaphoreTake(xCommsWaiterMutexHandle, pdMS_TO_TICKS(COMMS_WAITER_MUTEX_TIMEOUT_MS)) == pdTRUE) {
        comms_waiter_match_params.waiter_present = true;
        comms_waiter_match_params.match_is_response_spec = match_is_response_spec;
        comms_waiter_match_params.match_is_response = match_is_response;
        comms_waiter_match_params.match_seqnum_spec = match_seqnum_spec;
        comms_waiter_match_params.match_seqnum = match_seqnum;
        comms_waiter_match_params.match_src_hwid_spec = match_src_hwid_spec;
        comms_waiter_match_params.match_src_hwid = match_src_hwid;
        comms_waiter_match_params.match_cmd_num_spec = match_cmd_num_spec;
        comms_waiter_match_params.match_cmd_num = match_cmd_num;
        comms_waiter_match_params.cmd_ptr = cmd_ptr;

        xSemaphoreGive(xCommsWaiterMutexHandle);
        return COMMS_SUCCESS;
    }
    else {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms resp mtx t-out 1");
        return COMMS_WAITER_MUTEX_TIMEOUT;
    }
}

/**
 * @brief Sets flag indicating Comms waiter is active with pointer to parameters
 *
 * @return COMMS_SUCCESS on success, COMMS_WAITER_MUTEX_TIMEOUT if taking mutex timed out
 */
comms_err_t comms_set_waiter_match_struct(comms_waiter_match_params_t* match_params) {
    if (match_params->match_seqnum > 0x7FFF) {
        return COMMS_BAD_SEQNUM_ERR;
    }

    if (match_params->match_is_response > 1) {
        return COMMS_BAD_RESPONSE_BIT_FIELD_ERR;
    }

    if (xSemaphoreTake(xCommsWaiterMutexHandle, pdMS_TO_TICKS(COMMS_WAITER_MUTEX_TIMEOUT_MS)) == pdTRUE) {
        comms_waiter_match_params = *match_params;
        comms_waiter_match_params.waiter_present = true;
        xSemaphoreGive(xCommsWaiterMutexHandle);
        return COMMS_SUCCESS;
    }
    else {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms resp mtx t-out 3");
        return COMMS_WAITER_MUTEX_TIMEOUT;
    }
}

/**
 * @brief Clears flag indicating Comms waiter is active and zeros response parameters
 *
 * @return COMMS_SUCCESS on success, COMMS_WAITER_MUTEX_TIMEOUT if taking mutex timed out
 */
comms_err_t comms_clear_waiter_match(void) {
    if (xSemaphoreTake(xCommsWaiterMutexHandle, pdMS_TO_TICKS(COMMS_WAITER_MUTEX_TIMEOUT_MS)) == pdTRUE) {
        comms_waiter_match_params.waiter_present = false;
        comms_waiter_match_params.match_is_response_spec = COMMS_MATCH_ANY;
        comms_waiter_match_params.match_is_response = 0;
        comms_waiter_match_params.match_seqnum_spec = COMMS_MATCH_ANY;
        comms_waiter_match_params.match_seqnum = 0;
        comms_waiter_match_params.match_src_hwid_spec = COMMS_MATCH_ANY;
        comms_waiter_match_params.match_src_hwid = 0;
        comms_waiter_match_params.match_cmd_num_spec = COMMS_MATCH_ANY;
        comms_waiter_match_params.match_cmd_num = 0;
        comms_waiter_match_params.cmd_ptr = NULL;

        xSemaphoreGive(xCommsWaiterMutexHandle);
        return COMMS_SUCCESS;
    }
    else {
        log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms resp mtx t-out 2");
        return COMMS_WAITER_MUTEX_TIMEOUT;
    }
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Comms interrupt handler task.
 *
 * @param[in] pvParameters used implicitly for watchdog handling.
 */
void vCommsInterruptServiceTask(void* pvParameters) {
    static uint16_t data[128] = {0x0000};
    static uint16_t reply[128] = {0x0000};
    static comms_command_t comms_cmd_buf = {0};
    static comms_command_t comms_reply_buf = {0};
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);
    EventBits_t uxBits;
    mibspi_err_t mibspi_ret = MIBSPI_UNKNOWN_ERR;
    comms_err_t comms_err = COMMS_UNKNOWN_ERR;

    while (1) {
        set_task_status(wd_task_id, task_asleep);
        // Block until notified from ISR
        uxBits = xEventGroupWaitBits(xCommsWaiterEventGroupHandle, COMMS_RX_EVENT_UNBLOCK_INT_TASK_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
        set_task_status(wd_task_id, task_alive);

        do {
            if ((uxBits & COMMS_RX_EVENT_UNBLOCK_INT_TASK_BIT) == 0) {
                // timed out, wait again
                break;
            }

            mibspi_ret = comms_mibspi_rx(data);
            if (mibspi_ret != MIBSPI_NO_ERR) {
                log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms spi rx err %d", mibspi_ret);
                break;
            }
            // try converting to cmd_struct to check data validity
            comms_err = comms_buffer_to_cmd_struct(data, &comms_cmd_buf);
            if (comms_err != COMMS_SUCCESS) {
                log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms rx data err");
                break;
            }

            // drop all packets not for OBC, should never happen as Comms filters them all out already
            if (comms_cmd_buf.header.dest_hwid != obc_hwid) {
                // TODO: revisit
                break;
            }

            // See if there is a waiter who wants this message
            // Even if there is, do not consume it here
            if (xSemaphoreTake(xCommsWaiterMutexHandle, pdMS_TO_TICKS(COMMS_WAITER_MUTEX_TIMEOUT_MS)) == pdTRUE) {
                if (comms_try_waiter_match(&comms_cmd_buf, &comms_waiter_match_params) == true) {
                    if (comms_waiter_match_params.cmd_ptr != NULL) {
                        memcpy(comms_waiter_match_params.cmd_ptr, &comms_cmd_buf, sizeof(comms_cmd_buf));
                    }
                    xEventGroupSetBits(xCommsWaiterEventGroupHandle, COMMS_RX_EVENT_UNBLOCK_WAITER_BIT);
                }
                xSemaphoreGive(xCommsWaiterMutexHandle);
            }
            else {
                log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms resp mtx t-out");
            }

            // Packet is already given to a waiter if there is one, so at this point any responses are not needed
            if (comms_cmd_buf.header.is_response == 1) {
                break;
            }

            // Pass message through to general handler

            // This causes stall in logging on OBC if Comms sends too many messages. Kept here for use in debugging.
            // log_str(INFO, LOG_COMMS_GENERAL, false, "Comms prcs %x %x %x", comms_cmd_buf.header.seqnum, comms_cmd_buf.header.src_hwid, comms_cmd_buf.header.command);

            // TODO: refactor reply handling into separate file(s)
            memset(reply, 0, sizeof(reply));
            memset(&comms_reply_buf, 0, sizeof(comms_reply_buf));

            comms_reply_buf.header.seqnum = comms_cmd_buf.header.seqnum;
            comms_reply_buf.header.is_response = 1;
            comms_reply_buf.header.dest_hwid = comms_cmd_buf.header.src_hwid;
            comms_reply_buf.header.src_hwid = obc_hwid;

            // Comms control handler
            if (comms_cmd_buf.header.src_hwid == comms_hwid) {
                // special commands that only Comms can send
                if (comms_cmd_buf.header.command == COMMS_BOOTLOADER_MSG_START) {
                    // TODO: Do nothing for now
                    break;
                }
                else if (comms_cmd_buf.header.command == COMMS_RADIO_MSG_START) {
                    // TODO: Do nothing for now
                    break;
                }
                else if (comms_cmd_buf.header.command == COMMS_RADIO_MSG_REBOOTING) {
                    // TODO: Do nothing for now
                    break;
                }
                else {
                    break;  // comms never expects a reply for any messages it sends
                }
            }

            // basic handler for generic commands
            if (comms_cmd_buf.header.command == COMMS_COMMON_MSG_ACK) {
                comms_reply_buf.header.command = COMMS_COMMON_MSG_ACK;
            }
            else {
                // anything not handled from above will receive nack in response
                comms_reply_buf.header.command = COMMS_COMMON_MSG_NACK;
            }

            comms_err = comms_cmd_struct_to_buffer(&comms_reply_buf, reply);
            if (comms_err != COMMS_SUCCESS) {
                log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms gen reply err");
                break;
            }

            // Bug can appear here where if COMMS_INT is high but this loop has not passed this point
            // then this SPI TX can also read out the message from Comms and message gets lost.
            // Only occurs during high traffic.
            // TODO: fix by using tms_mibspi_tx_rx and chaining handling if we receive additional messages
            mibspi_ret = comms_mibspi_tx(reply);
            if (mibspi_ret != MIBSPI_NO_ERR) {
                log_str(ERROR, LOG_COMMS_GENERAL, false, "Comms spi tx err %d", mibspi_ret);
            }

        } while (0);
    }
}


bool comms_try_waiter_match(comms_command_t* comms_cmd, comms_waiter_match_params_t* match_params) {
    bool is_match = false;

    do {
        if (match_params->waiter_present == false) {
            break;
        }

        if ((match_params->match_is_response_spec == COMMS_MATCH_EQUAL) &&
            (match_params->match_is_response != comms_cmd->header.is_response)) {
            break;
        }

        if ((match_params->match_seqnum_spec == COMMS_MATCH_EQUAL) &&
            (match_params->match_seqnum != comms_cmd->header.seqnum)) {
            break;
        }

        if ((match_params->match_src_hwid_spec == COMMS_MATCH_EQUAL) &&
            (match_params->match_src_hwid != comms_cmd->header.src_hwid)) {
            break;
        }

        if ((match_params->match_cmd_num_spec == COMMS_MATCH_EQUAL) &&
            (match_params->match_cmd_num != comms_cmd->header.command)) {
            break;
        }

        is_match = true;

    } while (0);

    return is_match;
}
