/**
 * @file obc_rtos.h
 * @brief RTOS management
 */

#ifndef OBC_RTOS_H_
#define OBC_RTOS_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

// OBC
#include "obc_tasks_ids_gen.h"
#include "obc_watchdog.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef BaseType_t (*obc_task_hook_t)(void *pvParameter);

typedef struct {
    const char *name;
    StaticTask_t *task_buffer;
    StackType_t *stack;
    obc_task_hook_t hook_func;
    uint16_t stack_size;
    uint16_t priority;
} obc_task_spec_t;

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

void obc_rtos_init(void);

TaskHandle_t obc_rtos_create_task(obc_task_id_t id, TaskFunction_t task_func, void *const params, obc_watchdog_action_t watchdog_action);
TaskHandle_t obc_rtos_create_task_privileged(obc_task_id_t id, TaskFunction_t task_func, void *const params, obc_watchdog_action_t watchdog_action);

BaseType_t obc_rtos_task_hook(obc_task_id_t id, void *arg);

TaskHandle_t obc_rtos_get_task_handle(obc_task_id_t id);

void obc_rtos_get_idle_task_memory(StaticTask_t **task_buffer, StackType_t **stack, uint32_t *stack_size);

uint32_t obc_rtos_trace_freeze(uint32_t *trace_dest, uint32_t len);

#endif // OBC_RTOS_H_
