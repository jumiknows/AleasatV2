{% macro task_hook_sig(task_spec) -%}
static BaseType_t task_hook_{{ "%-25s"|format(task_spec.name) }}(void * pvParameter)
{%- endmacro %}
