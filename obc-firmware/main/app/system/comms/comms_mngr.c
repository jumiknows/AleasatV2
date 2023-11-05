/**
 * @file comms_mngr.c
 *
 * @brief Manages the state of the comms interface, handles interrupts from the RF card
 *        and the reception/transmission of radio packets.
 *
 *        Low-level status events from the comms board are handled at this layer. Other commands 
 *        are forwarded to the comms service layer. 
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_defs.h"
#include "comms_utils.h"

// OBC
#include "obc_hardwaredefs.h"
#include "obc_error.h"
#include "obc_task_info.h"
#include "obc_watchdog.h"

// Logging
#include "logger.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

#define COMMS_MNGR_CMD_Q_LEN        5U
#define COMMS_MNGR_CMD_Q_ITEM_SIZE  sizeof(comms_command_t*)

#define COMMS_RX_CMD_EVENT_BIT (1 << 0)
#define COMMS_TX_CMD_EVENT_BIT (1 << 1)

/******************************************************************************/
/*                             T Y P E D E F S                                */
/******************************************************************************/
typedef enum {
    RADIO_STATE_OFFLINE,
    RADIO_STATE_INIT,
    RADIO_STATE_READY,
    RADIO_STATE_BUSY,
} radio_state_t;

/******************************************************************************/
/*                   F U N C T I O N  P R O T O T Y P E S                     */
/******************************************************************************/

static void vCommsMngrTask(void* pvParameters);
static void handle_tx_event(void);
static void handle_rx_event(void);
static void handle_dev_interrupt(bool is_isr, void* param);
static EventBits_t get_unblock_conditions(void);

/******************************************************************************/
/*                      G L O B A L  V A R I A B L E S                        */
/******************************************************************************/

static QueueHandle_t cmd_q;
static EventGroupHandle_t evt_group;
static comms_dev_handle_t cdev;
static volatile radio_state_t rfc_state = RADIO_STATE_READY;

/******************************************************************************/
/*                       P U B L I C  F U N C T I O N S                       */
/******************************************************************************/

void comms_mngr_create_infra(void) {
    static StaticQueue_t cmd_q_buf;
    static uint8_t cmd_q_storage[COMMS_MNGR_CMD_Q_LEN * COMMS_MNGR_CMD_Q_ITEM_SIZE];
    static StaticEventGroup_t evt_group_buf;

    cmd_q = xQueueCreateStatic(COMMS_MNGR_CMD_Q_LEN,
                               COMMS_MNGR_CMD_Q_ITEM_SIZE,
                               cmd_q_storage,
                               &cmd_q_buf);

    evt_group = xEventGroupCreateStatic(&evt_group_buf);
}

/**
 * @brief Start the comms manager task
 *
 * @param[in] cdev handle for the low-level comms device driver
 */
void comms_mngr_start_task(comms_dev_handle_t cdev_hdl) {
    static StackType_t xCommsMngrTaskStack[COMMS_IRQ_TASK_STACK_SIZE];
    static StaticTask_t xCommsMngrTaskBuffer;

    cdev = cdev_hdl;
    task_create_static(
        &vCommsMngrTask,
        "comms_mngr",
        COMMS_IRQ_TASK_STACK_SIZE,
        NULL,
        COMMS_IRQ_SERVICE_PRIORITY,
        xCommsMngrTaskStack,
        &xCommsMngrTaskBuffer
    );
}

/**
 * @brief Send a command over the comms interface. The packet will be queued
 *        and transmitted asynchronously by the comms manager task. 
 *
 * @pre   comms_mngr_create_infra
 *
 * @param[in] cmd pointer to the command packet to be transmitted.
 *
 * @return COMMS_SUCCESS if no error, COMMS_ERR_TIMEOUT if the command queue is full
 */
comms_err_t comms_mngr_send_cmd(
    comms_command_t* cmd
) {
    comms_err_t err = COMMS_SUCCESS;
        
    if(xQueueSend(cmd_q, (void*)&cmd, 0) != pdTRUE) {
        err = COMMS_ERR_BUSY;
    }
    else {
        xEventGroupSetBits(evt_group, COMMS_TX_CMD_EVENT_BIT);
    }

    return err;
}

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

/**
 * @brief Send a packet over the comms interface
 */
static void handle_tx_event(void) {
    static uint16_t buf[(COMMS_DEV_MIN_BUFFER_SIZE / 2)] = {0};
    static uint8_t *buf_u8 = (uint8_t *)&buf[0];

    comms_command_t* cmd;
    comms_err_t err = COMMS_SUCCESS;

    uint16_t msg_len = 0;

    do {
        if(xQueueReceive(cmd_q,
            (void*)&cmd, 0) != pdPASS) {
            LOG_COMMS__CMD_QUEUE_EMPTY();
            err = COMMS_ERR_TIMEOUT;
            break;
        }

        if(comms_cmd_struct_to_buffer(cmd, buf_u8, &msg_len) != COMMS_SUCCESS) {
            LOG_COMMS__FAILED_CONVERTING_PACKET_TO_BUFFER(err);
            err = COMMS_ERR_INVALID_ARG;
            break;
        }

        if(comms_dev_send(cdev, buf, msg_len) != COMMS_DEV_SUCCESS) {
            LOG_COMMS__FAILED_TO_SEND_CMD(err);
            err = COMMS_ERR_RFC_TXN_FAIL;
            break;
        }

        if(cmd->header.is_response == 0) {
            rfc_state = RADIO_STATE_BUSY;
        }
    } while (0);

    if(err != COMMS_SUCCESS) {
        // notify the upper layers that the packet failed
        comms_service_packet_failure(cmd);
    }
}

/**
 * @brief Receive a packet from the comms interface
 */
static void handle_rx_event(void) {
    static uint16_t buf[(COMMS_DEV_MIN_BUFFER_SIZE / 2)] = {0};
    static uint8_t *buf_u8 = (uint8_t *)&buf[0];

    comms_command_t cmd;
    comms_err_t err = COMMS_SUCCESS;

    do {
        if (comms_dev_receive(cdev, buf, sizeof(buf)) != COMMS_DEV_SUCCESS) {
            LOG_COMMS__RX_ERROR(err);
            err = COMMS_ERR_RFC_TXN_FAIL;
            break;
        }

        err = comms_buffer_to_cmd_struct(buf_u8, &cmd);
        if (err != COMMS_SUCCESS) {
            LOG_COMMS__RX_DATA_ERROR(err);
            break;
        }

        if(cmd.header.is_response) {
            rfc_state = RADIO_STATE_READY;
        }

        switch(cmd.header.command) {
            // no response is required for these commands
            case COMMS_RADIO_MSG_START:
                rfc_state = RADIO_STATE_READY;
                break;
            case COMMS_BOOTLOADER_MSG_START:
                rfc_state = RADIO_STATE_INIT;
                break;
            default:
                break;
        }
    } while(0);

    if(err == COMMS_SUCCESS) {
        comms_service_packet_input(&cmd);
    }
}

/**
 * @brief Check the current radio state and get the unblock conditions
 *        for the comms manager task.
 *
 * @return RTOS event bitmask with the current unblock conditions
 */
static EventBits_t get_unblock_conditions(void) {

    EventBits_t uxWaitBits = (EventBits_t)COMMS_RX_CMD_EVENT_BIT;

    switch(rfc_state) {
        // unblock on command requests only if the radio is ready or in bootloader state
        case RADIO_STATE_READY:
        case RADIO_STATE_INIT:
            uxWaitBits |= COMMS_TX_CMD_EVENT_BIT;
            break;

        // no additional unblock conditions for other states
        case RADIO_STATE_OFFLINE:
        case RADIO_STATE_BUSY:
        default:
            break;
    }
    return uxWaitBits;
}

/**
 * @brief handle interrupt generated by the comms board. Notify
 *        the comms manager task that a packet is ready to be received.
 */
static void handle_dev_interrupt(bool is_isr, void* param) {
    if (is_isr) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        xEventGroupSetBitsFromISR(evt_group,
                COMMS_RX_CMD_EVENT_BIT, &xHigherPriorityTaskWoken);

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xEventGroupSetBits(evt_group, COMMS_RX_CMD_EVENT_BIT);
    }
}

/**
 * @brief Comms manager handler task.
 *
 * @param[in] pvParameters low-level comms device driver handle.
 */
static void vCommsMngrTask(void* pvParameters) {
    task_id_t wd_task_id = WD_TASK_ID(pvParameters);
    EventBits_t uxSetBits, uxWaitBits;

    comms_dev_register_callback(cdev, &handle_dev_interrupt, NULL);

    while (1) {

        // Unblock on an interrupt event from the RF card
        uxWaitBits = get_unblock_conditions();

        set_task_status(wd_task_id, task_asleep);
        // Block until notified from ISR
        // TODO ensure the event group bits are set and cleared correctly
        uxSetBits = xEventGroupWaitBits(evt_group,
                        uxWaitBits, pdTRUE, pdFALSE, portMAX_DELAY);
        set_task_status(wd_task_id, task_alive);

        if(uxWaitBits & uxSetBits & COMMS_RX_CMD_EVENT_BIT) {
            handle_rx_event();
        }

        if(uxWaitBits & uxSetBits & COMMS_TX_CMD_EVENT_BIT) {
            while(uxQueueMessagesWaiting(cmd_q) >0) {
                handle_tx_event();
            }
        }
    }
}
