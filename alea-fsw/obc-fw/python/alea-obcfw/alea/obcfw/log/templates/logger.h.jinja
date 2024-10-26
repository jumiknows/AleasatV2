// =============================================================================
//                      AUTO-GENERATED FILE - DO NOT EDIT
//
// This file was auto-generated on {{ timestamp.strftime("%Y-%m-%d at %H:%M:%S") }}
// =============================================================================

/**
 * @file log_gen.h
 * @brief Generated header file from log specifications
*/

#ifndef LOG_GEN_H_
#define LOG_GEN_H_

// OBC
#include "log_sys.h"

// Utils
#include "data_fmt.h"
#include "obc_utils.h"


/******************************************************************************/
/*                              T Y P E D E F S                               */
/******************************************************************************/

{% for log_spec in log_specs %}
// {{ (log_spec.name + " ").ljust(76, "-") }}
{% for signal_spec in log_spec.signals %}
{%- if signal_spec.has_data_fields %}
typedef struct {
    {%- for field in signal_spec.data %}
    {% if field.size > 0 -%}
    {{ field.c_type.type_name }} {{ field.name }}{% if field.is_array %}{% for dim in field.array_shape %}[{{ dim }}]{% endfor %}{% endif %};
    {%- else -%}
    // {{ field_to_str(field) }}
    {%- endif %}
    {%- endfor %}
} log_{{ log_spec.name }}__{{ signal_spec.name }}_data_t;
{% endif -%}
{% endfor -%}
{% endfor %}

/******************************************************************************/
/*                             F U N C T I O N S                              */
/******************************************************************************/

/* Debugging printf function, defined in log_sys.c */
void LOG_PRINTF(char* format, ...);

/* Sepcial logging function for cmd system scheduled responses */
void LOG_CMD_SYS_SCHED_RESP(uint32_t num_bytes, const uint8_t* data);

{% for log_spec in log_specs %}
/* {{ log_spec.name }} */
{% for signal_spec in log_spec.signals -%}
inline void {{log_spec.name}}__{{signal_spec.name}}(
    {%- if signal_spec.has_data_fields -%}
    {%- for field in signal_spec.data -%}
    {% if field.size > 0 -%}
    const {{ field.c_type.type_name }} {{ field.name }}{% if field.is_array %}{% for dim in field.array_shape %}[{{ dim }}]{% endfor %}{% endif %}{{ ", " if not loop.last else "" }}
    {%- else -%}
    // {{ field_to_str(field) }}
    {%- endif -%}
    {%- endfor -%}
    {%- endif -%}
  ) {
  {% if signal_spec.has_data_fields -%}
  log_{{ log_spec.name }}__{{ signal_spec.name }}_data_t log_data;

  /* Populate Struct Fields */
  {%- for field in signal_spec.data %}
  {% if field.size > 0 -%}
  {%- if field.is_array -%}
  memcpy(log_data.{{field.name}}, {{field.name}}, {% for dim in field.array_shape %}{{ dim }}{{ "*" if not loop.last else "" }}{% endfor %});
  {%- else -%}
  log_data.{{field.name}} = {{field.name}};
  {%- endif %}
  {%- endif %}
  {%- endfor %}

  // Data Field Descriptors
  {%- if signal_spec.has_data_fields %}
  static const data_fmt_field_desc_t data_field_descs[{{ signal_spec.data.fixed_field_count }}] = {
    {%- for field in signal_spec.data %}
    {% if field.size > 0 -%}
    { .type = {{ field.c_type.type_enum }}, .struct_offset = offsetof(log_{{ log_spec.name }}__{{ signal_spec.name }}_data_t, {{ field.name }}), .array_len = {{ field.array_len }} },
    {%- else -%}
    // {{ field_to_str(field) }}
    {%- endif %}
    {%- endfor %}
  };
  {%- endif %}

  // Data Descriptor
  static const data_fmt_desc_t data_desc = { .fields = data_field_descs, .count = {{ signal_spec.data.fixed_field_count }} };

  log_sys_log_signal_with_data({{log_spec.id}}, {{signal_spec.id}}, {{signal_spec.data.size}}, (const void *)&log_data, &data_desc);
  {%- else -%}
  log_sys_log_signal({{log_spec.id}}, {{signal_spec.id}});
  {%- endif %}
}

{% endfor %}
{%- endfor %}




#endif // LOG_GEN_H_
