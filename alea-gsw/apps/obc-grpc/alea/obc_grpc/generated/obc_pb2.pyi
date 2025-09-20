from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class log_args(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class log_response(_message.Message):
    __slots__ = ("date_time", "log_id", "signal_id", "payload_len", "group_name", "group_desc", "signal_level", "signal_name", "signal_desc", "data")
    class OBCLogLevel(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        DEBUG: _ClassVar[log_response.OBCLogLevel]
        INFO: _ClassVar[log_response.OBCLogLevel]
        WARNING: _ClassVar[log_response.OBCLogLevel]
        ERROR: _ClassVar[log_response.OBCLogLevel]
    DEBUG: log_response.OBCLogLevel
    INFO: log_response.OBCLogLevel
    WARNING: log_response.OBCLogLevel
    ERROR: log_response.OBCLogLevel
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    LOG_ID_FIELD_NUMBER: _ClassVar[int]
    SIGNAL_ID_FIELD_NUMBER: _ClassVar[int]
    PAYLOAD_LEN_FIELD_NUMBER: _ClassVar[int]
    GROUP_NAME_FIELD_NUMBER: _ClassVar[int]
    GROUP_DESC_FIELD_NUMBER: _ClassVar[int]
    SIGNAL_LEVEL_FIELD_NUMBER: _ClassVar[int]
    SIGNAL_NAME_FIELD_NUMBER: _ClassVar[int]
    SIGNAL_DESC_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    date_time: _timestamp_pb2.Timestamp
    log_id: int
    signal_id: int
    payload_len: int
    group_name: str
    group_desc: str
    signal_level: log_response.OBCLogLevel
    signal_name: str
    signal_desc: str
    data: str
    def __init__(self, date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., log_id: _Optional[int] = ..., signal_id: _Optional[int] = ..., payload_len: _Optional[int] = ..., group_name: _Optional[str] = ..., group_desc: _Optional[str] = ..., signal_level: _Optional[_Union[log_response.OBCLogLevel, str]] = ..., signal_name: _Optional[str] = ..., signal_desc: _Optional[str] = ..., data: _Optional[str] = ...) -> None: ...

class reset_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class reset_command_OBCResponse(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class ping_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class ping_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: ping_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[ping_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class ping_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class get_time_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class get_time_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: get_time_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[get_time_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class get_time_command_OBCResponseData(_message.Message):
    __slots__ = ("timestamp",)
    TIMESTAMP_FIELD_NUMBER: _ClassVar[int]
    timestamp: _timestamp_pb2.Timestamp
    def __init__(self, timestamp: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class set_time_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "timestamp")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    TIMESTAMP_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    timestamp: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., timestamp: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class set_time_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: set_time_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[set_time_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class set_time_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class get_min_stack_space_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class get_min_stack_space_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: get_min_stack_space_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[get_min_stack_space_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class get_min_stack_space_command_OBCResponseData(_message.Message):
    __slots__ = ("min_stack_space",)
    MIN_STACK_SPACE_FIELD_NUMBER: _ClassVar[int]
    min_stack_space: bytes
    def __init__(self, min_stack_space: _Optional[bytes] = ...) -> None: ...

class capture_rtos_trace_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "length")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    LENGTH_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    length: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., length: _Optional[int] = ...) -> None: ...

class capture_rtos_trace_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: capture_rtos_trace_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[capture_rtos_trace_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class capture_rtos_trace_command_OBCResponseData(_message.Message):
    __slots__ = ("trace",)
    TRACE_FIELD_NUMBER: _ClassVar[int]
    trace: bytes
    def __init__(self, trace: _Optional[bytes] = ...) -> None: ...

class rtos_state_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class rtos_state_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: rtos_state_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[rtos_state_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class rtos_state_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class low_power_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "enable")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    ENABLE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    enable: bool
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., enable: bool = ...) -> None: ...

class low_power_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: low_power_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[low_power_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class low_power_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class sys_time_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class sys_time_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: sys_time_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[sys_time_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class sys_time_command_OBCResponseData(_message.Message):
    __slots__ = ("sys_time_us",)
    SYS_TIME_US_FIELD_NUMBER: _ClassVar[int]
    sys_time_us: int
    def __init__(self, sys_time_us: _Optional[int] = ...) -> None: ...

class cpu_usage_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "task")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    TASK_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    task: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., task: _Optional[int] = ...) -> None: ...

class cpu_usage_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: cpu_usage_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[cpu_usage_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class cpu_usage_command_OBCResponseData(_message.Message):
    __slots__ = ("usage_pct",)
    USAGE_PCT_FIELD_NUMBER: _ClassVar[int]
    usage_pct: int
    def __init__(self, usage_pct: _Optional[int] = ...) -> None: ...

class task_runtime_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "task")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    TASK_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    task: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., task: _Optional[int] = ...) -> None: ...

class task_runtime_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: task_runtime_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[task_runtime_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class task_runtime_command_OBCResponseData(_message.Message):
    __slots__ = ("runtime",)
    RUNTIME_FIELD_NUMBER: _ClassVar[int]
    runtime: int
    def __init__(self, runtime: _Optional[int] = ...) -> None: ...

class gpio_exp_reset_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class gpio_exp_reset_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gpio_exp_reset_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gpio_exp_reset_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gpio_exp_reset_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class i2c_reset_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class i2c_reset_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: i2c_reset_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[i2c_reset_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class i2c_reset_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class heartbeat_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "enable")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    ENABLE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    enable: bool
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., enable: bool = ...) -> None: ...

class heartbeat_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: heartbeat_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[heartbeat_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class heartbeat_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class gps_restart_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "start_mode")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    START_MODE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    start_mode: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., start_mode: _Optional[int] = ...) -> None: ...

class gps_restart_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_restart_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_restart_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_restart_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class fw_info_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class fw_info_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: fw_info_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[fw_info_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class fw_info_command_OBCResponseData(_message.Message):
    __slots__ = ("fw_version", "fw_hash", "flash_address", "platform", "target", "flags", "size", "crc32")
    FW_VERSION_FIELD_NUMBER: _ClassVar[int]
    FW_HASH_FIELD_NUMBER: _ClassVar[int]
    FLASH_ADDRESS_FIELD_NUMBER: _ClassVar[int]
    PLATFORM_FIELD_NUMBER: _ClassVar[int]
    TARGET_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    SIZE_FIELD_NUMBER: _ClassVar[int]
    CRC32_FIELD_NUMBER: _ClassVar[int]
    fw_version: _containers.RepeatedScalarFieldContainer[int]
    fw_hash: int
    flash_address: int
    platform: int
    target: int
    flags: int
    size: int
    crc32: int
    def __init__(self, fw_version: _Optional[_Iterable[int]] = ..., fw_hash: _Optional[int] = ..., flash_address: _Optional[int] = ..., platform: _Optional[int] = ..., target: _Optional[int] = ..., flags: _Optional[int] = ..., size: _Optional[int] = ..., crc32: _Optional[int] = ...) -> None: ...

class wmm_init_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class wmm_init_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: wmm_init_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[wmm_init_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class wmm_init_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class wmm_is_init_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class wmm_is_init_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: wmm_is_init_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[wmm_is_init_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class wmm_is_init_command_OBCResponseData(_message.Message):
    __slots__ = ("is_init",)
    IS_INIT_FIELD_NUMBER: _ClassVar[int]
    is_init: bool
    def __init__(self, is_init: bool = ...) -> None: ...

class wmm_get_mag_ref_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "lon", "lat", "alt", "year", "month", "day")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    LON_FIELD_NUMBER: _ClassVar[int]
    LAT_FIELD_NUMBER: _ClassVar[int]
    ALT_FIELD_NUMBER: _ClassVar[int]
    YEAR_FIELD_NUMBER: _ClassVar[int]
    MONTH_FIELD_NUMBER: _ClassVar[int]
    DAY_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    lon: float
    lat: float
    alt: float
    year: int
    month: int
    day: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., lon: _Optional[float] = ..., lat: _Optional[float] = ..., alt: _Optional[float] = ..., year: _Optional[int] = ..., month: _Optional[int] = ..., day: _Optional[int] = ...) -> None: ...

class wmm_get_mag_ref_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: wmm_get_mag_ref_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[wmm_get_mag_ref_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class wmm_get_mag_ref_command_OBCResponseData(_message.Message):
    __slots__ = ("mag_ref",)
    MAG_REF_FIELD_NUMBER: _ClassVar[int]
    mag_ref: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, mag_ref: _Optional[_Iterable[float]] = ...) -> None: ...

class gps_query_sw_ver_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class gps_query_sw_ver_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_query_sw_ver_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_query_sw_ver_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_query_sw_ver_command_OBCResponseData(_message.Message):
    __slots__ = ("gps_software_version",)
    GPS_SOFTWARE_VERSION_FIELD_NUMBER: _ClassVar[int]
    gps_software_version: _containers.RepeatedScalarFieldContainer[int]
    def __init__(self, gps_software_version: _Optional[_Iterable[int]] = ...) -> None: ...

class gps_set_factory_settings_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class gps_set_factory_settings_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_set_factory_settings_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_set_factory_settings_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_set_factory_settings_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class gps_configure_power_mode_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "write_settings", "enable_power_save")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    WRITE_SETTINGS_FIELD_NUMBER: _ClassVar[int]
    ENABLE_POWER_SAVE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    write_settings: int
    enable_power_save: bool
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., write_settings: _Optional[int] = ..., enable_power_save: bool = ...) -> None: ...

class gps_configure_power_mode_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_configure_power_mode_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_configure_power_mode_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_configure_power_mode_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class gps_query_power_mode_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class gps_query_power_mode_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_query_power_mode_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_query_power_mode_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_query_power_mode_command_OBCResponseData(_message.Message):
    __slots__ = ("gps_in_power_save_mode",)
    GPS_IN_POWER_SAVE_MODE_FIELD_NUMBER: _ClassVar[int]
    gps_in_power_save_mode: bool
    def __init__(self, gps_in_power_save_mode: bool = ...) -> None: ...

class gps_query_sw_crc_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class gps_query_sw_crc_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_query_sw_crc_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_query_sw_crc_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_query_sw_crc_command_OBCResponseData(_message.Message):
    __slots__ = ("software_crc",)
    SOFTWARE_CRC_FIELD_NUMBER: _ClassVar[int]
    software_crc: int
    def __init__(self, software_crc: _Optional[int] = ...) -> None: ...

class gps_configure_position_upate_rate_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "write_settings", "position_update_rate")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    WRITE_SETTINGS_FIELD_NUMBER: _ClassVar[int]
    POSITION_UPDATE_RATE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    write_settings: int
    position_update_rate: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., write_settings: _Optional[int] = ..., position_update_rate: _Optional[int] = ...) -> None: ...

class gps_configure_position_upate_rate_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_configure_position_upate_rate_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_configure_position_upate_rate_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_configure_position_upate_rate_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class gps_query_position_update_rate_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class gps_query_position_update_rate_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_query_position_update_rate_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_query_position_update_rate_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_query_position_update_rate_command_OBCResponseData(_message.Message):
    __slots__ = ("update_rate",)
    UPDATE_RATE_FIELD_NUMBER: _ClassVar[int]
    update_rate: int
    def __init__(self, update_rate: _Optional[int] = ...) -> None: ...

class gps_configure_nmea_messges_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "write_settings", "gga_interval", "gsa_interval", "gsv_interval", "gll_interval", "rmc_interval", "vtg_interval", "zda_interval")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    WRITE_SETTINGS_FIELD_NUMBER: _ClassVar[int]
    GGA_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    GSA_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    GSV_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    GLL_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    RMC_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    VTG_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    ZDA_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    write_settings: int
    gga_interval: int
    gsa_interval: int
    gsv_interval: int
    gll_interval: int
    rmc_interval: int
    vtg_interval: int
    zda_interval: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., write_settings: _Optional[int] = ..., gga_interval: _Optional[int] = ..., gsa_interval: _Optional[int] = ..., gsv_interval: _Optional[int] = ..., gll_interval: _Optional[int] = ..., rmc_interval: _Optional[int] = ..., vtg_interval: _Optional[int] = ..., zda_interval: _Optional[int] = ...) -> None: ...

class gps_configure_nmea_messges_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_configure_nmea_messges_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_configure_nmea_messges_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_configure_nmea_messges_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class gps_query_nmea_messges_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class gps_query_nmea_messges_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_query_nmea_messges_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_query_nmea_messges_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_query_nmea_messges_command_OBCResponseData(_message.Message):
    __slots__ = ("gga_interval", "gsa_interval", "gsv_interval", "gll_interval", "rmc_interval", "vtg_interval", "zda_interval")
    GGA_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    GSA_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    GSV_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    GLL_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    RMC_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    VTG_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    ZDA_INTERVAL_FIELD_NUMBER: _ClassVar[int]
    gga_interval: int
    gsa_interval: int
    gsv_interval: int
    gll_interval: int
    rmc_interval: int
    vtg_interval: int
    zda_interval: int
    def __init__(self, gga_interval: _Optional[int] = ..., gsa_interval: _Optional[int] = ..., gsv_interval: _Optional[int] = ..., gll_interval: _Optional[int] = ..., rmc_interval: _Optional[int] = ..., vtg_interval: _Optional[int] = ..., zda_interval: _Optional[int] = ...) -> None: ...

class gps_configure_serial_port_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "baud_rate")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    BAUD_RATE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    baud_rate: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., baud_rate: _Optional[int] = ...) -> None: ...

class gps_configure_serial_port_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_configure_serial_port_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_configure_serial_port_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_configure_serial_port_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class gps_configure_nav_msg_interval_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "write_settings", "navigation_msg_int")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    WRITE_SETTINGS_FIELD_NUMBER: _ClassVar[int]
    NAVIGATION_MSG_INT_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    write_settings: int
    navigation_msg_int: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., write_settings: _Optional[int] = ..., navigation_msg_int: _Optional[int] = ...) -> None: ...

class gps_configure_nav_msg_interval_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: gps_configure_nav_msg_interval_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[gps_configure_nav_msg_interval_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class gps_configure_nav_msg_interval_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class get_uptime_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class get_uptime_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: get_uptime_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[get_uptime_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class get_uptime_command_OBCResponseData(_message.Message):
    __slots__ = ("seconds",)
    SECONDS_FIELD_NUMBER: _ClassVar[int]
    seconds: int
    def __init__(self, seconds: _Optional[int] = ...) -> None: ...

class flash_sleep_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "enable")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    ENABLE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    enable: bool
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., enable: bool = ...) -> None: ...

class flash_sleep_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: flash_sleep_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[flash_sleep_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class flash_sleep_command_OBCResponseData(_message.Message):
    __slots__ = ("fs_err",)
    FS_ERR_FIELD_NUMBER: _ClassVar[int]
    fs_err: int
    def __init__(self, fs_err: _Optional[int] = ...) -> None: ...

class get_telemetry_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "priority", "size")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    PRIORITY_FIELD_NUMBER: _ClassVar[int]
    SIZE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    priority: int
    size: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., priority: _Optional[int] = ..., size: _Optional[int] = ...) -> None: ...

class get_telemetry_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: get_telemetry_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[get_telemetry_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class get_telemetry_command_OBCResponseData(_message.Message):
    __slots__ = ("data",)
    DATA_FIELD_NUMBER: _ClassVar[int]
    data: bytes
    def __init__(self, data: _Optional[bytes] = ...) -> None: ...

class test_echo_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "number", "array", "arrayf", "message")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    NUMBER_FIELD_NUMBER: _ClassVar[int]
    ARRAY_FIELD_NUMBER: _ClassVar[int]
    ARRAYF_FIELD_NUMBER: _ClassVar[int]
    MESSAGE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    number: int
    array: uint32_matrix
    arrayf: double_matrix
    message: str
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., number: _Optional[int] = ..., array: _Optional[_Union[uint32_matrix, _Mapping]] = ..., arrayf: _Optional[_Union[double_matrix, _Mapping]] = ..., message: _Optional[str] = ...) -> None: ...

class test_echo_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_echo_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_echo_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_echo_command_OBCResponseData(_message.Message):
    __slots__ = ("number", "array", "arrayf", "message")
    NUMBER_FIELD_NUMBER: _ClassVar[int]
    ARRAY_FIELD_NUMBER: _ClassVar[int]
    ARRAYF_FIELD_NUMBER: _ClassVar[int]
    MESSAGE_FIELD_NUMBER: _ClassVar[int]
    number: int
    array: uint32_matrix
    arrayf: double_matrix
    message: str
    def __init__(self, number: _Optional[int] = ..., array: _Optional[_Union[uint32_matrix, _Mapping]] = ..., arrayf: _Optional[_Union[double_matrix, _Mapping]] = ..., message: _Optional[str] = ...) -> None: ...

class test_hang_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "duration_us")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    DURATION_US_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    duration_us: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., duration_us: _Optional[int] = ...) -> None: ...

class test_hang_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_hang_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_hang_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_hang_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class test_comms_tx_rx_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_comms_tx_rx_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_comms_tx_rx_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_comms_tx_rx_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_comms_tx_rx_command_OBCResponseData(_message.Message):
    __slots__ = ("comms_err", "command")
    COMMS_ERR_FIELD_NUMBER: _ClassVar[int]
    COMMAND_FIELD_NUMBER: _ClassVar[int]
    comms_err: int
    command: int
    def __init__(self, comms_err: _Optional[int] = ..., command: _Optional[int] = ...) -> None: ...

class test_comms_stress1_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_comms_stress1_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_comms_stress1_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_comms_stress1_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_comms_stress1_command_OBCResponseData(_message.Message):
    __slots__ = ("fail",)
    PASS_FIELD_NUMBER: _ClassVar[int]
    FAIL_FIELD_NUMBER: _ClassVar[int]
    fail: int
    def __init__(self, fail: _Optional[int] = ..., **kwargs) -> None: ...

class test_comms_flash_app_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_comms_flash_app_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_comms_flash_app_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_comms_flash_app_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_comms_flash_app_command_OBCResponseData(_message.Message):
    __slots__ = ("comms_err",)
    COMMS_ERR_FIELD_NUMBER: _ClassVar[int]
    comms_err: int
    def __init__(self, comms_err: _Optional[int] = ...) -> None: ...

class test_comms_reboot_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_comms_reboot_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_comms_reboot_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_comms_reboot_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_comms_reboot_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class test_comms_get_telem_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_comms_get_telem_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_comms_get_telem_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_comms_get_telem_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_comms_get_telem_command_OBCResponseData(_message.Message):
    __slots__ = ("uptime", "uart0_rx_count", "uart1_rx_count", "rx_mode", "tx_mode", "adc", "rssi", "last_lqi", "last_frequest", "packets_sent", "cs_count", "packets_good", "packets_rejected_checksum", "packets_rejected_other", "custom0", "custom1")
    UPTIME_FIELD_NUMBER: _ClassVar[int]
    UART0_RX_COUNT_FIELD_NUMBER: _ClassVar[int]
    UART1_RX_COUNT_FIELD_NUMBER: _ClassVar[int]
    RX_MODE_FIELD_NUMBER: _ClassVar[int]
    TX_MODE_FIELD_NUMBER: _ClassVar[int]
    ADC_FIELD_NUMBER: _ClassVar[int]
    RSSI_FIELD_NUMBER: _ClassVar[int]
    LAST_LQI_FIELD_NUMBER: _ClassVar[int]
    LAST_FREQUEST_FIELD_NUMBER: _ClassVar[int]
    PACKETS_SENT_FIELD_NUMBER: _ClassVar[int]
    CS_COUNT_FIELD_NUMBER: _ClassVar[int]
    PACKETS_GOOD_FIELD_NUMBER: _ClassVar[int]
    PACKETS_REJECTED_CHECKSUM_FIELD_NUMBER: _ClassVar[int]
    PACKETS_REJECTED_OTHER_FIELD_NUMBER: _ClassVar[int]
    CUSTOM0_FIELD_NUMBER: _ClassVar[int]
    CUSTOM1_FIELD_NUMBER: _ClassVar[int]
    uptime: int
    uart0_rx_count: int
    uart1_rx_count: int
    rx_mode: int
    tx_mode: int
    adc: _containers.RepeatedScalarFieldContainer[int]
    rssi: int
    last_lqi: int
    last_frequest: int
    packets_sent: int
    cs_count: int
    packets_good: int
    packets_rejected_checksum: int
    packets_rejected_other: int
    custom0: int
    custom1: int
    def __init__(self, uptime: _Optional[int] = ..., uart0_rx_count: _Optional[int] = ..., uart1_rx_count: _Optional[int] = ..., rx_mode: _Optional[int] = ..., tx_mode: _Optional[int] = ..., adc: _Optional[_Iterable[int]] = ..., rssi: _Optional[int] = ..., last_lqi: _Optional[int] = ..., last_frequest: _Optional[int] = ..., packets_sent: _Optional[int] = ..., cs_count: _Optional[int] = ..., packets_good: _Optional[int] = ..., packets_rejected_checksum: _Optional[int] = ..., packets_rejected_other: _Optional[int] = ..., custom0: _Optional[int] = ..., custom1: _Optional[int] = ...) -> None: ...

class test_eps_read_float_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "read_cmd")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    READ_CMD_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    read_cmd: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., read_cmd: _Optional[int] = ...) -> None: ...

class test_eps_read_float_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_eps_read_float_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_eps_read_float_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_eps_read_float_command_OBCResponseData(_message.Message):
    __slots__ = ("eps_err", "data")
    EPS_ERR_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    eps_err: int
    data: float
    def __init__(self, eps_err: _Optional[int] = ..., data: _Optional[float] = ...) -> None: ...

class test_eps_read_float_raw_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "read_cmd")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    READ_CMD_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    read_cmd: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., read_cmd: _Optional[int] = ...) -> None: ...

class test_eps_read_float_raw_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_eps_read_float_raw_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_eps_read_float_raw_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_eps_read_float_raw_command_OBCResponseData(_message.Message):
    __slots__ = ("eps_err", "data")
    EPS_ERR_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    eps_err: int
    data: int
    def __init__(self, eps_err: _Optional[int] = ..., data: _Optional[int] = ...) -> None: ...

class test_eps_read_int_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "read_cmd")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    READ_CMD_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    read_cmd: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., read_cmd: _Optional[int] = ...) -> None: ...

class test_eps_read_int_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_eps_read_int_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_eps_read_int_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_eps_read_int_command_OBCResponseData(_message.Message):
    __slots__ = ("eps_err", "data")
    EPS_ERR_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    eps_err: int
    data: int
    def __init__(self, eps_err: _Optional[int] = ..., data: _Optional[int] = ...) -> None: ...

class test_eps_write_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "write_cmd", "state")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    WRITE_CMD_FIELD_NUMBER: _ClassVar[int]
    STATE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    write_cmd: int
    state: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., write_cmd: _Optional[int] = ..., state: _Optional[int] = ...) -> None: ...

class test_eps_write_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_eps_write_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_eps_write_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_eps_write_command_OBCResponseData(_message.Message):
    __slots__ = ("eps_err",)
    EPS_ERR_FIELD_NUMBER: _ClassVar[int]
    eps_err: int
    def __init__(self, eps_err: _Optional[int] = ...) -> None: ...

class test_eps_read_sanity_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_eps_read_sanity_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_eps_read_sanity_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_eps_read_sanity_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_eps_read_sanity_command_OBCResponseData(_message.Message):
    __slots__ = ("success_counter",)
    SUCCESS_COUNTER_FIELD_NUMBER: _ClassVar[int]
    success_counter: int
    def __init__(self, success_counter: _Optional[int] = ...) -> None: ...

class test_eps_writeread_sanity_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_eps_writeread_sanity_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_eps_writeread_sanity_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_eps_writeread_sanity_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_eps_writeread_sanity_command_OBCResponseData(_message.Message):
    __slots__ = ("success_counter",)
    SUCCESS_COUNTER_FIELD_NUMBER: _ClassVar[int]
    success_counter: int
    def __init__(self, success_counter: _Optional[int] = ...) -> None: ...

class test_eps_measure_buses_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_eps_measure_buses_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_eps_measure_buses_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_eps_measure_buses_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_eps_measure_buses_command_OBCResponseData(_message.Message):
    __slots__ = ("eps_err", "batt_V", "batt_A", "bcr_V", "bcr_A", "bus_3v3_V", "bus_3v3_A", "bus_5v_V", "bus_5v_A", "lup_3v3_V", "lup_5v_V")
    EPS_ERR_FIELD_NUMBER: _ClassVar[int]
    BATT_V_FIELD_NUMBER: _ClassVar[int]
    BATT_A_FIELD_NUMBER: _ClassVar[int]
    BCR_V_FIELD_NUMBER: _ClassVar[int]
    BCR_A_FIELD_NUMBER: _ClassVar[int]
    BUS_3V3_V_FIELD_NUMBER: _ClassVar[int]
    BUS_3V3_A_FIELD_NUMBER: _ClassVar[int]
    BUS_5V_V_FIELD_NUMBER: _ClassVar[int]
    BUS_5V_A_FIELD_NUMBER: _ClassVar[int]
    LUP_3V3_V_FIELD_NUMBER: _ClassVar[int]
    LUP_5V_V_FIELD_NUMBER: _ClassVar[int]
    eps_err: int
    batt_V: float
    batt_A: float
    bcr_V: float
    bcr_A: float
    bus_3v3_V: float
    bus_3v3_A: float
    bus_5v_V: float
    bus_5v_A: float
    lup_3v3_V: float
    lup_5v_V: float
    def __init__(self, eps_err: _Optional[int] = ..., batt_V: _Optional[float] = ..., batt_A: _Optional[float] = ..., bcr_V: _Optional[float] = ..., bcr_A: _Optional[float] = ..., bus_3v3_V: _Optional[float] = ..., bus_3v3_A: _Optional[float] = ..., bus_5v_V: _Optional[float] = ..., bus_5v_A: _Optional[float] = ..., lup_3v3_V: _Optional[float] = ..., lup_5v_V: _Optional[float] = ...) -> None: ...

class test_eps_measure_temps_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_eps_measure_temps_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_eps_measure_temps_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_eps_measure_temps_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_eps_measure_temps_command_OBCResponseData(_message.Message):
    __slots__ = ("eps_err", "cell_temps", "mcu_temp")
    EPS_ERR_FIELD_NUMBER: _ClassVar[int]
    CELL_TEMPS_FIELD_NUMBER: _ClassVar[int]
    MCU_TEMP_FIELD_NUMBER: _ClassVar[int]
    eps_err: int
    cell_temps: _containers.RepeatedScalarFieldContainer[float]
    mcu_temp: float
    def __init__(self, eps_err: _Optional[int] = ..., cell_temps: _Optional[_Iterable[float]] = ..., mcu_temp: _Optional[float] = ...) -> None: ...

class test_eps_read_counters_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_eps_read_counters_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_eps_read_counters_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_eps_read_counters_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_eps_read_counters_command_OBCResponseData(_message.Message):
    __slots__ = ("eps_err", "uptime", "power_on_cycles", "under_voltage", "short_circuit", "over_temp")
    EPS_ERR_FIELD_NUMBER: _ClassVar[int]
    UPTIME_FIELD_NUMBER: _ClassVar[int]
    POWER_ON_CYCLES_FIELD_NUMBER: _ClassVar[int]
    UNDER_VOLTAGE_FIELD_NUMBER: _ClassVar[int]
    SHORT_CIRCUIT_FIELD_NUMBER: _ClassVar[int]
    OVER_TEMP_FIELD_NUMBER: _ClassVar[int]
    eps_err: int
    uptime: int
    power_on_cycles: int
    under_voltage: int
    short_circuit: int
    over_temp: int
    def __init__(self, eps_err: _Optional[int] = ..., uptime: _Optional[int] = ..., power_on_cycles: _Optional[int] = ..., under_voltage: _Optional[int] = ..., short_circuit: _Optional[int] = ..., over_temp: _Optional[int] = ...) -> None: ...

class test_eps_read_status_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_eps_read_status_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_eps_read_status_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_eps_read_status_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_eps_read_status_command_OBCResponseData(_message.Message):
    __slots__ = ("eps_err", "out_cond_1", "out_cond_2")
    EPS_ERR_FIELD_NUMBER: _ClassVar[int]
    OUT_COND_1_FIELD_NUMBER: _ClassVar[int]
    OUT_COND_2_FIELD_NUMBER: _ClassVar[int]
    eps_err: int
    out_cond_1: int
    out_cond_2: int
    def __init__(self, eps_err: _Optional[int] = ..., out_cond_1: _Optional[int] = ..., out_cond_2: _Optional[int] = ...) -> None: ...

class test_mag_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "magnetorquer")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    MAGNETORQUER_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    magnetorquer: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., magnetorquer: _Optional[int] = ...) -> None: ...

class test_mag_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_mag_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_mag_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_mag_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class test_mag_all_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_mag_all_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_mag_all_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_mag_all_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_mag_all_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class test_mag_init_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_mag_init_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_mag_init_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_mag_init_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_mag_init_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class test_imu_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_imu_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_imu_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_imu_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_imu_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class test_panel_gyro_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "gyro")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    GYRO_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    gyro: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., gyro: _Optional[int] = ...) -> None: ...

class test_panel_gyro_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_panel_gyro_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_panel_gyro_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_panel_gyro_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class test_can_gpio_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "port", "pin", "value")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    PORT_FIELD_NUMBER: _ClassVar[int]
    PIN_FIELD_NUMBER: _ClassVar[int]
    VALUE_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    port: int
    pin: int
    value: bool
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., port: _Optional[int] = ..., pin: _Optional[int] = ..., value: bool = ...) -> None: ...

class test_can_gpio_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_can_gpio_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_can_gpio_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_can_gpio_command_OBCResponseData(_message.Message):
    __slots__ = ("gpio_err",)
    GPIO_ERR_FIELD_NUMBER: _ClassVar[int]
    gpio_err: int
    def __init__(self, gpio_err: _Optional[int] = ...) -> None: ...

class test_flash_rw_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "addr", "len")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    ADDR_FIELD_NUMBER: _ClassVar[int]
    LEN_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    addr: int
    len: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., addr: _Optional[int] = ..., len: _Optional[int] = ...) -> None: ...

class test_flash_rw_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_flash_rw_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_flash_rw_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_flash_rw_command_OBCResponseData(_message.Message):
    __slots__ = ("erase_err", "write_err", "read_err", "data_match")
    ERASE_ERR_FIELD_NUMBER: _ClassVar[int]
    WRITE_ERR_FIELD_NUMBER: _ClassVar[int]
    READ_ERR_FIELD_NUMBER: _ClassVar[int]
    DATA_MATCH_FIELD_NUMBER: _ClassVar[int]
    erase_err: int
    write_err: int
    read_err: int
    data_match: bool
    def __init__(self, erase_err: _Optional[int] = ..., write_err: _Optional[int] = ..., read_err: _Optional[int] = ..., data_match: bool = ...) -> None: ...

class test_filesystem_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_filesystem_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_filesystem_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_filesystem_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_filesystem_command_OBCResponseData(_message.Message):
    __slots__ = ("fs_err",)
    FS_ERR_FIELD_NUMBER: _ClassVar[int]
    fs_err: int
    def __init__(self, fs_err: _Optional[int] = ...) -> None: ...

class test_cam_init_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_cam_init_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_cam_init_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_cam_init_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_cam_init_command_OBCResponseData(_message.Message):
    __slots__ = ("arducam_err",)
    ARDUCAM_ERR_FIELD_NUMBER: _ClassVar[int]
    arducam_err: int
    def __init__(self, arducam_err: _Optional[int] = ...) -> None: ...

class test_cam_capture_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_cam_capture_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_cam_capture_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_cam_capture_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_cam_capture_command_OBCResponseData(_message.Message):
    __slots__ = ("arducam_err", "image_size", "image_data")
    ARDUCAM_ERR_FIELD_NUMBER: _ClassVar[int]
    IMAGE_SIZE_FIELD_NUMBER: _ClassVar[int]
    IMAGE_DATA_FIELD_NUMBER: _ClassVar[int]
    arducam_err: int
    image_size: int
    image_data: bytes
    def __init__(self, arducam_err: _Optional[int] = ..., image_size: _Optional[int] = ..., image_data: _Optional[bytes] = ...) -> None: ...

class test_cam_wr_sreg_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "addr", "data")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    ADDR_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    addr: int
    data: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., addr: _Optional[int] = ..., data: _Optional[int] = ...) -> None: ...

class test_cam_wr_sreg_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_cam_wr_sreg_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_cam_wr_sreg_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_cam_wr_sreg_command_OBCResponseData(_message.Message):
    __slots__ = ("ov5642_err",)
    OV5642_ERR_FIELD_NUMBER: _ClassVar[int]
    ov5642_err: int
    def __init__(self, ov5642_err: _Optional[int] = ...) -> None: ...

class test_cam_rd_sreg_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "addr")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    ADDR_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    addr: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., addr: _Optional[int] = ...) -> None: ...

class test_cam_rd_sreg_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_cam_rd_sreg_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_cam_rd_sreg_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_cam_rd_sreg_command_OBCResponseData(_message.Message):
    __slots__ = ("ov5642_err", "data")
    OV5642_ERR_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    ov5642_err: int
    data: int
    def __init__(self, ov5642_err: _Optional[int] = ..., data: _Optional[int] = ...) -> None: ...

class test_cam_wr_sregs_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "reg_data")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    REG_DATA_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    reg_data: bytes
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., reg_data: _Optional[bytes] = ...) -> None: ...

class test_cam_wr_sregs_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_cam_wr_sregs_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_cam_wr_sregs_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_cam_wr_sregs_command_OBCResponseData(_message.Message):
    __slots__ = ("ov5642_err",)
    OV5642_ERR_FIELD_NUMBER: _ClassVar[int]
    ov5642_err: int
    def __init__(self, ov5642_err: _Optional[int] = ...) -> None: ...

class test_dsplib_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "x", "y")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    X_FIELD_NUMBER: _ClassVar[int]
    Y_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    x: float
    y: float
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., x: _Optional[float] = ..., y: _Optional[float] = ...) -> None: ...

class test_dsplib_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_dsplib_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_dsplib_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_dsplib_command_OBCResponseData(_message.Message):
    __slots__ = ("x_abs", "y_abs")
    X_ABS_FIELD_NUMBER: _ClassVar[int]
    Y_ABS_FIELD_NUMBER: _ClassVar[int]
    x_abs: float
    y_abs: float
    def __init__(self, x_abs: _Optional[float] = ..., y_abs: _Optional[float] = ...) -> None: ...

class test_quest_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "sun_obs", "sun_ref", "mag_obs", "mag_ref", "weights")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    SUN_OBS_FIELD_NUMBER: _ClassVar[int]
    SUN_REF_FIELD_NUMBER: _ClassVar[int]
    MAG_OBS_FIELD_NUMBER: _ClassVar[int]
    MAG_REF_FIELD_NUMBER: _ClassVar[int]
    WEIGHTS_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    sun_obs: _containers.RepeatedScalarFieldContainer[float]
    sun_ref: _containers.RepeatedScalarFieldContainer[float]
    mag_obs: _containers.RepeatedScalarFieldContainer[float]
    mag_ref: _containers.RepeatedScalarFieldContainer[float]
    weights: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., sun_obs: _Optional[_Iterable[float]] = ..., sun_ref: _Optional[_Iterable[float]] = ..., mag_obs: _Optional[_Iterable[float]] = ..., mag_ref: _Optional[_Iterable[float]] = ..., weights: _Optional[_Iterable[float]] = ...) -> None: ...

class test_quest_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_quest_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_quest_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_quest_command_OBCResponseData(_message.Message):
    __slots__ = ("quat", "adcs_err", "avg_duration")
    QUAT_FIELD_NUMBER: _ClassVar[int]
    ADCS_ERR_FIELD_NUMBER: _ClassVar[int]
    AVG_DURATION_FIELD_NUMBER: _ClassVar[int]
    quat: _containers.RepeatedScalarFieldContainer[float]
    adcs_err: int
    avg_duration: float
    def __init__(self, quat: _Optional[_Iterable[float]] = ..., adcs_err: _Optional[int] = ..., avg_duration: _Optional[float] = ...) -> None: ...

class test_rtc_get_time_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "duration_s")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    DURATION_S_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    duration_s: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., duration_s: _Optional[int] = ...) -> None: ...

class test_rtc_get_time_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_rtc_get_time_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_rtc_get_time_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_rtc_get_time_command_OBCResponseData(_message.Message):
    __slots__ = ("second", "minute", "hour", "day", "month", "year")
    SECOND_FIELD_NUMBER: _ClassVar[int]
    MINUTE_FIELD_NUMBER: _ClassVar[int]
    HOUR_FIELD_NUMBER: _ClassVar[int]
    DAY_FIELD_NUMBER: _ClassVar[int]
    MONTH_FIELD_NUMBER: _ClassVar[int]
    YEAR_FIELD_NUMBER: _ClassVar[int]
    second: int
    minute: int
    hour: int
    day: int
    month: int
    year: int
    def __init__(self, second: _Optional[int] = ..., minute: _Optional[int] = ..., hour: _Optional[int] = ..., day: _Optional[int] = ..., month: _Optional[int] = ..., year: _Optional[int] = ...) -> None: ...

class test_rtc_cap_and_get_timestamp_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "duration_s")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    DURATION_S_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    duration_s: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., duration_s: _Optional[int] = ...) -> None: ...

class test_rtc_cap_and_get_timestamp_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_rtc_cap_and_get_timestamp_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_rtc_cap_and_get_timestamp_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_rtc_cap_and_get_timestamp_command_OBCResponseData(_message.Message):
    __slots__ = ("init_second", "init_minute", "init_hour", "init_day", "init_month", "init_year", "ts_second", "ts_minute", "ts_hour", "ts_day", "ts_month", "ts_year")
    INIT_SECOND_FIELD_NUMBER: _ClassVar[int]
    INIT_MINUTE_FIELD_NUMBER: _ClassVar[int]
    INIT_HOUR_FIELD_NUMBER: _ClassVar[int]
    INIT_DAY_FIELD_NUMBER: _ClassVar[int]
    INIT_MONTH_FIELD_NUMBER: _ClassVar[int]
    INIT_YEAR_FIELD_NUMBER: _ClassVar[int]
    TS_SECOND_FIELD_NUMBER: _ClassVar[int]
    TS_MINUTE_FIELD_NUMBER: _ClassVar[int]
    TS_HOUR_FIELD_NUMBER: _ClassVar[int]
    TS_DAY_FIELD_NUMBER: _ClassVar[int]
    TS_MONTH_FIELD_NUMBER: _ClassVar[int]
    TS_YEAR_FIELD_NUMBER: _ClassVar[int]
    init_second: int
    init_minute: int
    init_hour: int
    init_day: int
    init_month: int
    init_year: int
    ts_second: int
    ts_minute: int
    ts_hour: int
    ts_day: int
    ts_month: int
    ts_year: int
    def __init__(self, init_second: _Optional[int] = ..., init_minute: _Optional[int] = ..., init_hour: _Optional[int] = ..., init_day: _Optional[int] = ..., init_month: _Optional[int] = ..., init_year: _Optional[int] = ..., ts_second: _Optional[int] = ..., ts_minute: _Optional[int] = ..., ts_hour: _Optional[int] = ..., ts_day: _Optional[int] = ..., ts_month: _Optional[int] = ..., ts_year: _Optional[int] = ...) -> None: ...

class debug_read_rtc_reg_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "reg")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    REG_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    reg: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., reg: _Optional[int] = ...) -> None: ...

class debug_read_rtc_reg_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: debug_read_rtc_reg_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[debug_read_rtc_reg_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class debug_read_rtc_reg_command_OBCResponseData(_message.Message):
    __slots__ = ("val",)
    VAL_FIELD_NUMBER: _ClassVar[int]
    val: int
    def __init__(self, val: _Optional[int] = ...) -> None: ...

class debug_write_rtc_reg_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "reg", "val")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    REG_FIELD_NUMBER: _ClassVar[int]
    VAL_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    reg: int
    val: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., reg: _Optional[int] = ..., val: _Optional[int] = ...) -> None: ...

class debug_write_rtc_reg_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: debug_write_rtc_reg_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[debug_write_rtc_reg_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class debug_write_rtc_reg_command_OBCResponseData(_message.Message):
    __slots__ = ("val_after_write",)
    VAL_AFTER_WRITE_FIELD_NUMBER: _ClassVar[int]
    val_after_write: int
    def __init__(self, val_after_write: _Optional[int] = ...) -> None: ...

class test_rtc_alarm_n_sec_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "duration_s")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    DURATION_S_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    duration_s: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., duration_s: _Optional[int] = ...) -> None: ...

class test_rtc_alarm_n_sec_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_rtc_alarm_n_sec_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_rtc_alarm_n_sec_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_rtc_alarm_n_sec_command_OBCResponseData(_message.Message):
    __slots__ = ()
    def __init__(self) -> None: ...

class test_adcs_sun_model_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    time: _containers.RepeatedScalarFieldContainer[int]
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., time: _Optional[_Iterable[int]] = ...) -> None: ...

class test_adcs_sun_model_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_adcs_sun_model_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_adcs_sun_model_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_adcs_sun_model_command_OBCResponseData(_message.Message):
    __slots__ = ("jul", "pos")
    JUL_FIELD_NUMBER: _ClassVar[int]
    POS_FIELD_NUMBER: _ClassVar[int]
    jul: float
    pos: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, jul: _Optional[float] = ..., pos: _Optional[_Iterable[float]] = ...) -> None: ...

class test_adc_voltage_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "channel")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    CHANNEL_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    channel: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., channel: _Optional[int] = ...) -> None: ...

class test_adc_voltage_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_adc_voltage_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_adc_voltage_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_adc_voltage_command_OBCResponseData(_message.Message):
    __slots__ = ("channel_voltage", "adc_err_status")
    CHANNEL_VOLTAGE_FIELD_NUMBER: _ClassVar[int]
    ADC_ERR_STATUS_FIELD_NUMBER: _ClassVar[int]
    channel_voltage: int
    adc_err_status: int
    def __init__(self, channel_voltage: _Optional[int] = ..., adc_err_status: _Optional[int] = ...) -> None: ...

class test_large_payload_split_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "data")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    data: bytes
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., data: _Optional[bytes] = ...) -> None: ...

class test_large_payload_split_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_large_payload_split_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_large_payload_split_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_large_payload_split_command_OBCResponseData(_message.Message):
    __slots__ = ("sum",)
    SUM_FIELD_NUMBER: _ClassVar[int]
    sum: int
    def __init__(self, sum: _Optional[int] = ...) -> None: ...

class test_mram_rw_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "addr", "len")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    ADDR_FIELD_NUMBER: _ClassVar[int]
    LEN_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    addr: int
    len: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., addr: _Optional[int] = ..., len: _Optional[int] = ...) -> None: ...

class test_mram_rw_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_mram_rw_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_mram_rw_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_mram_rw_command_OBCResponseData(_message.Message):
    __slots__ = ("erase_err", "write_err", "read_err", "data_match")
    ERASE_ERR_FIELD_NUMBER: _ClassVar[int]
    WRITE_ERR_FIELD_NUMBER: _ClassVar[int]
    READ_ERR_FIELD_NUMBER: _ClassVar[int]
    DATA_MATCH_FIELD_NUMBER: _ClassVar[int]
    erase_err: int
    write_err: int
    read_err: int
    data_match: bool
    def __init__(self, erase_err: _Optional[int] = ..., write_err: _Optional[int] = ..., read_err: _Optional[int] = ..., data_match: bool = ...) -> None: ...

class test_mram_write_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "addr", "write_data")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    ADDR_FIELD_NUMBER: _ClassVar[int]
    WRITE_DATA_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    addr: int
    write_data: bytes
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., addr: _Optional[int] = ..., write_data: _Optional[bytes] = ...) -> None: ...

class test_mram_write_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_mram_write_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_mram_write_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_mram_write_command_OBCResponseData(_message.Message):
    __slots__ = ("write_err",)
    WRITE_ERR_FIELD_NUMBER: _ClassVar[int]
    write_err: int
    def __init__(self, write_err: _Optional[int] = ...) -> None: ...

class test_mram_read_command_args(_message.Message):
    __slots__ = ("timeout", "date_time", "addr", "len")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    ADDR_FIELD_NUMBER: _ClassVar[int]
    LEN_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    addr: int
    len: int
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., addr: _Optional[int] = ..., len: _Optional[int] = ...) -> None: ...

class test_mram_read_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_mram_read_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_mram_read_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_mram_read_command_OBCResponseData(_message.Message):
    __slots__ = ("read_err", "data")
    READ_ERR_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    read_err: int
    data: bytes
    def __init__(self, read_err: _Optional[int] = ..., data: _Optional[bytes] = ...) -> None: ...

class test_telem_glv_command_args(_message.Message):
    __slots__ = ("timeout", "date_time")
    TIMEOUT_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    timeout: int
    date_time: _timestamp_pb2.Timestamp
    def __init__(self, timeout: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...

class test_telem_glv_command_OBCResponse(_message.Message):
    __slots__ = ("cmd_inst_id", "cmd_id", "date_time", "flags", "data_header", "data", "is_success", "is_success_sched")
    CMD_INST_ID_FIELD_NUMBER: _ClassVar[int]
    CMD_ID_FIELD_NUMBER: _ClassVar[int]
    DATE_TIME_FIELD_NUMBER: _ClassVar[int]
    FLAGS_FIELD_NUMBER: _ClassVar[int]
    DATA_HEADER_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_FIELD_NUMBER: _ClassVar[int]
    IS_SUCCESS_SCHED_FIELD_NUMBER: _ClassVar[int]
    cmd_inst_id: int
    cmd_id: int
    date_time: _timestamp_pb2.Timestamp
    flags: int
    data_header: OBCResponseDataHeader
    data: test_telem_glv_command_OBCResponseData
    is_success: bool
    is_success_sched: bool
    def __init__(self, cmd_inst_id: _Optional[int] = ..., cmd_id: _Optional[int] = ..., date_time: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ..., flags: _Optional[int] = ..., data_header: _Optional[_Union[OBCResponseDataHeader, _Mapping]] = ..., data: _Optional[_Union[test_telem_glv_command_OBCResponseData, _Mapping]] = ..., is_success: bool = ..., is_success_sched: bool = ...) -> None: ...

class test_telem_glv_command_OBCResponseData(_message.Message):
    __slots__ = ("epoch",)
    EPOCH_FIELD_NUMBER: _ClassVar[int]
    epoch: int
    def __init__(self, epoch: _Optional[int] = ...) -> None: ...

class uint32_matrix(_message.Message):
    __slots__ = ("rows", "cols", "data")
    ROWS_FIELD_NUMBER: _ClassVar[int]
    COLS_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    rows: int
    cols: int
    data: _containers.RepeatedScalarFieldContainer[int]
    def __init__(self, rows: _Optional[int] = ..., cols: _Optional[int] = ..., data: _Optional[_Iterable[int]] = ...) -> None: ...

class uint64_matrix(_message.Message):
    __slots__ = ("rows", "cols", "data")
    ROWS_FIELD_NUMBER: _ClassVar[int]
    COLS_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    rows: int
    cols: int
    data: _containers.RepeatedScalarFieldContainer[int]
    def __init__(self, rows: _Optional[int] = ..., cols: _Optional[int] = ..., data: _Optional[_Iterable[int]] = ...) -> None: ...

class sint32_matrix(_message.Message):
    __slots__ = ("rows", "cols", "data")
    ROWS_FIELD_NUMBER: _ClassVar[int]
    COLS_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    rows: int
    cols: int
    data: _containers.RepeatedScalarFieldContainer[int]
    def __init__(self, rows: _Optional[int] = ..., cols: _Optional[int] = ..., data: _Optional[_Iterable[int]] = ...) -> None: ...

class sint64_matrix(_message.Message):
    __slots__ = ("rows", "cols", "data")
    ROWS_FIELD_NUMBER: _ClassVar[int]
    COLS_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    rows: int
    cols: int
    data: _containers.RepeatedScalarFieldContainer[int]
    def __init__(self, rows: _Optional[int] = ..., cols: _Optional[int] = ..., data: _Optional[_Iterable[int]] = ...) -> None: ...

class double_matrix(_message.Message):
    __slots__ = ("rows", "cols", "data")
    ROWS_FIELD_NUMBER: _ClassVar[int]
    COLS_FIELD_NUMBER: _ClassVar[int]
    DATA_FIELD_NUMBER: _ClassVar[int]
    rows: int
    cols: int
    data: _containers.RepeatedScalarFieldContainer[float]
    def __init__(self, rows: _Optional[int] = ..., cols: _Optional[int] = ..., data: _Optional[_Iterable[float]] = ...) -> None: ...

class OBCResponseDataHeader(_message.Message):
    __slots__ = ("code", "exec_datetime")
    class Code(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        NONE: _ClassVar[OBCResponseDataHeader.Code]
        SUCCESS: _ClassVar[OBCResponseDataHeader.Code]
        SUCCESS_SCHED: _ClassVar[OBCResponseDataHeader.Code]
        ERROR: _ClassVar[OBCResponseDataHeader.Code]
        CMD_DNE: _ClassVar[OBCResponseDataHeader.Code]
        NOT_IMPl: _ClassVar[OBCResponseDataHeader.Code]
    NONE: OBCResponseDataHeader.Code
    SUCCESS: OBCResponseDataHeader.Code
    SUCCESS_SCHED: OBCResponseDataHeader.Code
    ERROR: OBCResponseDataHeader.Code
    CMD_DNE: OBCResponseDataHeader.Code
    NOT_IMPl: OBCResponseDataHeader.Code
    CODE_FIELD_NUMBER: _ClassVar[int]
    EXEC_DATETIME_FIELD_NUMBER: _ClassVar[int]
    code: OBCResponseDataHeader.Code
    exec_datetime: _timestamp_pb2.Timestamp
    def __init__(self, code: _Optional[_Union[OBCResponseDataHeader.Code, str]] = ..., exec_datetime: _Optional[_Union[_timestamp_pb2.Timestamp, _Mapping]] = ...) -> None: ...
