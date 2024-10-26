{% from 'macros.c' import task_hook_sig -%}
{% set task_name_fmt = '%-24s' -%}
// =============================================================================
//                      AUTO-GENERATED FILE - DO NOT EDIT
//
// This file was auto-generated on {{ timestamp.strftime("%Y-%m-%d at %H:%M:%S") }}
// =============================================================================

/**
 * @file obc_tasks_gen.c
 * @brief Generated code from OBC task specifications
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "{{ obc_tasks_gen_header }}"

// OBC RTOS
#include "obc_rtos.h"

// FreeRTOS
#include "rtos.h"

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/
{% for task_spec in task_specs %}
{{ task_hook_sig(task_spec) }};
{%- endfor %}

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

// Task Stacks
{% for task_spec in task_specs %}
static StackType_t task_stack_{{ task_name_fmt|format(task_spec.name) }}[{{ "%4d"|format(task_spec.stack_size) }}];
{%- endfor %}

// Task Buffers
{% for task_spec in task_specs %}
static StaticTask_t task_buf_{{ task_name_fmt|format(task_spec.name) }} = { 0 };
{%- endfor %}

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

const obc_task_spec_t OBC_TASK_SPEC_TABLE[OBC_TASK_COUNT] = {
{% for task_spec in task_specs %}
    [OBC_TASK_ID_{{ task_name_fmt|format(task_spec.name) }}] = { .name = {{ task_name_fmt|format("\"" ~ task_spec.name ~ "\"") }}, .task_buffer = &task_buf_{{ task_name_fmt|format(task_spec.name) }}, .stack = task_stack_{{ task_name_fmt|format(task_spec.name) }}, .hook_func = &task_hook_{{ task_name_fmt|format(task_spec.name) }}, .stack_size = {{ "%4d"|format(task_spec.stack_size) }}, .priority = {{ "%2d"|format(task_spec.priority) }} },
{%- endfor %}

};

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/
{% for task_spec in task_specs %}
{{ task_hook_sig(task_spec) }} { return obc_rtos_task_hook(OBC_TASK_ID_{{ task_name_fmt|format(task_spec.name) }}, pvParameter); }
{%- endfor %}
