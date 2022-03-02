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
 * @brief Comms message waiter will wait on this bit, so if this is set, waiter will unblock
 */
#define COMMS_RX_EVENT_UNBLOCK_WAITER_BIT  (uint8_t)(1U << 1U)

/**
 * @brief Comms interrupt task will wait on this bit, so if this is set, it will unblock
 */
#define COMMS_RX_EVENT_UNBLOCK_INT_TASK_BIT  (uint8_t)(1U << 2U)

#define COMMS_WAITER_MUTEX_TIMEOUT_MS 40

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

/**
 * @brief Enum representing match spec for packet fields
 */
typedef enum {
    COMMS_MATCH_ANY = 0,
    COMMS_MATCH_EQUAL = 1
} comms_match_spec_t;

/**
 * @brief Struct representing parameters to match a received packet for a waiter
 */
typedef struct comms_waiter_match_params_struct {
    bool waiter_present;
    comms_match_spec_t match_is_response_spec;
    uint8_t match_is_response;
    comms_match_spec_t match_seqnum_spec;
    uint16_t match_seqnum;
    comms_match_spec_t match_src_hwid_spec;
    hwid_t match_src_hwid;
    comms_match_spec_t match_cmd_num_spec;
    uint8_t match_cmd_num;
    comms_command_t* cmd_ptr;
} comms_waiter_match_params_t;

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

/**
 * @brief RTOS handles for data structures used in this driver.
 */
extern EventGroupHandle_t xMibspiCommsEventGroupHandle;
extern SemaphoreHandle_t xMibspiCommsMutexHandle;

extern EventGroupHandle_t xCommsWaiterEventGroupHandle;

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
);
comms_err_t comms_set_waiter_match_struct(comms_waiter_match_params_t* match_params);
comms_err_t comms_clear_waiter_match(void);

#endif /* OBC_COMMS_H_ */
