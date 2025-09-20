from typing import Any, ClassVar
from dataclasses import dataclass
from enum import IntEnum
import struct
import queue
import logging

from alea.common import alea_time

from alea.obcfw.cmd_sys import cmd_sys_spec

from alea.ttc.util import data_utils

logger = logging.getLogger(__name__)

class OBCCmd:
    """Represents a command that will be sent to the OBC.

    Attributes:
        spec: The command system specification for the command.
        inst_id: The unique instance ID of the command.
        args: The arguments to the command.
        date_time: When the command should be executed (either immediately or at the specified time).
    """

    def __init__(self, spec: cmd_sys_spec.OBCCmdSysSpec, inst_id: int, args: list[Any], date_time: alea_time.ALEADateTime):
        self._spec = spec
        self._inst_id = inst_id
        self._args = args
        self._date_time = date_time

    @property
    def spec(self) -> cmd_sys_spec.OBCCmdSysSpec:
        return self._spec

    @property
    def inst_id(self) -> int:
        return self._inst_id

    @property
    def args(self) -> list[Any]:
        return self._args

    @property
    def date_time(self) -> alea_time.ALEADateTime:
        return self._date_time

# This should match cmd_sys_resp_code_t in cmd_sys.h
class ResponseCode(IntEnum):
    SUCCESS       = 1
    SUCCESS_SCHED = 2
    ERROR         = 3
    CMD_DNE       = 4
    NOT_IMPl      = 5

class OBCCmdSysResponseError(Exception):
    pass

@dataclass(frozen=True)
class OBCResponseDataHeader:
    """Represents the header at the beginning of the data field of an OBCResponse

    Attributes:
        code (readonly): The response status code.
        exec_datetime (readonly): The date/time when the command was executed.
    """

    FMT: ClassVar[str] = f"!BI"
    HEADER_SIZE: ClassVar[int] = 5

    code: ResponseCode
    exec_datetime: alea_time.ALEADateTime

    def __str__(self) -> str:
        return f"code={self.code.name} exec_datetime={str(self.exec_datetime)}"

    @classmethod
    def deserialize(cls, data: bytes) -> "OBCResponseDataHeader":
        """Construct an OBCResponseDataHeader instance from a byte array.

        Args:
            data: A byte array containing the serialized OBCResponseDataHeader.

        Raises:
            OBCCmdSysResponseError: If the data cannot be parsed into a valid OBCResponseDataHeader.

        Returns:
            A new OBCResponseDataHeader instance.
        """
        try:
            header_fields = struct.unpack(cls.FMT, data)
        except struct.error as e:
            raise OBCCmdSysResponseError(f"Failed to decode response data header: {str(e)}")

        # Attempt to extract response code
        try:
            code = ResponseCode(header_fields[0])
        except ValueError:
            raise OBCCmdSysResponseError(f"Invalid response code ({header_fields[0]}).")

        # Extract exec_datetime
        exec_datetime = alea_time.ALEADateTime.from_timestamp(header_fields[1])

        return OBCResponseDataHeader(code, exec_datetime)

@dataclass(frozen=True)
class OBCResponseData:
    """Represents the data (after the data header) in an OBC response.

    Attributes:
        raw (readonly): The raw response data.
        fields (readonly): The response data decoded into individual fields.
        error (readonly): If an error occurred decoding the response data it should be set here.
    """
    raw: bytes
    fields: dict[str, Any]
    error: cmd_sys_spec.OBCCmdSysSpecDecodeError = None

    @property
    def has_fields(self) -> bool:
        """True if this response was expecting fields and successfully decoded the fields
        (even if the number of fields was zero).
        """
        return self.fields is not None

    def __contains__(self, key: str) -> bool:
        return self.has_fields and (key in self.fields)

    def __getitem__(self, key: str) -> Any:
        if not self.has_fields:
            raise OBCCmdSysResponseError(f"Response does not have fields")

        return self.fields[key]

    def __str__(self) -> str:
        data_str = ""
        if self.has_fields:
            data_str = " " + ", ".join(map(lambda name : f"{name}={str(self.fields[name])}", self.fields))
        else:
            data_str = " " + data_utils.bytes_to_hexstr(self.raw)

        error_str = ""
        if self.error is not None:
            error_str = f" | error={str(self.error)}"

        return f"{data_str}{error_str}"

@dataclass(frozen=True)
class OBCResponse:
    """Represents an actual received response to a command.

    If the response contained data that could be parsed into fields according to the cmd_sys_spec,
    then those fields are accessible via subscripting:

    ```python
    response = OBCResponse.create(...)
    print(response.data["field_name"])
    ```

    Attributes:
        cmd_inst_id (readonly): The instance ID of the command to which this is a response.
        cmd_id (readonly): The ID of the command to which this is a response.
        date_time (readonly): The command timestamp.
        flags (readonly): The command flags.
        data (readonly): The response data.
    """

    cmd_inst_id: int
    cmd_id: int
    date_time: alea_time.ALEADateTime
    flags: int
    data_header: OBCResponseDataHeader
    data: OBCResponseData

    @property
    def is_success(self) -> bool:
        return (self.data_header.code == ResponseCode.SUCCESS)

    @property
    def is_success_sched(self) -> bool:
        return (self.data_header.code == ResponseCode.SUCCESS_SCHED)

    def __str__(self) -> str:
        return f"[OBCResponse] instance={self.cmd_inst_id} {str(self.data_header)}{str(self.data)}"

    @classmethod
    def create(cls, spec: cmd_sys_spec.OBCCmdSysSpec, cmd_inst_id: int, date_time: alea_time.ALEADateTime, flags: int, resp_data: bytes, ignore_fields: bool = False) -> "OBCResponse":
        """Create an `OBCResponse` object from a spec, header and data.

        Args:
            spec:  The specification of the response to create.
            cmd_inst_id: The instance ID of the command to which this is a response.
            date_time (readonly): The command timestamp.
            flags (readonly): The command flags.
            resp_data: The full raw response data. Cannot be None.
            ignore_fields: Set to True if the usual fields are not expected in this response. Defaults to False.

        Raises:
            OBCCmdSysResponseError: If the resp_data does not contain a valid data header.

        Returns:
            An `OBCResponse` instance.
        """

        if len(resp_data) < OBCResponseDataHeader.HEADER_SIZE:
            raise OBCCmdSysResponseError(f"Response data missing data header for command instance: {cmd_inst_id}")

        data_header = OBCResponseDataHeader.deserialize(resp_data[:OBCResponseDataHeader.HEADER_SIZE])
        data_data = resp_data[OBCResponseDataHeader.HEADER_SIZE:]

        # Attempt to parse response fields
        fields: dict[str, Any] = None
        error: cmd_sys_spec.OBCCmdSysSpecDecodeError = None

        if ignore_fields:
            fields = {}
        else:
            try:
                fields = spec.decode_resp(data_data)
            except cmd_sys_spec.OBCCmdSysSpecDecodeError as e:
                logger.error(f"[OBCCmdSysSpecDecodeError] {str(e)}", exc_info=True)
                fields = None
                error = e

        return OBCResponse(cmd_inst_id, spec.id, date_time, flags, data_header, OBCResponseData(data_data, fields, error=error))

class OBCPendingResponse:
    """A response that is pending to a scheduled command.

    Attributes:
        sched_resp (readonly): The response to the command that scheduled this command.
    """

    def __init__(self, sched_resp: OBCResponse):
        self._sched_resp = sched_resp
        self._q = queue.Queue(maxsize=1)

    @property
    def sched_resp(self) -> OBCResponse:
        """The response to the command that scheduled this command.
        """
        return self._sched_resp

    def set_response(self, response: OBCResponse):
        """Sets the response (unblocking anyone who called `OBCPendingResponse.wait()`)

        Args:
            response: The actual response to the command.
        """
        self._q.put_nowait(response)

    def set_error(self, error: OBCCmdSysResponseError):
        """Sets the response to an error (unblocking anyone who called `OBCPendingResponse.wait()`)

        Args:
            error: The exception that was raised.
        """
        self._q.put_nowait(error)

    def wait(self, timeout: float = None) -> OBCResponse:
        """Waits for the response to this command to be supplied (blocks the current thread)
        for up to timeout seconds.

        Args:
            timeout: The maximum time to wait for a response to this command. None waits indefinitely. Defaults to None.

        Raises:
            OBCCmdSysResponseError: If an error occurred handling the response once it was received.

        Returns:
            The actual response to the command or None if no response was received before the timeout elapsed.
        """
        try:
            value = self._q.get(block=True, timeout=timeout)
            if isinstance(value, Exception):
                raise value
            return value
        except queue.Empty:
            return None
