// =============================================================================
//                      AUTO-GENERATED FILE - DO NOT EDIT
//
// This file was auto-generated on {{ timestamp.strftime("%Y-%m-%d at %H:%M:%S") }}
// =============================================================================

/**
 * @file obc_task_ids_gen.h
 * @brief Generated code for OBC task IDs
 */

#ifndef OBC_TASK_IDS_GEN_H_
#define OBC_TASK_IDS_GEN_H_

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
{% for task_spec in task_specs %}
    OBC_TASK_ID_{{ "%-25s"|format(task_spec.name) }} = {{ task_spec.id }},
{%- endfor %}

    OBC_TASK_COUNT = {{ task_specs.max_id + 1 }},
} obc_task_id_t;

#endif // OBC_TASK_IDS_GEN_H_
