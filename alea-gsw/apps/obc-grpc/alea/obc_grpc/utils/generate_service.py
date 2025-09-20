import json
import re
from pathlib import Path


def read_json(file_path):
    with open(file_path, "r", encoding="utf-8") as f:
        return json.load(f)


def gen_argument_conversion(args):
    lines = []
    for field in args:
        field_name = list(field.keys())[0]
        type_str = field[field_name]
        if type_str == "datetime":
            lines.append(f", alea_time.ALEADateTime(request.{field_name}.ToDatetime())")
        elif "][" in type_str:
            lines.append(f", request.{field_name}.data")
        else:
            lines.append(f", request.{field_name}")
    return "".join(lines)


def gen_response_fields(resp):
    lines = []
    for field in resp:
        field_name = list(field.keys())[0]
        type_str = field[field_name]
        if type_str == "datetime":
            lines.append(
                f'"{field_name}": toProtobufTimestamp(resp.data.fields["{field_name}"].date_time)'
            )
        elif type_str == "bytes":
            lines.append(f'"{field_name}": bytes(resp.data.fields["{field_name}"])')
        elif "][" in type_str:
            dims = list(map(int, re.findall(r"\[([0-9]+)\]", type_str)))
            base_type = type_str.split("[")[0]
            lines.append(
                f'"{field_name}": toMatrix(resp.data.fields["{field_name}"], "{base_type}", {dims[0]}, {dims[1]})'
            )
        else:
            lines.append(f'"{field_name}": resp.data.fields["{field_name}"]')
    return ", ".join(lines)


def spec_to_python_rpc(spec):
    methods = []

    for command_name, details in spec.items():
        name_l = command_name.lower()
        args_conv = gen_argument_conversion(details["args"])

        # async def {cmd}
        method = f"""  async def {name_l}(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "{command_name}"{args_conv}, date_time=alea_time.IMMEDIATE, timeout=request.timeout))"""

        if details["resp"] is None:
            method += f"""
    return obc_pb2.{name_l}_command_OBCResponse()"""
        else:
            resp_fields = gen_response_fields(details["resp"])
            method += f"""
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.{name_l}_command_OBCResponseData(**{{{resp_fields}}}) if resp.date_time.is_immediate else obc_pb2.{name_l}_command_OBCResponseData()
    return obc_pb2.{name_l}_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)"""

        # async def schedule_{cmd}
        schedule = f"""
  async def schedule_{name_l}(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "{command_name}"{args_conv}, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))"""

        if details["resp"] is None:
            schedule += f"""
    yield obc_pb2.{name_l}_command_OBCResponse()"""
        else:
            resp_fields = gen_response_fields(details["resp"])
            schedule += f"""
    if isinstance(resp, OBCPendingResponse): 
      data_header = obc_pb2.OBCResponseDataHeader(code=resp.sched_resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.sched_resp.data_header.exec_datetime.date_time))
      data = obc_pb2.{name_l}_command_OBCResponseData()
      yield obc_pb2.{name_l}_command_OBCResponse(cmd_inst_id=resp.sched_resp.cmd_inst_id, cmd_id=resp.sched_resp.cmd_id, date_time=toProtobufTimestamp(resp.sched_resp.date_time.date_time), flags=resp.sched_resp.flags, data_header=data_header, data=data, is_success=resp.sched_resp.is_success, is_success_sched=resp.sched_resp.is_success_sched)
      while True:
        try:
          resp = resp._q.get_nowait()
          if isinstance(resp, Exception):
            raise resp
          break
        except queue.Empty:
          await asyncio.sleep(0.1)
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.{name_l}_command_OBCResponseData(**{{{resp_fields}}}) if resp.date_time.is_immediate else obc_pb2.{name_l}_command_OBCResponseData()
    yield obc_pb2.{name_l}_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data, is_success=resp.is_success, is_success_sched=resp.is_success_sched)"""

        methods.append(method + schedule)

    return "\n".join(methods)


def generate_service_file(cmd_sys_path, cmd_sys_test_path, output_path):
    cmd_sys_spec = read_json(cmd_sys_path)
    cmd_sys_test_spec = read_json(cmd_sys_test_path)

    header = """import alea.obc_grpc.generated.obc_pb2 as obc_pb2
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
"""

    methods = (
        spec_to_python_rpc(cmd_sys_spec) + "\n" + spec_to_python_rpc(cmd_sys_test_spec)
    )

    full_code = header + methods

    with open(output_path, "w", encoding="utf-8") as f:
        f.write(full_code)
