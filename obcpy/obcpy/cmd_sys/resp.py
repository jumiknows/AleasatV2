from typing import Dict, Any
from enum import IntEnum
import struct

from obcpy.obc_protocol import cmd_sys as cmd_sys_protocol
from obcpy.utils import data as data_utils
from obcpy.utils import exc
from obcpy.utils import obc_time

from . import spec

class OBCResponseDataHeader:
    """Represents the header at the beginning of the data field of an OBCResponse

    Attributes:
        code (readonly): The response status code.
        exec_datetime (readonly): The date/time when the command was executed.
    """

    FMT = f"!BI"
    HEADER_SIZE = 5

    # This should match cmd_sys_resp_code_t in cmd_sys.h
    class Code(IntEnum):
        SUCCESS       = 1
        SUCCESS_SCHED = 2
        ERROR         = 3
        CMD_DNE       = 4
        NOT_IMPl      = 5

    def __init__(self, code: Code, exec_datetime: obc_time.OBCDateTime):
        self._code          = code
        self._exec_datetime = exec_datetime

    @property
    def code(self) -> Code:
        return self._code

    @property
    def exec_datetime(self) -> obc_time.OBCDateTime:
        return self._exec_datetime

    def __str__(self) -> str:
        return f"code={self.code.name} exec_datetime={str(self.exec_datetime)}"

    @classmethod
    def deserialize(cls, data: bytes) -> "OBCResponseDataHeader":
        """Construct an OBCResponseDataHeader instance from a byte array.

        Args:
            data: A byte array containing the serialized OBCResponseDataHeader.

        Raises:
            exc.OBCCmdSysResponseError: If the data cannot be parsed into a valid OBCResponseDataHeader.

        Returns:
            A new OBCResponseDataHeader instance.
        """
        try:
            header_fields = struct.unpack(cls.FMT, data)
        except struct.error as e:
            raise exc.OBCCmdSysResponseError(f"Failed to decode response data header: {str(e)}")

        # Attempt to extract response code
        try:
            code = cls.Code(header_fields[0])
        except ValueError:
            raise exc.OBCCmdSysResponseError(f"Invalid response code ({header_fields[0]}).")

        # Extract exec_datetime
        exec_datetime = obc_time.OBCDateTime.from_timestamp(header_fields[1])

        return OBCResponseDataHeader(code, exec_datetime)

class OBCResponse:
    """Represents an actual received response to a command.

    If the response contained data that could be parsed into fields according to the cmd_sys_spec,
    then those fields are accessible via subscripting:

    ```python
    response = OBCResponse.create(...)
    print(response["field_name"])
    ```

    Attributes:
        cmd_sys_spec (readonly): The specification of the response this represents.
        header (readonly): The `OBCCmdSysMsgHeader` header that was sent with the response.
        data_header (readonly): The header at the beginning of the response data field.
        data (readonly): The raw response data.
        error (readonly): An `exc.OBCDecodeError` if one was thrown when attempting to decode the fields, otherwise None.
    """

    def __init__(self, cmd_sys_spec: spec.OBCCmdSysSpec, header: cmd_sys_protocol.OBCCmdSysMsgHeader, data_header: OBCResponseDataHeader, data: bytes, fields: Dict[str, Any], error: exc.OBCDecodeError = None):
        """Initializes a new OBCResponse.

        Args:
            cmd_sys_spec:  The specification of the response this represents.
            header: The `OBCCmdSysMsgHeader` header that was sent with the response.
            data_header: The header at the beginning of the response data field.
            data: The raw response data (excluding the data_header). Can be empty but cannot be None.
            fields: A dictionary mapping field names to values if available (may be None).
            error: An `exc.OBCDecodeError` if one was thrown when attempting to decode the fields.
        """
        self._cmd_sys_spec  = cmd_sys_spec
        self._header        = header
        self._data_header   = data_header
        self._data          = data
        self._fields        = fields
        self._error         = error

    @property
    def cmd_sys_spec(self) -> spec.OBCCmdSysSpec:
        return self._cmd_sys_spec

    @property
    def header(self) -> cmd_sys_protocol.OBCCmdSysMsgHeader:
        return self._header

    @property
    def data_header(self) -> OBCResponseDataHeader:
        return self._data_header

    @property
    def data(self) -> bytes:
        return self._data

    @property
    def is_success(self) -> bool:
        return (self.data_header.code == OBCResponseDataHeader.Code.SUCCESS)

    @property
    def is_success_sched(self) -> bool:
        return (self.data_header.code == OBCResponseDataHeader.Code.SUCCESS_SCHED)

    @property
    def has_fields(self) -> bool:
        """True if this response was expecting fields and successfully decoded the fields
        (even if the number of fields was zero).
        """
        return self._fields is not None

    @property
    def error(self) -> exc.OBCDecodeError:
        return self._error

    def __contains__(self, key: str) -> bool:
        return self.has_fields and (key in self._fields)

    def __getitem__(self, key: str) -> Any:
        if not self.has_fields:
            raise exc.OBCCmdSysResponseError(f"Response does not have fields")

        return self._fields[key]

    def __str__(self) -> str:
        data_str = ""
        if self.has_fields:
            data_str = " " + ", ".join(map(lambda name : f"{name}={str(self._fields[name])}", self._fields))
        else:
            data_str = " " + data_utils.bytes_to_hexstr(self.data)

        error_str = ""
        if self.error is not None:
            error_str = f" | error={str(self.error)}"

        return f"[OBCResponse] cmd={self.cmd_sys_spec.name} {str(self.data_header)}{data_str}{error_str}"

    @classmethod
    def create(cls, cmd_sys_spec: spec.OBCCmdSysSpec, resp_header: cmd_sys_protocol.OBCCmdSysMsgHeader, resp_data: bytes) -> "OBCResponse":
        """Create an `OBCResponse` object from a spec, header and data.

        Args:
            cmd_sys_spec:  The specification of the response to create.
            resp_header: The `OBCCmdSysMsgHeader` header that was sent with the response.
            resp_data: The raw response data following the header. Cannot be None.

        Raises:
            exc.OBCCmdSysResponseError: If the resp_data does not contain a valid data header.

        Returns:
            An `OBCResponse` instance.
        """

        if len(resp_data) < OBCResponseDataHeader.HEADER_SIZE:
            raise exc.OBCCmdSysResponseError(f"Response data missing data header for command: {cmd_sys_spec.name}")

        data_header = OBCResponseDataHeader.deserialize(resp_data[:OBCResponseDataHeader.HEADER_SIZE])
        data_data = resp_data[OBCResponseDataHeader.HEADER_SIZE:]

        # Attempt to parse response fields
        fields: Dict[str, Any] = None
        error: exc.OBCDecodeError = None
        try:
            fields = cmd_sys_spec.decode_resp(data_data)
        except exc.OBCDecodeError as e:
            print(f"[OBCDecodeError] {str(e)}")
            fields = None
            error = e

        return OBCResponse(cmd_sys_spec, resp_header, data_header, data_data, fields, error=error)
