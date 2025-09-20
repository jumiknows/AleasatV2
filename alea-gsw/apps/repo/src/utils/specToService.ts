import type { Spec } from "@aleasat/command-spec";

const specToProtoRpc = (spec: Spec): string => {
  const specCommands = Object.keys(spec);

  return specCommands
    .map((commandName) => {
      let commandMessage = `  async def ${commandName.toLowerCase()}(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "${commandName}"${spec[
      commandName
    ].args
      .map((fieldObj) => {
        const fieldName = Object.keys(fieldObj)[0];
        const type = fieldObj[fieldName];
        if (type === "datetime")
          return `, alea_time.ALEADateTime(request.${fieldName}.ToDatetime())`;
        // if (type === "bytes")
        //   return `, bytes(request.${fieldName}"`
        return `, request.${fieldName}`;
      })
      .join(
        "",
      )}, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))`;

      if (spec[commandName].resp === null) {
        commandMessage += `
    return obc_pb2.${commandName.toLowerCase()}_command_OBCResponse()`;
      } else {
        commandMessage += `
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.${commandName.toLowerCase()}_command_OBCResponseData(**{${
      spec[commandName].resp === null
        ? ""
        : spec[commandName]
            .resp!.map((fieldObj) => {
              const fieldName = Object.keys(fieldObj)[0];
              const type = fieldObj[fieldName];
              if (type === "datetime")
                return `"${fieldName}": toProtobufTimestamp(resp.data.fields["${fieldName}"].date_time)`;
              if (type === "bytes")
                return `"${fieldName}": bytes(resp.data.fields["${fieldName}"])`;
              return `"${fieldName}": resp.data.fields["${fieldName}"]`;
            })
            .join(", ")
    }}) if resp.date_time.is_immediate else obc_pb2.${commandName.toLowerCase()}_command_OBCResponseData()
    return obc_pb2.${commandName.toLowerCase()}_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)`;
      }

      return commandMessage;
    })
    .join("\n");
};

export default (cmdSysSpec: Spec, cmdSysTestSpec: Spec | undefined): string => {
  return `import alea.obc_grpc.generated.obc_pb2 as obc_pb2
import alea.obc_grpc.generated.obc_pb2_grpc as obc_pb2_grpc
from alea.obc_grpc.repositories.obc import obc
from alea.common import alea_time
from alea.ttc.cmd_sys import OBCPendingResponse
from datetime import datetime
from concurrent.futures import ThreadPoolExecutor
import asyncio
import functools

_executor = ThreadPoolExecutor(1)

def toProtobufTimestamp(date_time: datetime) -> obc_pb2.google_dot_protobuf_dot_timestamp__pb2.Timestamp:
  timestamp = obc_pb2.google_dot_protobuf_dot_timestamp__pb2.Timestamp()
  timestamp.FromDatetime(date_time)
  return timestamp

class Obc(obc_pb2_grpc.obcServicer):
${specToProtoRpc(cmdSysSpec)}
${cmdSysTestSpec ? specToProtoRpc(cmdSysTestSpec) : ""}
`;
};
