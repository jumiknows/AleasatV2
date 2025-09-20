{% macro telem_impl_sig(telem_spec) -%}
telem_err_t telem_impl_{{ telem_spec.name }}(
    {%- if telem_spec.has_resp -%}
        telem_{{ telem_spec.name }}_resp_t *resp
    {%- endif -%}
)
{%- endmacro %}

{% macro telem_invoke_sig(telem_spec) -%}
static telem_err_t telem_invoke_{{ telem_spec.name }}(const telem_id_t telem_id, const uint8_t priority, const data_fmt_desc_t *resp_desc)
{%- endmacro %}

{% macro field_to_str(field) -%}
{{ field.name }}: {{ field.__class__.__name__|lower }}
{%- endmacro %}
