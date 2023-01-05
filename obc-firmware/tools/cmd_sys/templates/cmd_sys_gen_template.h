{% from 'macros.c' import cmd_impl_sig, field_to_str -%}
// =============================================================================
//                      AUTO-GENERATED FILE - DO NOT EDIT
//
// This file was auto-generated on {{ timestamp.strftime("%Y-%m-%d at %H:%M:%S") }}
// =============================================================================


/**
 * @file cmd_sys_gen.h
 * @brief Generated code from command system specifications
 */

#ifndef CMD_SYS_GEN_H_
#define CMD_SYS_GEN_H_

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "cmd_sys.h"

// Standard Library
#include <stdint.h>

/******************************************************************************/
/*                               D E F I N E S                                */
/******************************************************************************/

// Public defines that may be used by other files

/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

typedef enum {
{% for cmd_spec in cmd_specs %}
    CMD_ID_{{ "%-25s"|format(cmd_spec.name) }} = {{ cmd_spec.id }},
{%- endfor %}

    CMD_COUNT = {{ cmd_specs.max_id + 1 }},

} cmd_id_t;

{% for cmd_spec in cmd_specs %}
// {{ (cmd_spec.name + " ").ljust(76, "-") }}
{% if cmd_spec.has_args_fields %}
typedef struct {
    {%- for arg in cmd_spec.args %}
    {% if arg.size > 0 -%}
    {{ arg.c_type.type_name }} {{ arg.name }}{% if arg.is_array %}[{{ arg.array_len }}]{% endif %};
    {%- else -%}
    // {{ field_to_str(arg) }}
    {%- endif %}
    {%- endfor %}
} cmd_{{ cmd_spec.name }}_args_t;
{% endif -%}
{% if cmd_spec.has_resp_fields %}
typedef struct {
    {%- for resp_field in cmd_spec.resp %}
    {% if resp_field.size > 0 -%}
    {{ resp_field.c_type.type_name }} {{ resp_field.name }}{% if resp_field.is_array %}[{{ resp_field.array_len }}]{% endif %};
    {%- else -%}
    // {{ field_to_str(resp_field) }}
    {%- endif %}
    {%- endfor %}
} cmd_{{ cmd_spec.name }}_resp_t;
{% endif -%}
{% endfor %}

/******************************************************************************/
/*                       G L O B A L  V A R I A B L E S                       */
/******************************************************************************/

extern const cmd_sys_cmd_spec_t CMD_SPEC_TABLE[CMD_COUNT];

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

// Command implementations
{% for cmd_spec in cmd_specs %}
{{ cmd_impl_sig(cmd_spec) }};
{%- endfor %}

#endif // CMD_SYS_GEN_H_
