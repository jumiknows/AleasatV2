/**
 * @file obc_comms.h
 * @brief Low-level OBC to Comms driver using MIBSPI
 */

#ifndef OBC_COMMS_H_
#define OBC_COMMS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "comms_defs.h"

// OBC
#include "tms_mibspi.h"
#include "obc_error.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "rtos_event_groups.h"
#include "rtos_semphr.h"

// HAL
#include "sys_common.h"

// Standard Library
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// default value of 100 milliseconds for most Comms commands, except for flashing and reboot related
#define COMMS_MIBSPI_MUTEX_TIMEOUT_MS 100

/**
 * @brief Comms receiver will wait on this bit, so if this is set, receiver will unblock
 */
#define COMMS_RX_EVENT_UNBLOCK_RECEIVER_BIT  (uint8_t)(1U << 1U)

/**
 * @brief Comms interrupt task will wait on this bit, so if this is set, it will unblock
 */
#define COMMS_RX_EVENT_UNBLOCK_INT_TASK_BIT  (uint8_t)(1U << 2U)

#define COMMS_RESPONSE_MUTEX_TIMEOUT_MS 20

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Struct representing parameters for how to handle a response message from Comms
 */
typedef struct comms_response_params_struct {
    bool waiter_present;
    uint16_t seqnum;
    comms_command_t* cmd_ptr;
} comms_response_params_t;

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

/**
 * @brief RTOS handles for data structures used in this driver.
 */
extern EventGroupHandle_t xMibspiCommsEventGroupHandle;
extern SemaphoreHandle_t xMibspiCommsMutexHandle;

extern EventGroupHandle_t xCommsResponseEventGroupHandle;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/* Startup steps */
void comms_create_infra(void);
void comms_init_irq(void);
void notify_comms_irq(void);
void comms_interrupt_start_task(void);

mibspi_err_t comms_mibspi_tx(uint16_t* tx_buffer);
mibspi_err_t comms_mibspi_rx(uint16_t* rx_buffer);

comms_err_t comms_set_response_waiter(uint16_t seqnum, comms_command_t* cmd_ptr);
comms_err_t comms_clear_response_waiter(void);

#endif /* OBC_COMMS_H_ */
