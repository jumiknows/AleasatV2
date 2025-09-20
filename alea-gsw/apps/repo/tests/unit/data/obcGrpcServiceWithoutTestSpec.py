import alea.obc_grpc.generated.obc_pb2 as obc_pb2
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
  async def reset(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "RESET", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    return obc_pb2.reset_command_OBCResponse()
  async def ping(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "PING", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.ping_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.ping_command_OBCResponseData()
    return obc_pb2.ping_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def get_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_TIME", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_time_command_OBCResponseData(**{"timestamp": toProtobufTimestamp(resp.data.fields["timestamp"].date_time)}) if resp.date_time.is_immediate else obc_pb2.get_time_command_OBCResponseData()
    return obc_pb2.get_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def set_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "SET_TIME", alea_time.ALEADateTime(request.timestamp.ToDatetime()), date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.set_time_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.set_time_command_OBCResponseData()
    return obc_pb2.set_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def get_min_stack_space(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_MIN_STACK_SPACE", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_min_stack_space_command_OBCResponseData(**{"min_stack_space": bytes(resp.data.fields["min_stack_space"])}) if resp.date_time.is_immediate else obc_pb2.get_min_stack_space_command_OBCResponseData()
    return obc_pb2.get_min_stack_space_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def capture_rtos_trace(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "CAPTURE_RTOS_TRACE", request.length, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.capture_rtos_trace_command_OBCResponseData(**{"trace": bytes(resp.data.fields["trace"])}) if resp.date_time.is_immediate else obc_pb2.capture_rtos_trace_command_OBCResponseData()
    return obc_pb2.capture_rtos_trace_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def rtos_state(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "RTOS_STATE", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.rtos_state_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.rtos_state_command_OBCResponseData()
    return obc_pb2.rtos_state_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def low_power(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "LOW_POWER", request.enable, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.low_power_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.low_power_command_OBCResponseData()
    return obc_pb2.low_power_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def sys_time(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "SYS_TIME", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.sys_time_command_OBCResponseData(**{"sys_time_us": resp.data.fields["sys_time_us"]}) if resp.date_time.is_immediate else obc_pb2.sys_time_command_OBCResponseData()
    return obc_pb2.sys_time_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def cpu_usage(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "CPU_USAGE", request.task, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.cpu_usage_command_OBCResponseData(**{"usage_pct": resp.data.fields["usage_pct"]}) if resp.date_time.is_immediate else obc_pb2.cpu_usage_command_OBCResponseData()
    return obc_pb2.cpu_usage_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def task_runtime(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "TASK_RUNTIME", request.task, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.task_runtime_command_OBCResponseData(**{"runtime": resp.data.fields["runtime"]}) if resp.date_time.is_immediate else obc_pb2.task_runtime_command_OBCResponseData()
    return obc_pb2.task_runtime_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gpio_exp_reset(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPIO_EXP_RESET", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gpio_exp_reset_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gpio_exp_reset_command_OBCResponseData()
    return obc_pb2.gpio_exp_reset_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def i2c_reset(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "I2C_RESET", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.i2c_reset_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.i2c_reset_command_OBCResponseData()
    return obc_pb2.i2c_reset_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def heartbeat(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "HEARTBEAT", request.enable, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.heartbeat_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.heartbeat_command_OBCResponseData()
    return obc_pb2.heartbeat_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_restart(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_RESTART", request.start_mode, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_restart_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_restart_command_OBCResponseData()
    return obc_pb2.gps_restart_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def fw_info(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "FW_INFO", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.fw_info_command_OBCResponseData(**{"fw_version": resp.data.fields["fw_version"], "fw_hash": resp.data.fields["fw_hash"], "flash_address": resp.data.fields["flash_address"], "platform": resp.data.fields["platform"], "target": resp.data.fields["target"], "flags": resp.data.fields["flags"], "size": resp.data.fields["size"], "crc32": resp.data.fields["crc32"]}) if resp.date_time.is_immediate else obc_pb2.fw_info_command_OBCResponseData()
    return obc_pb2.fw_info_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def wmm_init(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "WMM_INIT", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.wmm_init_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.wmm_init_command_OBCResponseData()
    return obc_pb2.wmm_init_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def wmm_is_init(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "WMM_IS_INIT", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.wmm_is_init_command_OBCResponseData(**{"is_init": resp.data.fields["is_init"]}) if resp.date_time.is_immediate else obc_pb2.wmm_is_init_command_OBCResponseData()
    return obc_pb2.wmm_is_init_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def wmm_get_mag_ref(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "WMM_GET_MAG_REF", request.lon, request.lat, request.alt, request.year, request.month, request.day, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.wmm_get_mag_ref_command_OBCResponseData(**{"mag_ref": resp.data.fields["mag_ref"]}) if resp.date_time.is_immediate else obc_pb2.wmm_get_mag_ref_command_OBCResponseData()
    return obc_pb2.wmm_get_mag_ref_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_query_sw_ver(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_SW_VER", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_sw_ver_command_OBCResponseData(**{"gps_software_version": resp.data.fields["gps_software_version"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_sw_ver_command_OBCResponseData()
    return obc_pb2.gps_query_sw_ver_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_set_factory_settings(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_SET_FACTORY_SETTINGS", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_set_factory_settings_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_set_factory_settings_command_OBCResponseData()
    return obc_pb2.gps_set_factory_settings_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_configure_power_mode(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_POWER_MODE", request.write_settings, request.enable_power_save, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_power_mode_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_power_mode_command_OBCResponseData()
    return obc_pb2.gps_configure_power_mode_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_query_power_mode(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_POWER_MODE", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_power_mode_command_OBCResponseData(**{"gps_in_power_save_mode": resp.data.fields["gps_in_power_save_mode"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_power_mode_command_OBCResponseData()
    return obc_pb2.gps_query_power_mode_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_query_sw_crc(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_SW_CRC", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_sw_crc_command_OBCResponseData(**{"software_crc": resp.data.fields["software_crc"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_sw_crc_command_OBCResponseData()
    return obc_pb2.gps_query_sw_crc_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_configure_position_upate_rate(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_POSITION_UPATE_RATE", request.write_settings, request.position_update_rate, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_position_upate_rate_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_position_upate_rate_command_OBCResponseData()
    return obc_pb2.gps_configure_position_upate_rate_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_query_position_update_rate(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_POSITION_UPDATE_RATE", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_position_update_rate_command_OBCResponseData(**{"update_rate": resp.data.fields["update_rate"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_position_update_rate_command_OBCResponseData()
    return obc_pb2.gps_query_position_update_rate_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_configure_nmea_messges(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_NMEA_MESSGES", request.write_settings, request.gga_interval, request.gsa_interval, request.gsv_interval, request.gll_interval, request.rmc_interval, request.vtg_interval, request.zda_interval, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_nmea_messges_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_nmea_messges_command_OBCResponseData()
    return obc_pb2.gps_configure_nmea_messges_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_query_nmea_messges(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_QUERY_NMEA_MESSGES", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_query_nmea_messges_command_OBCResponseData(**{"gga_interval": resp.data.fields["gga_interval"], "gsa_interval": resp.data.fields["gsa_interval"], "gsv_interval": resp.data.fields["gsv_interval"], "gll_interval": resp.data.fields["gll_interval"], "rmc_interval": resp.data.fields["rmc_interval"], "vtg_interval": resp.data.fields["vtg_interval"], "zda_interval": resp.data.fields["zda_interval"]}) if resp.date_time.is_immediate else obc_pb2.gps_query_nmea_messges_command_OBCResponseData()
    return obc_pb2.gps_query_nmea_messges_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_configure_serial_port(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_SERIAL_PORT", request.baud_rate, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_serial_port_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_serial_port_command_OBCResponseData()
    return obc_pb2.gps_configure_serial_port_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def gps_configure_nav_msg_interval(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GPS_CONFIGURE_NAV_MSG_INTERVAL", request.write_settings, request.navigation_msg_int, date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.gps_configure_nav_msg_interval_command_OBCResponseData(**{}) if resp.date_time.is_immediate else obc_pb2.gps_configure_nav_msg_interval_command_OBCResponseData()
    return obc_pb2.gps_configure_nav_msg_interval_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)
  async def get_uptime(self, request, context):
    resp = await asyncio.get_event_loop().run_in_executor(_executor, functools.partial(obc.send_obc_cmd, "GET_UPTIME", date_time=alea_time.ALEADateTime(request.date_time.ToDatetime()), timeout=request.timeout))
    if (isinstance(resp, OBCPendingResponse)): resp = resp.sched_resp
    data_header = obc_pb2.OBCResponseDataHeader(code=resp.data_header.code, exec_datetime=toProtobufTimestamp(resp.data_header.exec_datetime.date_time))
    data = obc_pb2.get_uptime_command_OBCResponseData(**{"seconds": resp.data.fields["seconds"]}) if resp.date_time.is_immediate else obc_pb2.get_uptime_command_OBCResponseData()
    return obc_pb2.get_uptime_command_OBCResponse(cmd_inst_id=resp.cmd_inst_id, cmd_id=resp.cmd_id, date_time=toProtobufTimestamp(resp.date_time.date_time), flags=resp.flags, data_header=data_header, data=data)

