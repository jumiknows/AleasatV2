import type { ArgType, Spec } from "@aleasat/command-spec";

const specTypeToProtoType = (
  type: ArgType,
  name: string,
  index: number,
): string => {
  // u8       = U8
  // s8       = S8
  // u16      = U16
  // s16      = S16
  // u32      = U32
  // s32      = S32
  // u64      = U64
  // s64      = S64
  // f32      = F32
  // f64      = F64
  // bool     = Bool
  // datetime = DateTime
  // bytes    = Bytes
  // string   = String
  const typeToScalar = (): string => {
    switch (type.replace(/\[[0-9]+\]/g, "")) {
      case "u8":
        return "uint32";
      case "s8":
        return "sint32";
      case "u16":
        return "uint32";
      case "s16":
        return "sint32";
      case "u32":
        return "uint32";
      case "s32":
        return "sint32";
      case "u64":
        return "uint64";
      case "s64":
        return "sint64";
      case "f32":
        return "double";
      case "f64":
        return "double";
      case "bool":
        return "bool";
      case "datetime":
        return "google.protobuf.Timestamp";
      case "bytes":
        return "bytes";
      case "string":
        return "string";
      default:
        throw new Error(`Unknown type: ${type}`);
    }
  };
  const arrayDim = (type.match(/\[/g) ?? []).length;

  return `${arrayDim === 1 ? "repeated " : ""}${typeToScalar()}${
    arrayDim === 2 ? "_matrix" : ""
  } ${name} = ${index}${arrayDim === 1 ? " [packed=true]" : ""};`;
};

const specToProtoMessage = (spec: Spec): string => {
  const specCommands = Object.keys(spec);

  return specCommands
    .map((commandName) => {
      let commandMessage = "";

      commandMessage += `message ${commandName.toLowerCase()}_command_args {\n  uint64 timeout = 1;\n  google.protobuf.Timestamp date_time = 2;\n`;
      spec[commandName].resp &&
        spec[commandName].args.forEach((fieldObj, i) => {
          const fieldName = Object.keys(fieldObj)[0];
          commandMessage += `  ${specTypeToProtoType(
            fieldObj[fieldName],
            fieldName,
            i + 3,
          )}\n`;
        });
      commandMessage += "}\n";

      if (spec[commandName].resp !== null) {
        commandMessage += `message ${commandName.toLowerCase()}_command_OBCResponse {\n  uint64 cmd_inst_id = 1;\n  uint32 cmd_id = 2;\n  google.protobuf.Timestamp date_time = 3;\n  uint32 flags = 4;\n  OBCResponseDataHeader data_header = 5;\n  ${commandName.toLowerCase()}_command_OBCResponseData data = 6;\n}\n`;

        commandMessage += `message ${commandName.toLowerCase()}_command_OBCResponseData {\n`;
        spec[commandName].resp?.forEach((fieldObj, i) => {
          const fieldName = Object.keys(fieldObj)[0];
          commandMessage += `  ${specTypeToProtoType(
            fieldObj[fieldName],
            fieldName,
            i + 1,
          )}\n`;
        });
        commandMessage += "}";
      } else {
        commandMessage += `message ${commandName.toLowerCase()}_command_OBCResponse {\n}\n`;
      }

      return commandMessage;
    })
    .join("\n\n");
};

const specToProtoRpc = (spec: Spec): string => {
  const specCommands = Object.keys(spec);

  return specCommands
    .map((commandName) => {
      let commandMessage = "";
      commandMessage += `  rpc ${commandName.toLowerCase()} (${commandName.toLowerCase()}_command_args) returns (${commandName.toLowerCase()}_command_OBCResponse) {}\n`;
      return commandMessage;
    })
    .join("");
};

const generateMatrix = (): string => {
  const types = ["uint32", "uint64", "sint32", "sint64", "double"];

  return types
    .map(
      (type) =>
        `message ${type}_matrix {\n  uint32 rows = 1;\n  uint32 cols = 2;\n  repeated ${type} data = 3 [packed=true];\n}`,
    )
    .join("\n");
};

export default (cmdSysSpec: Spec, cmdSysTestSpec: Spec | undefined): string => {
  return `syntax = "proto3";

import "google/protobuf/timestamp.proto";

// Reminder: NO float only double, due to node floating point precision (IEEE 754 double precision)
// field name cant be https://github.com/protocolbuffers/protobuf-javascript/blob/main/generator/js_generator.cc#L60-L73

service obc {
${specToProtoRpc(cmdSysSpec)}
${cmdSysTestSpec ? specToProtoRpc(cmdSysTestSpec) : ""}
}

${specToProtoMessage(cmdSysSpec)}
${cmdSysTestSpec ? specToProtoMessage(cmdSysTestSpec) : ""}
${generateMatrix()}

message OBCResponseDataHeader {
  Code code = 1;
  google.protobuf.Timestamp exec_datetime = 2;
}

enum Code {
  NONE          = 0;
  SUCCESS       = 1;
  SUCCESS_SCHED = 2;
  ERROR         = 3;
  CMD_DNE       = 4;
  NOT_IMPl      = 5;
}
`;
};
