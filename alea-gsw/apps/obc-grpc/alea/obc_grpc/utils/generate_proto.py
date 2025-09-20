import json
import re
from pathlib import Path


def read_json(file_path):
    with open(file_path, "r", encoding="utf-8") as f:
        return json.load(f)


def spec_type_to_proto_type(type_str, name, index):
    def type_to_scalar(base_type):
        return {
            "u8": "uint32",
            "s8": "sint32",
            "u16": "uint32",
            "s16": "sint32",
            "u32": "uint32",
            "s32": "sint32",
            "u64": "uint64",
            "s64": "sint64",
            "f32": "double",
            "f64": "double",
            "bool": "bool",
            "datetime": "google.protobuf.Timestamp",
            "bytes": "bytes",
            "string": "string",
        }.get(base_type, "unknown")

    array_dim = len(re.findall(r"\[", type_str))
    base_type = re.sub(r"\[[0-9]*\]", "", type_str)
    scalar_type = type_to_scalar(base_type)

    repeated_prefix = "repeated " if array_dim == 1 else ""
    matrix_suffix = "_matrix" if array_dim == 2 else ""
    packed = " [packed=true]" if array_dim == 1 else ""

    return f"{repeated_prefix}{scalar_type}{matrix_suffix} {name} = {index}{packed};"


def spec_to_proto_message(spec):
    output = []
    for command_name, command_data in spec.items():
        cmd_lower = command_name.lower()
        args_lines = [
            f"message {cmd_lower}_command_args {{",
            "  uint64 timeout = 1;",
            "  google.protobuf.Timestamp date_time = 2;",
        ]

        for i, field in enumerate(command_data.get("args", [])):
            field_name = list(field.keys())[0]
            field_type = field[field_name]
            args_lines.append(
                f"  {spec_type_to_proto_type(field_type, field_name, i + 3)}"
            )

        args_lines.append("}")
        output.append("\n".join(args_lines))

        if command_data.get("resp") is not None:
            resp_lines = [
                f"message {cmd_lower}_command_OBCResponse {{",
                "  uint64 cmd_inst_id = 1;",
                "  uint32 cmd_id = 2;",
                "  google.protobuf.Timestamp date_time = 3;",
                "  uint32 flags = 4;",
                "  OBCResponseDataHeader data_header = 5;",
                f"  {cmd_lower}_command_OBCResponseData data = 6;",
                "  bool is_success = 7;",
                "  bool is_success_sched = 8;",
                "}",
            ]
            output.append("\n".join(resp_lines))

            data_lines = [f"message {cmd_lower}_command_OBCResponseData {{"]
            for i, field in enumerate(command_data["resp"]):
                field_name = list(field.keys())[0]
                field_type = field[field_name]
                data_lines.append(
                    f"  {spec_type_to_proto_type(field_type, field_name, i + 1)}"
                )
            data_lines.append("}")
            output.append("\n".join(data_lines))
        else:
            output.append(f"message {cmd_lower}_command_OBCResponse {{\n}}")

    return "\n\n".join(output)


def spec_to_proto_rpc(spec):
    rpcs = []
    for command_name in spec:
        cmd_lower = command_name.lower()
        rpcs.append(
            f"  rpc {cmd_lower} ({cmd_lower}_command_args) returns ({cmd_lower}_command_OBCResponse) {{}}"
        )
        rpcs.append(
            f"  rpc schedule_{cmd_lower} ({cmd_lower}_command_args) returns (stream {cmd_lower}_command_OBCResponse) {{}}"
        )
    return "\n".join(rpcs)


def generate_matrix():
    types = ["uint32", "uint64", "sint32", "sint64", "double"]
    messages = []
    for t in types:
        messages.append(
            f"message {t}_matrix {{\n"
            f"  uint32 rows = 1;\n"
            f"  uint32 cols = 2;\n"
            f"  repeated {t} data = 3 [packed=true];\n"
            f"}}"
        )
    return "\n".join(messages)


def generate_proto_file(cmd_sys_path, cmd_sys_test_path, output_path):
    cmd_sys_spec = read_json(cmd_sys_path)
    cmd_sys_test_spec = read_json(cmd_sys_test_path)

    proto_content = f"""syntax = "proto3";

import "google/protobuf/timestamp.proto";

// Reminder: NO float only double, due to node floating point precision (IEEE 754 double precision)
// field name cant be https://github.com/protocolbuffers/protobuf-javascript/blob/main/generator/js_generator.cc#L60-L73

service obc {{
  rpc log (log_args) returns (stream log_response) {{}}
{spec_to_proto_rpc(cmd_sys_spec)}
{spec_to_proto_rpc(cmd_sys_test_spec)}
}}

message log_args {{}}

message log_response {{
  enum OBCLogLevel {{
    DEBUG   = 0;
    INFO    = 1;
    WARNING = 2;
    ERROR   = 3;
  }}

  google.protobuf.Timestamp date_time = 1;
  uint64 log_id = 2;
  uint64 signal_id = 3;
  uint64 payload_len = 4;

  string group_name = 5;
  string group_desc = 6;
  
  log_response.OBCLogLevel signal_level = 7;
  
  string signal_name = 8;
  string signal_desc = 9;
  
  string data = 10;
}}

{spec_to_proto_message(cmd_sys_spec)}

{spec_to_proto_message(cmd_sys_test_spec)}

{generate_matrix()}

message OBCResponseDataHeader {{
  enum Code {{
    NONE          = 0;
    SUCCESS       = 1;
    SUCCESS_SCHED = 2;
    ERROR         = 3;
    CMD_DNE       = 4;
    NOT_IMPl      = 5;
  }}
    
  Code code = 1;
  google.protobuf.Timestamp exec_datetime = 2;
}}
"""

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(proto_content)
