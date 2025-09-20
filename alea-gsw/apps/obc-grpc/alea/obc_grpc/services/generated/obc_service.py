import alea.obc_grpc.generated.obc_pb2 as obc_pb2
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
  if type in ("f32", "f64"):
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
  async def reset(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "RESET", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    return obc_pb2.reset_command_OBCResponse()
  async def schedule_reset(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "RESET", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    yield obc_pb2.reset_command_OBCResponse()
  async def ping(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "PING", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.ping_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.ping_command_OBCResponseData()
    return obc_pb2.ping_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_ping(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "PING", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.ping_command_OBCResponseData()
      yield obc_pb2.ping_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.ping_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.ping_command_OBCResponseData()
    yield obc_pb2.ping_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def get_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_TIME", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_time_command_OBCResponseData(**{"timestamp": toProtobufTimestamp(resp.data.fields["timestamp"].date_time)}) if resp.date_time.is_immediate else obc_pb2.get_time_command_OBCResponseData()
    return obc_pb2.get_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_get_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_TIME", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.get_time_command_OBCResponseData()
      yield obc_pb2.get_time_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_time_command_OBCResponseData(**{"timestamp": toProtobufTimestamp(resp.data.fields["timestamp"].date_time)}) if resp.date_time.is_immediate else obc_pb2.get_time_command_OBCResponseData()
    yield obc_pb2.get_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def set_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "SET_TIME", alea_time.ALEADateTime(request.timestamp.ToDatetime()), date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.set_time_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.set_time_command_OBCResponseData()
    return obc_pb2.set_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_set_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "SET_TIME", alea_time.ALEADateTime(request.timestamp.ToDatetime()), date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.set_time_command_OBCResponseData()
      yield obc_pb2.set_time_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.set_time_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.set_time_command_OBCResponseData()
    yield obc_pb2.set_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def get_min_stack_space(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_MIN_STACK_SPACE", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_min_stack_space_command_OBCResponseData(**{"min_stack_space": bytes(resp.data.fields["min_stack_space"])}) if resp.date_time.is_immediate else obc_pb2.get_min_stack_space_command_OBCResponseData()
    return obc_pb2.get_min_stack_space_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_get_min_stack_space(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_MIN_STACK_SPACE", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.get_min_stack_space_command_OBCResponseData()
      yield obc_pb2.get_min_stack_space_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_min_stack_space_command_OBCResponseData(**{"min_stack_space": bytes(resp.data.fields["min_stack_space"])}) if resp.date_time.is_immediate else obc_pb2.get_min_stack_space_command_OBCResponseData()
    yield obc_pb2.get_min_stack_space_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def capture_rtos_trace(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "CAPTURE_RTOS_TRACE", request.length, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.capture_rtos_trace_command_OBCResponseData(**{"trace": bytes(resp.data.fields["trace"])}) if resp.date_time.is_immediate else obc_pb2.capture_rtos_trace_command_OBCResponseData()
    return obc_pb2.capture_rtos_trace_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_capture_rtos_trace(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "CAPTURE_RTOS_TRACE", request.length, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.capture_rtos_trace_command_OBCResponseData()
      yield obc_pb2.capture_rtos_trace_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.capture_rtos_trace_command_OBCResponseData(**{"trace": bytes(resp.data.fields["trace"])}) if resp.date_time.is_immediate else obc_pb2.capture_rtos_trace_command_OBCResponseData()
    yield obc_pb2.capture_rtos_trace_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def rtos_state(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "RTOS_STATE", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.rtos_state_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.rtos_state_command_OBCResponseData()
    return obc_pb2.rtos_state_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_rtos_state(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "RTOS_STATE", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.rtos_state_command_OBCResponseData()
      yield obc_pb2.rtos_state_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.rtos_state_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.rtos_state_command_OBCResponseData()
    yield obc_pb2.rtos_state_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def low_power(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "LOW_POWER", request.enable, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.low_power_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.low_power_command_OBCResponseData()
    return obc_pb2.low_power_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_low_power(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "LOW_POWER", request.enable, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.low_power_command_OBCResponseData()
      yield obc_pb2.low_power_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.low_power_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.low_power_command_OBCResponseData()
    yield obc_pb2.low_power_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def sys_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "SYS_TIME", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.sys_time_command_OBCResponseData(**{"sys_time_us": resp.data.fields["sys_time_us"]}) if resp.date_time.is_immediate else obc_pb2.sys_time_command_OBCResponseData()
    return obc_pb2.sys_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_sys_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "SYS_TIME", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.sys_time_command_OBCResponseData()
      yield obc_pb2.sys_time_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.sys_time_command_OBCResponseData(**{"sys_time_us": resp.data.fields["sys_time_us"]}) if resp.date_time.is_immediate else obc_pb2.sys_time_command_OBCResponseData()
    yield obc_pb2.sys_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def cpu_usage(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "CPU_USAGE", request.task, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.cpu_usage_command_OBCResponseData(**{"usage_pct": resp.data.fields["usage_pct"]}) if resp.date_time.is_immediate else obc_pb2.cpu_usage_command_OBCResponseData()
    return obc_pb2.cpu_usage_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_cpu_usage(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "CPU_USAGE", request.task, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.cpu_usage_command_OBCResponseData()
      yield obc_pb2.cpu_usage_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.cpu_usage_command_OBCResponseData(**{"usage_pct": resp.data.fields["usage_pct"]}) if resp.date_time.is_immediate else obc_pb2.cpu_usage_command_OBCResponseData()
    yield obc_pb2.cpu_usage_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def task_runtime(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TASK_RUNTIME", request.task, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.task_runtime_command_OBCResponseData(**{"runtime": resp.data.fields["runtime"]}) if resp.date_time.is_immediate else obc_pb2.task_runtime_command_OBCResponseData()
    return obc_pb2.task_runtime_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_task_runtime(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TASK_RUNTIME", request.task, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.task_runtime_command_OBCResponseData()
      yield obc_pb2.task_runtime_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.task_runtime_command_OBCResponseData(**{"runtime": resp.data.fields["runtime"]}) if resp.date_time.is_immediate else obc_pb2.task_runtime_command_OBCResponseData()
    yield obc_pb2.task_runtime_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gpio_exp_reset(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPIO_EXP_RESET", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gpio_exp_reset_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gpio_exp_reset_command_OBCResponseData()
    return obc_pb2.gpio_exp_reset_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gpio_exp_reset(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPIO_EXP_RESET", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gpio_exp_reset_command_OBCResponseData()
      yield obc_pb2.gpio_exp_reset_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gpio_exp_reset_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gpio_exp_reset_command_OBCResponseData()
    yield obc_pb2.gpio_exp_reset_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def i2c_reset(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "I2C_RESET", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.i2c_reset_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.i2c_reset_command_OBCResponseData()
    return obc_pb2.i2c_reset_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_i2c_reset(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "I2C_RESET", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.i2c_reset_command_OBCResponseData()
      yield obc_pb2.i2c_reset_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.i2c_reset_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.i2c_reset_command_OBCResponseData()
    yield obc_pb2.i2c_reset_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def heartbeat(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "HEARTBEAT", request.enable, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.heartbeat_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.heartbeat_command_OBCResponseData()
    return obc_pb2.heartbeat_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_heartbeat(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "HEARTBEAT", request.enable, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.heartbeat_command_OBCResponseData()
      yield obc_pb2.heartbeat_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.heartbeat_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.heartbeat_command_OBCResponseData()
    yield obc_pb2.heartbeat_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_restart(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_RESTART", request.start_mode, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_restart_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_restart_command_OBCResponseData()
    return obc_pb2.gps_restart_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_restart(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_RESTART", request.start_mode, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_restart_command_OBCResponseData()
      yield obc_pb2.gps_restart_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_restart_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_restart_command_OBCResponseData()
    yield obc_pb2.gps_restart_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def fw_info(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "FW_INFO", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.fw_info_command_OBCResponseData(**{"fw_version": resp.data.fields["fw_version"], "fw_hash": resp.data.fields["fw_hash"], "flash_address": resp.data.fields["flash_address"], "platform": resp.data.fields["platform"], "target": resp.data.fields["target"], "flags": resp.data.fields["flags"], "size": resp.data.fields["size"], "crc32": resp.data.fields["crc32"]}) if resp.date_time.is_immediate else obc_pb2.fw_info_command_OBCResponseData()
    return obc_pb2.fw_info_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_fw_info(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "FW_INFO", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.fw_info_command_OBCResponseData()
      yield obc_pb2.fw_info_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.fw_info_command_OBCResponseData(**{"fw_version": resp.data.fields["fw_version"], "fw_hash": resp.data.fields["fw_hash"], "flash_address": resp.data.fields["flash_address"], "platform": resp.data.fields["platform"], "target": resp.data.fields["target"], "flags": resp.data.fields["flags"], "size": resp.data.fields["size"], "crc32": resp.data.fields["crc32"]}) if resp.date_time.is_immediate else obc_pb2.fw_info_command_OBCResponseData()
    yield obc_pb2.fw_info_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def wmm_init(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "WMM_INIT", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.wmm_init_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.wmm_init_command_OBCResponseData()
    return obc_pb2.wmm_init_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_wmm_init(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "WMM_INIT", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.wmm_init_command_OBCResponseData()
      yield obc_pb2.wmm_init_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.wmm_init_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.wmm_init_command_OBCResponseData()
    yield obc_pb2.wmm_init_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def wmm_is_init(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "WMM_IS_INIT", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.wmm_is_init_command_OBCResponseData(**{"is_init": resp.data.fields["is_init"]}) if resp.date_time.is_immediate else obc_pb2.wmm_is_init_command_OBCResponseData()
    return obc_pb2.wmm_is_init_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_wmm_is_init(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "WMM_IS_INIT", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.wmm_is_init_command_OBCResponseData()
      yield obc_pb2.wmm_is_init_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.wmm_is_init_command_OBCResponseData(**{"is_init": resp.data.fields["is_init"]}) if resp.date_time.is_immediate else obc_pb2.wmm_is_init_command_OBCResponseData()
    yield obc_pb2.wmm_is_init_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def wmm_get_mag_ref(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "WMM_GET_MAG_REF", request.lon, request.lat, request.alt, request.year, request.month, request.day, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.wmm_get_mag_ref_command_OBCResponseData(**{"mag_ref": resp.data.fields["mag_ref"]}) if resp.date_time.is_immediate else obc_pb2.wmm_get_mag_ref_command_OBCResponseData()
    return obc_pb2.wmm_get_mag_ref_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_wmm_get_mag_ref(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "WMM_GET_MAG_REF", request.lon, request.lat, request.alt, request.year, request.month, request.day, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.wmm_get_mag_ref_command_OBCResponseData()
      yield obc_pb2.wmm_get_mag_ref_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.wmm_get_mag_ref_command_OBCResponseData(**{"mag_ref": resp.data.fields["mag_ref"]}) if resp.date_time.is_immediate else obc_pb2.wmm_get_mag_ref_command_OBCResponseData()
    yield obc_pb2.wmm_get_mag_ref_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_query_sw_ver(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_SW_VER", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_sw_ver_command_OBCResponseData(**{"gps_software_version": resp.data.fields["gps_software_version"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_sw_ver_command_OBCResponseData()
    return obc_pb2.gps_query_sw_ver_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_query_sw_ver(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_SW_VER", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_query_sw_ver_command_OBCResponseData()
      yield obc_pb2.gps_query_sw_ver_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_sw_ver_command_OBCResponseData(**{"gps_software_version": resp.data.fields["gps_software_version"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_sw_ver_command_OBCResponseData()
    yield obc_pb2.gps_query_sw_ver_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_set_factory_settings(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_SET_FACTORY_SETTINGS", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_set_factory_settings_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_set_factory_settings_command_OBCResponseData()
    return obc_pb2.gps_set_factory_settings_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_set_factory_settings(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_SET_FACTORY_SETTINGS", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_set_factory_settings_command_OBCResponseData()
      yield obc_pb2.gps_set_factory_settings_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_set_factory_settings_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_set_factory_settings_command_OBCResponseData()
    yield obc_pb2.gps_set_factory_settings_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_configure_power_mode(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_POWER_MODE", request.write_settings, request.enable_power_save, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_power_mode_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_power_mode_command_OBCResponseData()
    return obc_pb2.gps_configure_power_mode_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_configure_power_mode(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_POWER_MODE", request.write_settings, request.enable_power_save, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_configure_power_mode_command_OBCResponseData()
      yield obc_pb2.gps_configure_power_mode_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_power_mode_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_power_mode_command_OBCResponseData()
    yield obc_pb2.gps_configure_power_mode_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_query_power_mode(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_POWER_MODE", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_power_mode_command_OBCResponseData(**{"gps_in_power_save_mode": resp.data.fields["gps_in_power_save_mode"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_power_mode_command_OBCResponseData()
    return obc_pb2.gps_query_power_mode_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_query_power_mode(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_POWER_MODE", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_query_power_mode_command_OBCResponseData()
      yield obc_pb2.gps_query_power_mode_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_power_mode_command_OBCResponseData(**{"gps_in_power_save_mode": resp.data.fields["gps_in_power_save_mode"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_power_mode_command_OBCResponseData()
    yield obc_pb2.gps_query_power_mode_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_query_sw_crc(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_SW_CRC", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_sw_crc_command_OBCResponseData(**{"software_crc": resp.data.fields["software_crc"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_sw_crc_command_OBCResponseData()
    return obc_pb2.gps_query_sw_crc_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_query_sw_crc(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_SW_CRC", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_query_sw_crc_command_OBCResponseData()
      yield obc_pb2.gps_query_sw_crc_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_sw_crc_command_OBCResponseData(**{"software_crc": resp.data.fields["software_crc"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_sw_crc_command_OBCResponseData()
    yield obc_pb2.gps_query_sw_crc_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_configure_position_upate_rate(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_POSITION_UPATE_RATE", request.write_settings, request.position_update_rate, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_position_upate_rate_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_position_upate_rate_command_OBCResponseData()
    return obc_pb2.gps_configure_position_upate_rate_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_configure_position_upate_rate(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_POSITION_UPATE_RATE", request.write_settings, request.position_update_rate, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_configure_position_upate_rate_command_OBCResponseData()
      yield obc_pb2.gps_configure_position_upate_rate_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_position_upate_rate_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_position_upate_rate_command_OBCResponseData()
    yield obc_pb2.gps_configure_position_upate_rate_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_query_position_update_rate(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_POSITION_UPDATE_RATE", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_position_update_rate_command_OBCResponseData(**{"update_rate": resp.data.fields["update_rate"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_position_update_rate_command_OBCResponseData()
    return obc_pb2.gps_query_position_update_rate_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_query_position_update_rate(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_POSITION_UPDATE_RATE", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_query_position_update_rate_command_OBCResponseData()
      yield obc_pb2.gps_query_position_update_rate_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_position_update_rate_command_OBCResponseData(**{"update_rate": resp.data.fields["update_rate"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_position_update_rate_command_OBCResponseData()
    yield obc_pb2.gps_query_position_update_rate_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_configure_nmea_messges(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_NMEA_MESSGES", request.write_settings, request.gga_interval, request.gsa_interval, request.gsv_interval, request.gll_interval, request.rmc_interval, request.vtg_interval, request.zda_interval, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_nmea_messges_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_nmea_messges_command_OBCResponseData()
    return obc_pb2.gps_configure_nmea_messges_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_configure_nmea_messges(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_NMEA_MESSGES", request.write_settings, request.gga_interval, request.gsa_interval, request.gsv_interval, request.gll_interval, request.rmc_interval, request.vtg_interval, request.zda_interval, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_configure_nmea_messges_command_OBCResponseData()
      yield obc_pb2.gps_configure_nmea_messges_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_nmea_messges_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_nmea_messges_command_OBCResponseData()
    yield obc_pb2.gps_configure_nmea_messges_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_query_nmea_messges(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_NMEA_MESSGES", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_nmea_messges_command_OBCResponseData(**{"gga_interval": resp.data.fields["gga_interval"], "gsa_interval": resp.data.fields["gsa_interval"], "gsv_interval": resp.data.fields["gsv_interval"], "gll_interval": resp.data.fields["gll_interval"], "rmc_interval": resp.data.fields["rmc_interval"], "vtg_interval": resp.data.fields["vtg_interval"], "zda_interval": resp.data.fields["zda_interval"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_nmea_messges_command_OBCResponseData()
    return obc_pb2.gps_query_nmea_messges_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_query_nmea_messges(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_NMEA_MESSGES", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_query_nmea_messges_command_OBCResponseData()
      yield obc_pb2.gps_query_nmea_messges_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_nmea_messges_command_OBCResponseData(**{"gga_interval": resp.data.fields["gga_interval"], "gsa_interval": resp.data.fields["gsa_interval"], "gsv_interval": resp.data.fields["gsv_interval"], "gll_interval": resp.data.fields["gll_interval"], "rmc_interval": resp.data.fields["rmc_interval"], "vtg_interval": resp.data.fields["vtg_interval"], "zda_interval": resp.data.fields["zda_interval"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_nmea_messges_command_OBCResponseData()
    yield obc_pb2.gps_query_nmea_messges_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_configure_serial_port(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_SERIAL_PORT", request.baud_rate, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_serial_port_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_serial_port_command_OBCResponseData()
    return obc_pb2.gps_configure_serial_port_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_configure_serial_port(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_SERIAL_PORT", request.baud_rate, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_configure_serial_port_command_OBCResponseData()
      yield obc_pb2.gps_configure_serial_port_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_serial_port_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_serial_port_command_OBCResponseData()
    yield obc_pb2.gps_configure_serial_port_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def gps_configure_nav_msg_interval(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_NAV_MSG_INTERVAL", request.write_settings, request.navigation_msg_int, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_nav_msg_interval_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_nav_msg_interval_command_OBCResponseData()
    return obc_pb2.gps_configure_nav_msg_interval_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_gps_configure_nav_msg_interval(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_NAV_MSG_INTERVAL", request.write_settings, request.navigation_msg_int, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.gps_configure_nav_msg_interval_command_OBCResponseData()
      yield obc_pb2.gps_configure_nav_msg_interval_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_nav_msg_interval_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_nav_msg_interval_command_OBCResponseData()
    yield obc_pb2.gps_configure_nav_msg_interval_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def get_uptime(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_UPTIME", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_uptime_command_OBCResponseData(**{"seconds": resp.data.fields["seconds"]}) if resp.date_time.is_immediate else obc_pb2.get_uptime_command_OBCResponseData()
    return obc_pb2.get_uptime_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_get_uptime(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_UPTIME", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.get_uptime_command_OBCResponseData()
      yield obc_pb2.get_uptime_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_uptime_command_OBCResponseData(**{"seconds": resp.data.fields["seconds"]}) if resp.date_time.is_immediate else obc_pb2.get_uptime_command_OBCResponseData()
    yield obc_pb2.get_uptime_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def flash_sleep(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "FLASH_SLEEP", request.enable, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.flash_sleep_command_OBCResponseData(**{"fs_err": resp.data.fields["fs_err"]}) if resp.date_time.is_immediate else obc_pb2.flash_sleep_command_OBCResponseData()
    return obc_pb2.flash_sleep_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_flash_sleep(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "FLASH_SLEEP", request.enable, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.flash_sleep_command_OBCResponseData()
      yield obc_pb2.flash_sleep_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.flash_sleep_command_OBCResponseData(**{"fs_err": resp.data.fields["fs_err"]}) if resp.date_time.is_immediate else obc_pb2.flash_sleep_command_OBCResponseData()
    yield obc_pb2.flash_sleep_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def get_telemetry(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_TELEMETRY", request.priority, request.size, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_telemetry_command_OBCResponseData(**{"data": bytes(resp.data.fields["data"])}) if resp.date_time.is_immediate else obc_pb2.get_telemetry_command_OBCResponseData()
    return obc_pb2.get_telemetry_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_get_telemetry(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_TELEMETRY", request.priority, request.size, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.get_telemetry_command_OBCResponseData()
      yield obc_pb2.get_telemetry_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_telemetry_command_OBCResponseData(**{"data": bytes(resp.data.fields["data"])}) if resp.date_time.is_immediate else obc_pb2.get_telemetry_command_OBCResponseData()
    yield obc_pb2.get_telemetry_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_echo(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_ECHO", request.number, request.array.data, request.arrayf.data, request.message, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_echo_command_OBCResponseData(**{"number": resp.data.fields["number"], "array": toMatrix(resp.data.fields["array"], "u32", 2, 3), "arrayf": toMatrix(resp.data.fields["arrayf"], "f32", 2, 3), "message": resp.data.fields["message"]}) if resp.date_time.is_immediate else obc_pb2.test_echo_command_OBCResponseData()
    return obc_pb2.test_echo_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_echo(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_ECHO", request.number, request.array.data, request.arrayf.data, request.message, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_echo_command_OBCResponseData()
      yield obc_pb2.test_echo_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_echo_command_OBCResponseData(**{"number": resp.data.fields["number"], "array": toMatrix(resp.data.fields["array"], "u32", 2, 3), "arrayf": toMatrix(resp.data.fields["arrayf"], "f32", 2, 3), "message": resp.data.fields["message"]}) if resp.date_time.is_immediate else obc_pb2.test_echo_command_OBCResponseData()
    yield obc_pb2.test_echo_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_hang(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_HANG", request.duration_us, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_hang_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_hang_command_OBCResponseData()
    return obc_pb2.test_hang_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_hang(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_HANG", request.duration_us, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_hang_command_OBCResponseData()
      yield obc_pb2.test_hang_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_hang_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_hang_command_OBCResponseData()
    yield obc_pb2.test_hang_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_comms_tx_rx(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_COMMS_TX_RX", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_comms_tx_rx_command_OBCResponseData(**{"comms_err": resp.data.fields["comms_err"], "command": resp.data.fields["command"]}) if resp.date_time.is_immediate else obc_pb2.test_comms_tx_rx_command_OBCResponseData()
    return obc_pb2.test_comms_tx_rx_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_comms_tx_rx(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_COMMS_TX_RX", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_comms_tx_rx_command_OBCResponseData()
      yield obc_pb2.test_comms_tx_rx_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_comms_tx_rx_command_OBCResponseData(**{"comms_err": resp.data.fields["comms_err"], "command": resp.data.fields["command"]}) if resp.date_time.is_immediate else obc_pb2.test_comms_tx_rx_command_OBCResponseData()
    yield obc_pb2.test_comms_tx_rx_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_comms_stress1(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_COMMS_STRESS1", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_comms_stress1_command_OBCResponseData(**{"pass": resp.data.fields["pass"], "fail": resp.data.fields["fail"]}) if resp.date_time.is_immediate else obc_pb2.test_comms_stress1_command_OBCResponseData()
    return obc_pb2.test_comms_stress1_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_comms_stress1(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_COMMS_STRESS1", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_comms_stress1_command_OBCResponseData()
      yield obc_pb2.test_comms_stress1_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_comms_stress1_command_OBCResponseData(**{"pass": resp.data.fields["pass"], "fail": resp.data.fields["fail"]}) if resp.date_time.is_immediate else obc_pb2.test_comms_stress1_command_OBCResponseData()
    yield obc_pb2.test_comms_stress1_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_comms_flash_app(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_COMMS_FLASH_APP", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_comms_flash_app_command_OBCResponseData(**{"comms_err": resp.data.fields["comms_err"]}) if resp.date_time.is_immediate else obc_pb2.test_comms_flash_app_command_OBCResponseData()
    return obc_pb2.test_comms_flash_app_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_comms_flash_app(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_COMMS_FLASH_APP", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_comms_flash_app_command_OBCResponseData()
      yield obc_pb2.test_comms_flash_app_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_comms_flash_app_command_OBCResponseData(**{"comms_err": resp.data.fields["comms_err"]}) if resp.date_time.is_immediate else obc_pb2.test_comms_flash_app_command_OBCResponseData()
    yield obc_pb2.test_comms_flash_app_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_comms_reboot(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_COMMS_REBOOT", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_comms_reboot_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_comms_reboot_command_OBCResponseData()
    return obc_pb2.test_comms_reboot_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_comms_reboot(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_COMMS_REBOOT", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_comms_reboot_command_OBCResponseData()
      yield obc_pb2.test_comms_reboot_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_comms_reboot_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_comms_reboot_command_OBCResponseData()
    yield obc_pb2.test_comms_reboot_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_comms_get_telem(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_COMMS_GET_TELEM", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_comms_get_telem_command_OBCResponseData(**{"uptime": resp.data.fields["uptime"], "uart0_rx_count": resp.data.fields["uart0_rx_count"], "uart1_rx_count": resp.data.fields["uart1_rx_count"], "rx_mode": resp.data.fields["rx_mode"], "tx_mode": resp.data.fields["tx_mode"], "adc": resp.data.fields["adc"], "rssi": resp.data.fields["rssi"], "last_lqi": resp.data.fields["last_lqi"], "last_frequest": resp.data.fields["last_frequest"], "packets_sent": resp.data.fields["packets_sent"], "cs_count": resp.data.fields["cs_count"], "packets_good": resp.data.fields["packets_good"], "packets_rejected_checksum": resp.data.fields["packets_rejected_checksum"], "packets_rejected_other": resp.data.fields["packets_rejected_other"], "custom0": resp.data.fields["custom0"], "custom1": resp.data.fields["custom1"]}) if resp.date_time.is_immediate else obc_pb2.test_comms_get_telem_command_OBCResponseData()
    return obc_pb2.test_comms_get_telem_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_comms_get_telem(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_COMMS_GET_TELEM", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_comms_get_telem_command_OBCResponseData()
      yield obc_pb2.test_comms_get_telem_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_comms_get_telem_command_OBCResponseData(**{"uptime": resp.data.fields["uptime"], "uart0_rx_count": resp.data.fields["uart0_rx_count"], "uart1_rx_count": resp.data.fields["uart1_rx_count"], "rx_mode": resp.data.fields["rx_mode"], "tx_mode": resp.data.fields["tx_mode"], "adc": resp.data.fields["adc"], "rssi": resp.data.fields["rssi"], "last_lqi": resp.data.fields["last_lqi"], "last_frequest": resp.data.fields["last_frequest"], "packets_sent": resp.data.fields["packets_sent"], "cs_count": resp.data.fields["cs_count"], "packets_good": resp.data.fields["packets_good"], "packets_rejected_checksum": resp.data.fields["packets_rejected_checksum"], "packets_rejected_other": resp.data.fields["packets_rejected_other"], "custom0": resp.data.fields["custom0"], "custom1": resp.data.fields["custom1"]}) if resp.date_time.is_immediate else obc_pb2.test_comms_get_telem_command_OBCResponseData()
    yield obc_pb2.test_comms_get_telem_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_eps_read_float(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_FLOAT", request.read_cmd, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_float_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "data": resp.data.fields["data"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_float_command_OBCResponseData()
    return obc_pb2.test_eps_read_float_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_eps_read_float(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_FLOAT", request.read_cmd, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_eps_read_float_command_OBCResponseData()
      yield obc_pb2.test_eps_read_float_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_float_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "data": resp.data.fields["data"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_float_command_OBCResponseData()
    yield obc_pb2.test_eps_read_float_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_eps_read_float_raw(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_FLOAT_RAW", request.read_cmd, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_float_raw_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "data": resp.data.fields["data"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_float_raw_command_OBCResponseData()
    return obc_pb2.test_eps_read_float_raw_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_eps_read_float_raw(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_FLOAT_RAW", request.read_cmd, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_eps_read_float_raw_command_OBCResponseData()
      yield obc_pb2.test_eps_read_float_raw_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_float_raw_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "data": resp.data.fields["data"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_float_raw_command_OBCResponseData()
    yield obc_pb2.test_eps_read_float_raw_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_eps_read_int(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_INT", request.read_cmd, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_int_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "data": resp.data.fields["data"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_int_command_OBCResponseData()
    return obc_pb2.test_eps_read_int_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_eps_read_int(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_INT", request.read_cmd, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_eps_read_int_command_OBCResponseData()
      yield obc_pb2.test_eps_read_int_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_int_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "data": resp.data.fields["data"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_int_command_OBCResponseData()
    yield obc_pb2.test_eps_read_int_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_eps_write(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_WRITE", request.write_cmd, request.state, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_write_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_write_command_OBCResponseData()
    return obc_pb2.test_eps_write_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_eps_write(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_WRITE", request.write_cmd, request.state, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_eps_write_command_OBCResponseData()
      yield obc_pb2.test_eps_write_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_write_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_write_command_OBCResponseData()
    yield obc_pb2.test_eps_write_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_eps_read_sanity(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_SANITY", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_sanity_command_OBCResponseData(**{"success_counter": resp.data.fields["success_counter"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_sanity_command_OBCResponseData()
    return obc_pb2.test_eps_read_sanity_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_eps_read_sanity(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_SANITY", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_eps_read_sanity_command_OBCResponseData()
      yield obc_pb2.test_eps_read_sanity_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_sanity_command_OBCResponseData(**{"success_counter": resp.data.fields["success_counter"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_sanity_command_OBCResponseData()
    yield obc_pb2.test_eps_read_sanity_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_eps_writeread_sanity(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_WRITEREAD_SANITY", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_writeread_sanity_command_OBCResponseData(**{"success_counter": resp.data.fields["success_counter"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_writeread_sanity_command_OBCResponseData()
    return obc_pb2.test_eps_writeread_sanity_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_eps_writeread_sanity(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_WRITEREAD_SANITY", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_eps_writeread_sanity_command_OBCResponseData()
      yield obc_pb2.test_eps_writeread_sanity_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_writeread_sanity_command_OBCResponseData(**{"success_counter": resp.data.fields["success_counter"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_writeread_sanity_command_OBCResponseData()
    yield obc_pb2.test_eps_writeread_sanity_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_eps_measure_buses(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_MEASURE_BUSES", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_measure_buses_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "batt_V": resp.data.fields["batt_V"], "batt_A": resp.data.fields["batt_A"], "bcr_V": resp.data.fields["bcr_V"], "bcr_A": resp.data.fields["bcr_A"], "bus_3v3_V": resp.data.fields["bus_3v3_V"], "bus_3v3_A": resp.data.fields["bus_3v3_A"], "bus_5v_V": resp.data.fields["bus_5v_V"], "bus_5v_A": resp.data.fields["bus_5v_A"], "lup_3v3_V": resp.data.fields["lup_3v3_V"], "lup_5v_V": resp.data.fields["lup_5v_V"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_measure_buses_command_OBCResponseData()
    return obc_pb2.test_eps_measure_buses_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_eps_measure_buses(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_MEASURE_BUSES", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_eps_measure_buses_command_OBCResponseData()
      yield obc_pb2.test_eps_measure_buses_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_measure_buses_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "batt_V": resp.data.fields["batt_V"], "batt_A": resp.data.fields["batt_A"], "bcr_V": resp.data.fields["bcr_V"], "bcr_A": resp.data.fields["bcr_A"], "bus_3v3_V": resp.data.fields["bus_3v3_V"], "bus_3v3_A": resp.data.fields["bus_3v3_A"], "bus_5v_V": resp.data.fields["bus_5v_V"], "bus_5v_A": resp.data.fields["bus_5v_A"], "lup_3v3_V": resp.data.fields["lup_3v3_V"], "lup_5v_V": resp.data.fields["lup_5v_V"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_measure_buses_command_OBCResponseData()
    yield obc_pb2.test_eps_measure_buses_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_eps_measure_temps(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_MEASURE_TEMPS", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_measure_temps_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "cell_temps": resp.data.fields["cell_temps"], "mcu_temp": resp.data.fields["mcu_temp"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_measure_temps_command_OBCResponseData()
    return obc_pb2.test_eps_measure_temps_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_eps_measure_temps(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_MEASURE_TEMPS", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_eps_measure_temps_command_OBCResponseData()
      yield obc_pb2.test_eps_measure_temps_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_measure_temps_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "cell_temps": resp.data.fields["cell_temps"], "mcu_temp": resp.data.fields["mcu_temp"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_measure_temps_command_OBCResponseData()
    yield obc_pb2.test_eps_measure_temps_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_eps_read_counters(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_COUNTERS", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_counters_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "uptime": resp.data.fields["uptime"], "power_on_cycles": resp.data.fields["power_on_cycles"], "under_voltage": resp.data.fields["under_voltage"], "short_circuit": resp.data.fields["short_circuit"], "over_temp": resp.data.fields["over_temp"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_counters_command_OBCResponseData()
    return obc_pb2.test_eps_read_counters_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_eps_read_counters(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_COUNTERS", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_eps_read_counters_command_OBCResponseData()
      yield obc_pb2.test_eps_read_counters_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_counters_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "uptime": resp.data.fields["uptime"], "power_on_cycles": resp.data.fields["power_on_cycles"], "under_voltage": resp.data.fields["under_voltage"], "short_circuit": resp.data.fields["short_circuit"], "over_temp": resp.data.fields["over_temp"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_counters_command_OBCResponseData()
    yield obc_pb2.test_eps_read_counters_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_eps_read_status(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_STATUS", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_status_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "out_cond_1": resp.data.fields["out_cond_1"], "out_cond_2": resp.data.fields["out_cond_2"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_status_command_OBCResponseData()
    return obc_pb2.test_eps_read_status_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_eps_read_status(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_EPS_READ_STATUS", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_eps_read_status_command_OBCResponseData()
      yield obc_pb2.test_eps_read_status_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_eps_read_status_command_OBCResponseData(**{"eps_err": resp.data.fields["eps_err"], "out_cond_1": resp.data.fields["out_cond_1"], "out_cond_2": resp.data.fields["out_cond_2"]}) if resp.date_time.is_immediate else obc_pb2.test_eps_read_status_command_OBCResponseData()
    yield obc_pb2.test_eps_read_status_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_mag(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MAG", request.magnetorquer, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mag_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_mag_command_OBCResponseData()
    return obc_pb2.test_mag_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_mag(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MAG", request.magnetorquer, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_mag_command_OBCResponseData()
      yield obc_pb2.test_mag_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mag_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_mag_command_OBCResponseData()
    yield obc_pb2.test_mag_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_mag_all(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MAG_ALL", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mag_all_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_mag_all_command_OBCResponseData()
    return obc_pb2.test_mag_all_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_mag_all(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MAG_ALL", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_mag_all_command_OBCResponseData()
      yield obc_pb2.test_mag_all_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mag_all_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_mag_all_command_OBCResponseData()
    yield obc_pb2.test_mag_all_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_mag_init(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MAG_INIT", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mag_init_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_mag_init_command_OBCResponseData()
    return obc_pb2.test_mag_init_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_mag_init(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MAG_INIT", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_mag_init_command_OBCResponseData()
      yield obc_pb2.test_mag_init_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mag_init_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_mag_init_command_OBCResponseData()
    yield obc_pb2.test_mag_init_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_imu(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_IMU", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_imu_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_imu_command_OBCResponseData()
    return obc_pb2.test_imu_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_imu(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_IMU", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_imu_command_OBCResponseData()
      yield obc_pb2.test_imu_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_imu_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_imu_command_OBCResponseData()
    yield obc_pb2.test_imu_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_panel_gyro(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_PANEL_GYRO", request.gyro, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_panel_gyro_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_panel_gyro_command_OBCResponseData()
    return obc_pb2.test_panel_gyro_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_panel_gyro(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_PANEL_GYRO", request.gyro, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_panel_gyro_command_OBCResponseData()
      yield obc_pb2.test_panel_gyro_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_panel_gyro_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_panel_gyro_command_OBCResponseData()
    yield obc_pb2.test_panel_gyro_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_can_gpio(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAN_GPIO", request.port, request.pin, request.value, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_can_gpio_command_OBCResponseData(**{"gpio_err": resp.data.fields["gpio_err"]}) if resp.date_time.is_immediate else obc_pb2.test_can_gpio_command_OBCResponseData()
    return obc_pb2.test_can_gpio_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_can_gpio(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAN_GPIO", request.port, request.pin, request.value, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_can_gpio_command_OBCResponseData()
      yield obc_pb2.test_can_gpio_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_can_gpio_command_OBCResponseData(**{"gpio_err": resp.data.fields["gpio_err"]}) if resp.date_time.is_immediate else obc_pb2.test_can_gpio_command_OBCResponseData()
    yield obc_pb2.test_can_gpio_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_flash_rw(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_FLASH_RW", request.addr, request.len, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_flash_rw_command_OBCResponseData(**{"erase_err": resp.data.fields["erase_err"], "write_err": resp.data.fields["write_err"], "read_err": resp.data.fields["read_err"], "data_match": resp.data.fields["data_match"]}) if resp.date_time.is_immediate else obc_pb2.test_flash_rw_command_OBCResponseData()
    return obc_pb2.test_flash_rw_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_flash_rw(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_FLASH_RW", request.addr, request.len, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_flash_rw_command_OBCResponseData()
      yield obc_pb2.test_flash_rw_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_flash_rw_command_OBCResponseData(**{"erase_err": resp.data.fields["erase_err"], "write_err": resp.data.fields["write_err"], "read_err": resp.data.fields["read_err"], "data_match": resp.data.fields["data_match"]}) if resp.date_time.is_immediate else obc_pb2.test_flash_rw_command_OBCResponseData()
    yield obc_pb2.test_flash_rw_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_filesystem(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_FILESYSTEM", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_filesystem_command_OBCResponseData(**{"fs_err": resp.data.fields["fs_err"]}) if resp.date_time.is_immediate else obc_pb2.test_filesystem_command_OBCResponseData()
    return obc_pb2.test_filesystem_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_filesystem(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_FILESYSTEM", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_filesystem_command_OBCResponseData()
      yield obc_pb2.test_filesystem_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_filesystem_command_OBCResponseData(**{"fs_err": resp.data.fields["fs_err"]}) if resp.date_time.is_immediate else obc_pb2.test_filesystem_command_OBCResponseData()
    yield obc_pb2.test_filesystem_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_cam_init(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAM_INIT", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_cam_init_command_OBCResponseData(**{"arducam_err": resp.data.fields["arducam_err"]}) if resp.date_time.is_immediate else obc_pb2.test_cam_init_command_OBCResponseData()
    return obc_pb2.test_cam_init_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_cam_init(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAM_INIT", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_cam_init_command_OBCResponseData()
      yield obc_pb2.test_cam_init_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_cam_init_command_OBCResponseData(**{"arducam_err": resp.data.fields["arducam_err"]}) if resp.date_time.is_immediate else obc_pb2.test_cam_init_command_OBCResponseData()
    yield obc_pb2.test_cam_init_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_cam_capture(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAM_CAPTURE", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_cam_capture_command_OBCResponseData(**{"arducam_err": resp.data.fields["arducam_err"], "image_size": resp.data.fields["image_size"], "image_data": bytes(resp.data.fields["image_data"])}) if resp.date_time.is_immediate else obc_pb2.test_cam_capture_command_OBCResponseData()
    return obc_pb2.test_cam_capture_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_cam_capture(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAM_CAPTURE", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_cam_capture_command_OBCResponseData()
      yield obc_pb2.test_cam_capture_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_cam_capture_command_OBCResponseData(**{"arducam_err": resp.data.fields["arducam_err"], "image_size": resp.data.fields["image_size"], "image_data": bytes(resp.data.fields["image_data"])}) if resp.date_time.is_immediate else obc_pb2.test_cam_capture_command_OBCResponseData()
    yield obc_pb2.test_cam_capture_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_cam_wr_sreg(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAM_WR_SREG", request.addr, request.data, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_cam_wr_sreg_command_OBCResponseData(**{"ov5642_err": resp.data.fields["ov5642_err"]}) if resp.date_time.is_immediate else obc_pb2.test_cam_wr_sreg_command_OBCResponseData()
    return obc_pb2.test_cam_wr_sreg_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_cam_wr_sreg(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAM_WR_SREG", request.addr, request.data, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_cam_wr_sreg_command_OBCResponseData()
      yield obc_pb2.test_cam_wr_sreg_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_cam_wr_sreg_command_OBCResponseData(**{"ov5642_err": resp.data.fields["ov5642_err"]}) if resp.date_time.is_immediate else obc_pb2.test_cam_wr_sreg_command_OBCResponseData()
    yield obc_pb2.test_cam_wr_sreg_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_cam_rd_sreg(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAM_RD_SREG", request.addr, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_cam_rd_sreg_command_OBCResponseData(**{"ov5642_err": resp.data.fields["ov5642_err"], "data": resp.data.fields["data"]}) if resp.date_time.is_immediate else obc_pb2.test_cam_rd_sreg_command_OBCResponseData()
    return obc_pb2.test_cam_rd_sreg_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_cam_rd_sreg(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAM_RD_SREG", request.addr, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_cam_rd_sreg_command_OBCResponseData()
      yield obc_pb2.test_cam_rd_sreg_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_cam_rd_sreg_command_OBCResponseData(**{"ov5642_err": resp.data.fields["ov5642_err"], "data": resp.data.fields["data"]}) if resp.date_time.is_immediate else obc_pb2.test_cam_rd_sreg_command_OBCResponseData()
    yield obc_pb2.test_cam_rd_sreg_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_cam_wr_sregs(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAM_WR_SREGS", request.reg_data, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_cam_wr_sregs_command_OBCResponseData(**{"ov5642_err": resp.data.fields["ov5642_err"]}) if resp.date_time.is_immediate else obc_pb2.test_cam_wr_sregs_command_OBCResponseData()
    return obc_pb2.test_cam_wr_sregs_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_cam_wr_sregs(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_CAM_WR_SREGS", request.reg_data, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_cam_wr_sregs_command_OBCResponseData()
      yield obc_pb2.test_cam_wr_sregs_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_cam_wr_sregs_command_OBCResponseData(**{"ov5642_err": resp.data.fields["ov5642_err"]}) if resp.date_time.is_immediate else obc_pb2.test_cam_wr_sregs_command_OBCResponseData()
    yield obc_pb2.test_cam_wr_sregs_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_dsplib(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_DSPLIB", request.x, request.y, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_dsplib_command_OBCResponseData(**{"x_abs": resp.data.fields["x_abs"], "y_abs": resp.data.fields["y_abs"]}) if resp.date_time.is_immediate else obc_pb2.test_dsplib_command_OBCResponseData()
    return obc_pb2.test_dsplib_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_dsplib(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_DSPLIB", request.x, request.y, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_dsplib_command_OBCResponseData()
      yield obc_pb2.test_dsplib_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_dsplib_command_OBCResponseData(**{"x_abs": resp.data.fields["x_abs"], "y_abs": resp.data.fields["y_abs"]}) if resp.date_time.is_immediate else obc_pb2.test_dsplib_command_OBCResponseData()
    yield obc_pb2.test_dsplib_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_quest(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_QUEST", request.sun_obs, request.sun_ref, request.mag_obs, request.mag_ref, request.weights, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_quest_command_OBCResponseData(**{"quat": resp.data.fields["quat"], "adcs_err": resp.data.fields["adcs_err"], "avg_duration": resp.data.fields["avg_duration"]}) if resp.date_time.is_immediate else obc_pb2.test_quest_command_OBCResponseData()
    return obc_pb2.test_quest_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_quest(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_QUEST", request.sun_obs, request.sun_ref, request.mag_obs, request.mag_ref, request.weights, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_quest_command_OBCResponseData()
      yield obc_pb2.test_quest_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_quest_command_OBCResponseData(**{"quat": resp.data.fields["quat"], "adcs_err": resp.data.fields["adcs_err"], "avg_duration": resp.data.fields["avg_duration"]}) if resp.date_time.is_immediate else obc_pb2.test_quest_command_OBCResponseData()
    yield obc_pb2.test_quest_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_rtc_get_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_RTC_GET_TIME", request.duration_s, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_rtc_get_time_command_OBCResponseData(**{"second": resp.data.fields["second"], "minute": resp.data.fields["minute"], "hour": resp.data.fields["hour"], "day": resp.data.fields["day"], "month": resp.data.fields["month"], "year": resp.data.fields["year"]}) if resp.date_time.is_immediate else obc_pb2.test_rtc_get_time_command_OBCResponseData()
    return obc_pb2.test_rtc_get_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_rtc_get_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_RTC_GET_TIME", request.duration_s, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_rtc_get_time_command_OBCResponseData()
      yield obc_pb2.test_rtc_get_time_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_rtc_get_time_command_OBCResponseData(**{"second": resp.data.fields["second"], "minute": resp.data.fields["minute"], "hour": resp.data.fields["hour"], "day": resp.data.fields["day"], "month": resp.data.fields["month"], "year": resp.data.fields["year"]}) if resp.date_time.is_immediate else obc_pb2.test_rtc_get_time_command_OBCResponseData()
    yield obc_pb2.test_rtc_get_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_rtc_cap_and_get_timestamp(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_RTC_CAP_AND_GET_TIMESTAMP", request.duration_s, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_rtc_cap_and_get_timestamp_command_OBCResponseData(**{"init_second": resp.data.fields["init_second"], "init_minute": resp.data.fields["init_minute"], "init_hour": resp.data.fields["init_hour"], "init_day": resp.data.fields["init_day"], "init_month": resp.data.fields["init_month"], "init_year": resp.data.fields["init_year"], "ts_second": resp.data.fields["ts_second"], "ts_minute": resp.data.fields["ts_minute"], "ts_hour": resp.data.fields["ts_hour"], "ts_day": resp.data.fields["ts_day"], "ts_month": resp.data.fields["ts_month"], "ts_year": resp.data.fields["ts_year"]}) if resp.date_time.is_immediate else obc_pb2.test_rtc_cap_and_get_timestamp_command_OBCResponseData()
    return obc_pb2.test_rtc_cap_and_get_timestamp_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_rtc_cap_and_get_timestamp(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_RTC_CAP_AND_GET_TIMESTAMP", request.duration_s, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_rtc_cap_and_get_timestamp_command_OBCResponseData()
      yield obc_pb2.test_rtc_cap_and_get_timestamp_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_rtc_cap_and_get_timestamp_command_OBCResponseData(**{"init_second": resp.data.fields["init_second"], "init_minute": resp.data.fields["init_minute"], "init_hour": resp.data.fields["init_hour"], "init_day": resp.data.fields["init_day"], "init_month": resp.data.fields["init_month"], "init_year": resp.data.fields["init_year"], "ts_second": resp.data.fields["ts_second"], "ts_minute": resp.data.fields["ts_minute"], "ts_hour": resp.data.fields["ts_hour"], "ts_day": resp.data.fields["ts_day"], "ts_month": resp.data.fields["ts_month"], "ts_year": resp.data.fields["ts_year"]}) if resp.date_time.is_immediate else obc_pb2.test_rtc_cap_and_get_timestamp_command_OBCResponseData()
    yield obc_pb2.test_rtc_cap_and_get_timestamp_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def debug_read_rtc_reg(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "DEBUG_READ_RTC_REG", request.reg, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.debug_read_rtc_reg_command_OBCResponseData(**{"val": resp.data.fields["val"]}) if resp.date_time.is_immediate else obc_pb2.debug_read_rtc_reg_command_OBCResponseData()
    return obc_pb2.debug_read_rtc_reg_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_debug_read_rtc_reg(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "DEBUG_READ_RTC_REG", request.reg, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.debug_read_rtc_reg_command_OBCResponseData()
      yield obc_pb2.debug_read_rtc_reg_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.debug_read_rtc_reg_command_OBCResponseData(**{"val": resp.data.fields["val"]}) if resp.date_time.is_immediate else obc_pb2.debug_read_rtc_reg_command_OBCResponseData()
    yield obc_pb2.debug_read_rtc_reg_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def debug_write_rtc_reg(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "DEBUG_WRITE_RTC_REG", request.reg, request.val, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.debug_write_rtc_reg_command_OBCResponseData(**{"val_after_write": resp.data.fields["val_after_write"]}) if resp.date_time.is_immediate else obc_pb2.debug_write_rtc_reg_command_OBCResponseData()
    return obc_pb2.debug_write_rtc_reg_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_debug_write_rtc_reg(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "DEBUG_WRITE_RTC_REG", request.reg, request.val, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.debug_write_rtc_reg_command_OBCResponseData()
      yield obc_pb2.debug_write_rtc_reg_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.debug_write_rtc_reg_command_OBCResponseData(**{"val_after_write": resp.data.fields["val_after_write"]}) if resp.date_time.is_immediate else obc_pb2.debug_write_rtc_reg_command_OBCResponseData()
    yield obc_pb2.debug_write_rtc_reg_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_rtc_alarm_n_sec(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_RTC_ALARM_N_SEC", request.duration_s, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_rtc_alarm_n_sec_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_rtc_alarm_n_sec_command_OBCResponseData()
    return obc_pb2.test_rtc_alarm_n_sec_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_rtc_alarm_n_sec(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_RTC_ALARM_N_SEC", request.duration_s, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_rtc_alarm_n_sec_command_OBCResponseData()
      yield obc_pb2.test_rtc_alarm_n_sec_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_rtc_alarm_n_sec_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.test_rtc_alarm_n_sec_command_OBCResponseData()
    yield obc_pb2.test_rtc_alarm_n_sec_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_adcs_sun_model(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_ADCS_SUN_MODEL", request.time, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_adcs_sun_model_command_OBCResponseData(**{"jul": resp.data.fields["jul"], "pos": resp.data.fields["pos"]}) if resp.date_time.is_immediate else obc_pb2.test_adcs_sun_model_command_OBCResponseData()
    return obc_pb2.test_adcs_sun_model_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_adcs_sun_model(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_ADCS_SUN_MODEL", request.time, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_adcs_sun_model_command_OBCResponseData()
      yield obc_pb2.test_adcs_sun_model_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_adcs_sun_model_command_OBCResponseData(**{"jul": resp.data.fields["jul"], "pos": resp.data.fields["pos"]}) if resp.date_time.is_immediate else obc_pb2.test_adcs_sun_model_command_OBCResponseData()
    yield obc_pb2.test_adcs_sun_model_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_adc_voltage(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_ADC_VOLTAGE", request.channel, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_adc_voltage_command_OBCResponseData(**{"channel_voltage": resp.data.fields["channel_voltage"], "adc_err_status": resp.data.fields["adc_err_status"]}) if resp.date_time.is_immediate else obc_pb2.test_adc_voltage_command_OBCResponseData()
    return obc_pb2.test_adc_voltage_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_adc_voltage(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_ADC_VOLTAGE", request.channel, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_adc_voltage_command_OBCResponseData()
      yield obc_pb2.test_adc_voltage_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_adc_voltage_command_OBCResponseData(**{"channel_voltage": resp.data.fields["channel_voltage"], "adc_err_status": resp.data.fields["adc_err_status"]}) if resp.date_time.is_immediate else obc_pb2.test_adc_voltage_command_OBCResponseData()
    yield obc_pb2.test_adc_voltage_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_large_payload_split(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_LARGE_PAYLOAD_SPLIT", request.data, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_large_payload_split_command_OBCResponseData(**{"sum": resp.data.fields["sum"]}) if resp.date_time.is_immediate else obc_pb2.test_large_payload_split_command_OBCResponseData()
    return obc_pb2.test_large_payload_split_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_large_payload_split(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_LARGE_PAYLOAD_SPLIT", request.data, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_large_payload_split_command_OBCResponseData()
      yield obc_pb2.test_large_payload_split_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_large_payload_split_command_OBCResponseData(**{"sum": resp.data.fields["sum"]}) if resp.date_time.is_immediate else obc_pb2.test_large_payload_split_command_OBCResponseData()
    yield obc_pb2.test_large_payload_split_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_mram_rw(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MRAM_RW", request.addr, request.len, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mram_rw_command_OBCResponseData(**{"erase_err": resp.data.fields["erase_err"], "write_err": resp.data.fields["write_err"], "read_err": resp.data.fields["read_err"], "data_match": resp.data.fields["data_match"]}) if resp.date_time.is_immediate else obc_pb2.test_mram_rw_command_OBCResponseData()
    return obc_pb2.test_mram_rw_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_mram_rw(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MRAM_RW", request.addr, request.len, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_mram_rw_command_OBCResponseData()
      yield obc_pb2.test_mram_rw_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mram_rw_command_OBCResponseData(**{"erase_err": resp.data.fields["erase_err"], "write_err": resp.data.fields["write_err"], "read_err": resp.data.fields["read_err"], "data_match": resp.data.fields["data_match"]}) if resp.date_time.is_immediate else obc_pb2.test_mram_rw_command_OBCResponseData()
    yield obc_pb2.test_mram_rw_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_mram_write(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MRAM_WRITE", request.addr, request.write_data, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mram_write_command_OBCResponseData(**{"write_err": resp.data.fields["write_err"]}) if resp.date_time.is_immediate else obc_pb2.test_mram_write_command_OBCResponseData()
    return obc_pb2.test_mram_write_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_mram_write(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MRAM_WRITE", request.addr, request.write_data, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_mram_write_command_OBCResponseData()
      yield obc_pb2.test_mram_write_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mram_write_command_OBCResponseData(**{"write_err": resp.data.fields["write_err"]}) if resp.date_time.is_immediate else obc_pb2.test_mram_write_command_OBCResponseData()
    yield obc_pb2.test_mram_write_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_mram_read(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MRAM_READ", request.addr, request.len, date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mram_read_command_OBCResponseData(**{"read_err": resp.data.fields["read_err"], "data": bytes(resp.data.fields["data"])}) if resp.date_time.is_immediate else obc_pb2.test_mram_read_command_OBCResponseData()
    return obc_pb2.test_mram_read_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_mram_read(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_MRAM_READ", request.addr, request.len, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_mram_read_command_OBCResponseData()
      yield obc_pb2.test_mram_read_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_mram_read_command_OBCResponseData(**{"read_err": resp.data.fields["read_err"], "data": bytes(resp.data.fields["data"])}) if resp.date_time.is_immediate else obc_pb2.test_mram_read_command_OBCResponseData()
    yield obc_pb2.test_mram_read_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def test_telem_glv(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_TELEM_GLV", date_time=alea_time.IMMEDIATE, timeout=request.timeout))
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_telem_glv_command_OBCResponseData(**{"epoch": resp.data.fields["epoch"]}) if resp.date_time.is_immediate else obc_pb2.test_telem_glv_command_OBCResponseData()
    return obc_pb2.test_telem_glv_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)
  async def schedule_test_telem_glv(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TEST_TELEM_GLV", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.test_telem_glv_command_OBCResponseData()
      yield obc_pb2.test_telem_glv_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.test_telem_glv_command_OBCResponseData(**{"epoch": resp.data.fields["epoch"]}) if resp.date_time.is_immediate else obc_pb2.test_telem_glv_command_OBCResponseData()
    yield obc_pb2.test_telem_glv_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)