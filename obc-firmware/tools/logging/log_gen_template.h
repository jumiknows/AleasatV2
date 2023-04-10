// =============================================================================
//                      AUTO-GENERATED FILE - DO NOT EDIT
//
// This file was auto-generated on {{ timestamp.strftime("%Y-%m-%d at %H:%M:%S") }}
// =============================================================================

/**
 * @file log_gen.h
 * @brief Generated header file from log specifications
*/

// Turn off formatting so that the template doesn't get adjusted by the formatter.
// clang-format off

#ifndef LOG_GEN_H_
#define LOG_GEN_H_

typedef enum {
{% for entry in log_specs %}
    {{ entry }} = {{ log_specs[entry]['id'] }}, /* {{log_specs[entry]['description']}} */
{% endfor %}
} log_id_t;

typedef enum {
{% for entry in log_specs %}
  /* {{ entry }} */
  {% for signal in log_specs[entry]['signals'] -%}
    {{entry}}__{{signal}} = {{ log_specs[entry]['signals'][signal]['id'] }}, /* "{{log_specs[entry]['signals'][signal]['description']}}" */
  {% endfor %}
{%- endfor %}
} log_signal_id_t;

// clang-format on

#endif // LOG_GEN_H_
