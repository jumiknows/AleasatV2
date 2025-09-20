import fs from 'fs'

const cmdSysSpec = JSON.parse(fs.readFileSync("../../../alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys.json", "utf8"));
const cmdSysTestSpec = JSON.parse(fs.readFileSync("../../../alea-fsw/obc-fw/python/alea-obcfw/alea/obcfw/cmd_sys/data/cmd_sys_test.json", "utf8"));


const specToProtoRpc = (spec) => {
  const specCommands = Object.keys(spec);

  return specCommands.map((commandName) => {
    let commandMessage = `  async def ${commandName.toLowerCase()}(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "${commandName}"${spec[commandName].args.map((fieldObj, i) => {
      const fieldName = Object.keys(fieldObj)[0];
      const type = fieldObj[fieldName]
      if (type === "datetime")
        return `, alea_time.ALEADateTime(request.${fieldName}.ToDatetime())`
      if (type.includes("]["))
        return `, request.${fieldName}.data`
      // if (type === "bytes")
      //   return `, bytes(request.${fieldName}"`
      return `, request.${fieldName}`
    }).join("")}, date_time=alea_time.IMMEDIATE, timeout=request.timeout))`

    if (spec[commandName].resp === null) {
      commandMessage += `
    return obc_pb2.${commandName.toLowerCase()}_command_OBCResponse()`
    }
    else {
      commandMessage += `
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.${commandName.toLowerCase()}_command_OBCResponseData(**{${spec[commandName].resp === null ? "" : spec[commandName].resp.map((fieldObj, i) => {
        const fieldName = Object.keys(fieldObj)[0];
        const type = fieldObj[fieldName]
        if (type === "datetime")
          return `"${fieldName}": toProtobufTimestamp(resp.data.fields["${fieldName}"].date_time)`
        if (type === "bytes")
          return `"${fieldName}": bytes(resp.data.fields["${fieldName}"])`
        if (type.includes("]["))
          return `"${fieldName}": toMatrix(resp.data.fields["${fieldName}"], "${type.split("[")[0]}", ${type.match(/\[([0-9]+)\]/)[1]}, ${type.match(/\[[0-9]+\]\[([0-9]+)\]/)[1]})`
        return `"${fieldName}": resp.data.fields["${fieldName}"]`
      }).join(", ")}}) if resp.date_time.is_immediate else obc_pb2.${commandName.toLowerCase()}_command_OBCResponseData()
    return obc_pb2.${commandName.toLowerCase()}_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)`
    }

    commandMessage += `\n  async def schedule_${commandName.toLowerCase()}(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "${commandName}"${spec[commandName].args.map((fieldObj, i) => {
      const fieldName = Object.keys(fieldObj)[0];
      const type = fieldObj[fieldName]
      if (type === "datetime")
        return `, alea_time.ALEADateTime(request.${fieldName}.ToDatetime())`
      if (type.includes("]["))
        return `, request.${fieldName}.data`
      // if (type === "bytes")
      //   return `, bytes(request.${fieldName}"`
      return `, request.${fieldName}`
    }).join("")}, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))`

    if (spec[commandName].resp === null) {
      commandMessage += `
    yield obc_pb2.${commandName.toLowerCase()}_command_OBCResponse()`
    }
    else {
      commandMessage += `
    if (isinstance(resp, OBCPendingResponse)): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.${commandName.toLowerCase()}_command_OBCResponseData()
      yield obc_pb2.${commandName.toLowerCase()}_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.${commandName.toLowerCase()}_command_OBCResponseData(**{${spec[commandName].resp === null ? "" : spec[commandName].resp.map((fieldObj, i) => {
        const fieldName = Object.keys(fieldObj)[0];
        const type = fieldObj[fieldName]
        if (type === "datetime")
          return `"${fieldName}": toProtobufTimestamp(resp.data.fields["${fieldName}"].date_time)`
        if (type === "bytes")
          return `"${fieldName}": bytes(resp.data.fields["${fieldName}"])`
        if (type.includes("]["))
          return `"${fieldName}": toMatrix(resp.data.fields["${fieldName}"], "${type.split("[")[0]}", ${type.match(/\[([0-9]+)\]/)[1]}, ${type.match(/\[[0-9]+\]\[([0-9]+)\]/)[1]})`
        return `"${fieldName}": resp.data.fields["${fieldName}"]`
      }).join(", ")}}) if resp.date_time.is_immediate else obc_pb2.${commandName.toLowerCase()}_command_OBCResponseData()
    yield obc_pb2.${commandName.toLowerCase()}_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)`
    }

    return commandMessage
  }).join("\n")
}

let serviceFile = `import alea.obc_grpc.generated.obc_pb2 as obc_pb2
import alea.obc_grpc.generated.obc_pb2_grpc as obc_pb2_grpc
from alea.obc_grpc.repositories.obc import obc, log_recievers
from alea.common import alea_time
from alea.ttc.cmd_sys import OBCPendingResponse
from datetime import datetime
from concurrent.futures import ThreadPoolExecutor
import asyncio
import functools
import json
import numpy as np
import queue

_executor = ThreadPoolExecutor(1)

def toProtobufTimestamp(date_time: datetime) -> obc_pb2.google_dot_protobuf_dot_timestamp__pb2.Timestamp:
  timestamp = obc_pb2.google_dot_protobuf_dot_timestamp__pb2.Timestamp()
  timestamp.FromDatetime(date_time)
  return timestamp

def toMatrix(arg, type, rows, cols):
  if type == "f32" or type == "f64":
    matrixType = "double"
  elif type == "s64":
    matrixType = "sint64"
  elif type == "u64":
    matrixType = "uint64"
  elif type.startswith("u"):
    matrixType = "uint32"
  else:
    matrixType = "sint32"
  matrix = getattr(obc_pb2, f"{matrixType}_matrix")(
    rows=rows,
    cols=cols,
    data=np.array(arg).flatten(),
  )
  return matrix

def isJsonable(x):
  try:
    json.dumps(x)
    return True
  except:
    return False

class Obc(obc_pb2_grpc.obcServicer):
  async def log(self, request, context):
    log_queue = asyncio.Queue()
    log_recievers.append(log_queue)
    try:
      while True:
        log = await log_queue.get()
        yield obc_pb2.log_response(
          date_time=toProtobufTimestamp(log.date_time.date_time),
          log_id=log.log_id,
          signal_id=log.signal_id,
          payload_len=log.payload_len,
          group_name=log.group_name,
          group_desc=log.group_desc,
          signal_level=log.signal_level,
          signal_name=log.signal_name,
          signal_desc=log.signal_desc,
          data=json.dumps({k: v if isJsonable(v) else str(v) for k, v in log.data.items()}) if log.data else None,
        )
    except GeneratorExit:
      pass
    finally:
      log_recievers.remove(log_queue)
${specToProtoRpc(cmdSysSpec)}
${specToProtoRpc(cmdSysTestSpec)}
`

fs.writeFileSync("alea/obc_grpc/services/generated/obc_service.py", serviceFile)
