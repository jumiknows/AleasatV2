/**
 * @file obc_watchdog.h
 * @brief OBC software watchdog implementation
 */

#ifndef OBC_WATCHDOG_H_
#define OBC_WATCHDOG_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC RTOS
#include "obc_tasks_ids_gen.h"

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
    OBC_WATCHDOG_ACTION_IGNORE,     //< The watchdog will pay no attention to this task (it won't even be logged)
    OBC_WATCHDOG_ACTION_ALLOW,      //< The watchdog will log if this task stops responding, but won't do anything about it
    OBC_WATCHDOG_ACTION_SOFT_RESET, //< The watchdog will perform a soft reset of the CPU if the task stops responding
} obc_watchdog_action_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void obc_watchdog_create_infra(void);
void obc_watchdog_start_task(void);

void obc_watchdog_watch_task(obc_task_id_t task_id, obc_watchdog_action_t action);
void obc_watchdog_pet(obc_task_id_t task_id);

#endif // OBC_WATCHDOG_H_
