{% macro cmd_impl_sig(cmd_spec) -%}
{%- if cmd_spec.resp and cmd_spec.resp.has_variable_field -%} cmd_sys_err_t {% else -%} cmd_sys_resp_code_t {% endif -%}
cmd_impl_{{ cmd_spec.name }}(const cmd_sys_cmd_t *cmd
    {%- if cmd_spec.has_args_fields -%}, cmd_{{ cmd_spec.name }}_args_t *args
        {%- if cmd_spec.args.has_variable_field -%}, uint32_t args_len{%- endif -%}
    {%- endif -%}
    {%- if cmd_spec.has_resp_fields -%}, cmd_{{ cmd_spec.name }}_resp_t *resp
        {%- if cmd_spec.resp.has_variable_field -%}, const data_fmt_desc_t *resp_desc, uint32_t resp_len, uint8_t *buf{%- endif -%}
    {%- endif -%}
)
{%- endmacro %}

{% macro cmd_invoke_sig(cmd_spec) -%}
static cmd_sys_err_t cmd_invoke_{{ cmd_spec.name }}(const cmd_sys_cmd_t *cmd, const data_fmt_desc_t *args_desc, const data_fmt_desc_t *resp_desc)
{%- endmacro %}

{% macro field_to_str(field) -%}
{{ field.name }}: {{ field.__class__.__name__|lower }}
{%- endmacro %}
