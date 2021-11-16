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
#include "FreeRTOS.h"
#include "rtos_task.h"

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

static comms_response_params_t comms_response_params = {0};
// Mutex to protect comms_response_params
static SemaphoreHandle_t xCommsResponseMutexHandle;

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

// MIBSPI
EventGroupHandle_t xMibspiCommsEventGroupHandle;
SemaphoreHandle_t xMibspiCommsMutexHandle;

// vCommsInterruptServiceTask and Comms receiver
EventGroupHandle_t xCommsResponseEventGroupHandle;

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

void vCommsInterruptServiceTask(void* pvParameters);

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

/**
 * @brief Initializes the mibSPI peripheral and enables the appropriate transfer group interrupts.
 */
void comms_create_infra(void) {
    static StaticEventGroup_t xMibspiCommsStaticEventGroup;
    static StaticSemaphore_t xMibspiCommsStaticMutex;

    static StaticEventGroup_t xCommsResponseStaticEventGroup;
    static StaticSemaphore_t xCommsResponseStaticMutex;

    // Initialize RTOS event group
    xMibspiCommsEventGroupHandle = xEventGroupCreateStatic(&xMibspiCommsStaticEventGroup);

    // Initialize MIBSPI mutex
    xMibspiCommsMutexHandle = xSemaphoreCreateMutexStatic(&xMibspiCommsStaticMutex);

    xCommsResponseEventGroupHandle = xEventGroupCreateStatic(&xCommsResponseStaticEventGroup);
    xCommsResponseMutexHandle = xSemaphoreCreateMutexStatic(&xCommsResponseStaticMutex);
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

    xEventGroupSetBitsFromISR(xCommsResponseEventGroupHandle, COMMS_RX_EVENT_UNBLOCK_INT_TASK_BIT, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Transmit 256 bytes to Comms.
 *
 * @pre @ref mibspi_init_hw
 * @pre comms_init_irq
 * @pre comms_create_infra
 *
 * @param[in] tx_buffer pointer to the data to transmit, MUST be 256 bytes long
 *
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t comms_mibspi_tx(uint16_t* tx_buffer) {
    return tms_mibspi_tx(&COMMS_256_BYTES_TG, xMibspiCommsEventGroupHandle, tx_buffer, COMMS_MIBSPI_TIMEOUT_MS);
}

/**
 * @brief Receive 256 bytes from Comms.
 *
 * @pre @ref mibspi_init_hw
 * @pre comms_init_irq
 * @pre comms_create_infra
 *
 * @param[out] rx_buffer pointer to the data to receive, MUST be 256 bytes long
 *
 * @return MIBSPI_NO_ERR if no error, error code otherwise
 */
mibspi_err_t comms_mibspi_rx(uint16_t* rx_buffer) {
    return tms_mibspi_rx(&COMMS_256_BYTES_TG, xMibspiCommsEventGroupHandle, rx_buffer, COMMS_MIBSPI_TIMEOUT_MS);
}

/**
 * @brief Sets flag indicating Comms response waiter is active with given parameters
 *
 * @param[in] seqnum sequence number to match to be considered response
 * @param[in] cmd_ptr preallocated comms_command_t struct to fill with response
 * @return COMMS_SUCCESS on success, COMMS_RESPONSE_MUTEX_TIMEOUT if taking mutex timed out
 */
comms_err_t comms_set_response_waiter(uint16_t seqnum, comms_command_t* cmd_ptr) {
    if (xSemaphoreTake(xCommsResponseMutexHandle, pdMS_TO_TICKS(COMMS_RESPONSE_MUTEX_TIMEOUT_MS)) == pdTRUE) {
        comms_response_params.waiter_present = true;
        comms_response_params.seqnum = seqnum;
        comms_response_params.cmd_ptr = cmd_ptr;
        xSemaphoreGive(xCommsResponseMutexHandle);
        return COMMS_SUCCESS;
    }
    else {
        log_str(ERROR, COMMS_LOG, false, "Comms resp mtx t-out 1");
        return COMMS_RESPONSE_MUTEX_TIMEOUT;
    }
}

/**
 * @brief Clears flag indicating Comms response waiter is active and zeros response parameters
 *
 * @return COMMS_SUCCESS on success, COMMS_RESPONSE_MUTEX_TIMEOUT if taking mutex timed out
 */
comms_err_t comms_clear_response_waiter(void) {
    if (xSemaphoreTake(xCommsResponseMutexHandle, pdMS_TO_TICKS(COMMS_RESPONSE_MUTEX_TIMEOUT_MS)) == pdTRUE) {
        comms_response_params.waiter_present = false;
        comms_response_params.seqnum = 0;
        comms_response_params.cmd_ptr = NULL;
        xSemaphoreGive(xCommsResponseMutexHandle);
        return COMMS_SUCCESS;
    }
    else {
        log_str(ERROR, COMMS_LOG, false, "Comms resp mtx t-out 2");
        return COMMS_RESPONSE_MUTEX_TIMEOUT;
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
    static comms_command_t comms_cmd_buf = {0};
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);
    EventBits_t uxBits;
    mibspi_err_t mibspi_ret = MIBSPI_UNKNOWN_ERR;
    comms_err_t comms_err = COMMS_UNKNOWN_ERR;
    bool processed = false;  // control variable only

    while (1) {
        processed = false;
        set_task_status(wd_task_id, task_asleep);
        // Block until notified from ISR
        uxBits = xEventGroupWaitBits(xCommsResponseEventGroupHandle, COMMS_RX_EVENT_UNBLOCK_INT_TASK_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
        set_task_status(wd_task_id, task_alive);

        do {
            if ((uxBits & COMMS_RX_EVENT_UNBLOCK_INT_TASK_BIT) == 0) {
                // timed out, wait again
                break;
            }

            if (xSemaphoreTake(xMibspiCommsMutexHandle, pdMS_TO_TICKS(COMMS_MIBSPI_MUTEX_TIMEOUT_MS)) == pdFALSE) {
                log_str(ERROR, COMMS_LOG, false, "Comms spi mtx t-out");
                break;
            }
            mibspi_ret = comms_mibspi_rx(data);
            xSemaphoreGive(xMibspiCommsMutexHandle);
            if (mibspi_ret != MIBSPI_NO_ERR) {
                log_str(ERROR, COMMS_LOG, false, "Comms spi rx err %d", mibspi_ret);
                break;
            }
            // try converting to cmd_struct to check data validity
            comms_err = comms_buffer_to_cmd_struct(data, &comms_cmd_buf);
            if (comms_err != COMMS_SUCCESS) {
                log_str(ERROR, COMMS_LOG, false, "Comms rx data err");
                break;
            }

            // Try to take mutex to access shared variables
            if (xSemaphoreTake(xCommsResponseMutexHandle, pdMS_TO_TICKS(COMMS_RESPONSE_MUTEX_TIMEOUT_MS)) == pdTRUE) {
                if (comms_response_params.waiter_present == true) {
                    if ((comms_cmd_buf.header.seqnum == comms_response_params.seqnum)) {
                        memcpy(comms_response_params.cmd_ptr, &comms_cmd_buf, sizeof(comms_cmd_buf));
                        xEventGroupSetBits(xCommsResponseEventGroupHandle, COMMS_RX_EVENT_UNBLOCK_RECEIVER_BIT);
                        processed = true;
                    }
                }
                xSemaphoreGive(xCommsResponseMutexHandle);
            }
            else {
                log_str(ERROR, COMMS_LOG, false, "Comms resp mtx t-out");
                // don't break here because we can still pass the received message to the general handler
            }


            // if no receiver wants this msg or we couldn't take the mutex, pass it to general handler
            if (processed == false) {
                log_str(INFO, COMMS_LOG, false, "Comms task process %x", comms_cmd_buf.header.seqnum);
                // TODO: handle cmds addressed to OBC
            }
        } while (0);
    }
}
