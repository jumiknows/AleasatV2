{% from 'macros.c' import cmd_impl_sig, cmd_invoke_sig, field_to_str -%}
{% set cmd_name_fmt = '%-25s' -%}
// =============================================================================
//                      AUTO-GENERATED FILE - DO NOT EDIT
//
// This file was auto-generated on {{ timestamp.strftime("%Y-%m-%d at %H:%M:%S") }}
// =============================================================================

/**
 * @file cmd_sys_gen.c
 * @brief Generated code from command system specifications
 */

/******************************************************************************/
/*                              I N C L U D E S                               */
/******************************************************************************/

#include "{{ cmd_sys_gen_header }}"
#include "cmd_sys.h"

// Utils
#include "obc_utils.h"
#include "data_fmt.h"

// FreeRTOS
#include "rtos.h"

// Standard Library
#include <stddef.h>

/******************************************************************************/
/*               P R I V A T E  G L O B A L  V A R I A B L E S                */
/******************************************************************************/

// Argument Field Descriptors
{% for cmd_spec in cmd_specs -%}
{% if cmd_spec.has_args_fields %}
static const data_fmt_field_desc_t arg_field_descs_{{ cmd_spec.name }}[{{ cmd_spec.args.fixed_field_count }}] = {
{%- for arg in cmd_spec.args %}
    {% if arg.size > 0 -%}
    { .type = {{ arg.c_type.type_enum }}, .struct_offset = offsetof(cmd_{{ cmd_spec.name }}_args_t, {{ arg.name }}), .array_len = {{ arg.array_len }} },
    {%- else -%}
    // {{ field_to_str(arg) }}
    {%- endif %}
{%- endfor %}
};
{%- endif -%}
{% endfor %}

// Arguments Descriptors
{% for cmd_spec in cmd_specs -%}
{% if cmd_spec.has_args_fields %}
static const data_fmt_desc_t args_desc_{{ cmd_name_fmt|format(cmd_spec.name) }} = { .fields = arg_field_descs_{{ cmd_name_fmt|format(cmd_spec.name) }}, .count = {{ cmd_spec.args.fixed_field_count }} };
{%- endif -%}
{% endfor %}

// Response Field Descriptors
{% for cmd_spec in cmd_specs -%}
{% if cmd_spec.has_resp_fields %}
static const data_fmt_field_desc_t resp_field_descs_{{ cmd_spec.name }}[{{ cmd_spec.resp.fixed_field_count }}] = {
{%- for resp_field in cmd_spec.resp %}
    {% if resp_field.size > 0 -%}
    { .type = {{ resp_field.c_type.type_enum }}, .struct_offset = offsetof(cmd_{{ cmd_spec.name }}_resp_t, {{ resp_field.name }}), .array_len = {{ resp_field.array_len }} },
    {%- else -%}
    // {{ field_to_str(resp_field) }}
    {%- endif %}
{%- endfor %}
};
{%- endif -%}
{% endfor %}

// Response Descriptors
{% for cmd_spec in cmd_specs -%}
{% if cmd_spec.has_resp_fields %}
static const data_fmt_desc_t resp_desc_{{ cmd_name_fmt|format(cmd_spec.name) }} = { .fields = resp_field_descs_{{ cmd_name_fmt|format(cmd_spec.name) }}, .count = {{ cmd_spec.resp.fixed_field_count }} };
{%- endif -%}
{% endfor %}

/******************************************************************************/
/*            P R I V A T E  F U N C T I O N  P R O T O T Y P E S             */
/******************************************************************************/

{% for cmd_spec in cmd_specs -%}
{{ cmd_invoke_sig(cmd_spec) }};
{% endfor %}

/******************************************************************************/
/*                P U B L I C  G L O B A L  V A R I A B L E S                 */
/******************************************************************************/

const cmd_sys_cmd_spec_t CMD_SPEC_TABLE[CMD_COUNT] = {
{% for cmd_spec in cmd_specs %}
    [CMD_ID_{{ cmd_name_fmt|format(cmd_spec.name) }}] = { .invoke = &cmd_invoke_{{ cmd_name_fmt|format(cmd_spec.name) }}, .args = 
    {%- if cmd_spec.has_args_fields %} &args_desc_{{ cmd_name_fmt|format(cmd_spec.name) }} {%- else %} {{ "%-36s"|format("NULL") }} {%- endif -%}
    , .resp =
    {%- if cmd_spec.has_resp_fields %} &resp_desc_{{ cmd_name_fmt|format(cmd_spec.name) }} {%- else %} {{ "%-36s"|format("NULL") }} {%- endif -%}
    },
{%- endfor %}

};

/******************************************************************************/
/*                      P R I V A T E  F U N C T I O N S                      */
/******************************************************************************/

{%- for cmd_spec in cmd_specs %}
    {%- set args_size = cmd_spec.args.size -%}
    {%- set resp_size = cmd_spec.resp.size %}

{{ cmd_invoke_sig(cmd_spec) }} {
    cmd_sys_err_t err = CMD_SYS_SUCCESS;

{%- if ((cmd_spec.has_args_fields) or (cmd_spec.has_resp_fields)) %}

    // Re-use one buffer for args / resp
    uint8_t buf[{{ [args_size, resp_size] | max }}] = { 0 };
{%- endif -%}

{%- if cmd_spec.has_args_fields %}

    // Handle Arguments
    cmd_{{ cmd_spec.name }}_args_t args = { 0 };
    err = cmd_sys_handle_args(cmd, &args, args_desc, {{ args_size }}, buf);
    if (err != CMD_SYS_SUCCESS) {
        return err;
    }
{%- endif -%}

{%- if cmd_spec.has_resp_fields %}

    // Declare Response
    cmd_{{ cmd_spec.name }}_resp_t resp = { 0 };
{%- endif %}

    // Run Command
    {% if cmd_spec.resp %}{% if cmd_spec.resp.has_variable_field -%} err{% else -%} cmd_sys_resp_code_t resp_code{% endif %} = {% endif -%}
    cmd_impl_{{ cmd_spec.name }}(cmd
        {%- if cmd_spec.has_args_fields -%}, &args
            {%- if cmd_spec.args.has_variable_field -%}, {{ args_size }}{%- endif -%}
        {%- endif -%}
        {%- if cmd_spec.has_resp_fields -%}, &resp
            {%- if cmd_spec.resp.has_variable_field -%}, resp_desc, {{ resp_size }}, buf{%- endif -%}
        {%- endif -%}
    );

{%- if cmd_spec.resp and not cmd_spec.resp.has_variable_field %}

    // Handle Response
    err = cmd_sys_handle_resp(cmd
        {%- if cmd_spec.has_resp_fields -%}
            , &resp, resp_desc, {{ resp_size }}, buf
        {%- else -%}
            , NULL, NULL, 0, NULL
        {%- endif -%}
        , resp_code);
{%- endif %}

    return err;
}
{%- endfor %}
