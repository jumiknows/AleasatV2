from typing import Union, Dict, Any
from enum import IntEnum

from obcpy.obc_protocol import cmd_sys as cmd_sys_protocol
from obcpy.utils import data as data_utils
from obcpy.utils import exc

from . import spec

class OBCResponse:
    """Represents an actual received response to a command.

    If the response contained data that could be parsed into fields according to the cmd_sys_spec,
    then those fields are accessible via subscripting:

    ```python
    response = OBCResponse(...)
    print(response["field_name"])
    ```

    Attributes:
        cmd_sys_spec (readonly): The specification of the response this represents.
        header (readonly): The `OBCCmdSysMsgHeader` header that was sent with the response.
        code (readonly): The response status code.
        data (readonly): The raw response data.
        error (readonly): An `exc.OBCDecodeError` if one was thrown when attempting to decode the fields, otherwise None.
    """

    # This should match cmd_sys_resp_code_t in cmd_sys.h
    class Code(IntEnum):
        SUCCESS       = 1
        SUCCESS_SCHED = 2
        ERROR         = 3
        CMD_DNE       = 4
        NOT_IMPl      = 5

    def __init__(self, cmd_sys_spec: spec.OBCCmdSysSpec, header: cmd_sys_protocol.OBCCmdSysMsgHeader, code: Code, data: bytes, fields: Dict[str, Any], error: exc.OBCDecodeError = None):
        """Initializes a new OBCResponse.

        Args:
            cmd_sys_spec:  The specification of the response this represents.
            header: The `OBCCmdSysMsgHeader` header that was sent with the response.
            code: The response status code.
            data: The raw response data (excluding the code). Can be empty but cannot be None.
            fields: A dictionary mapping field names to values if available (may be None).
            error: An `exc.OBCDecodeError` if one was thrown when attempting to decode the fields.
        """
        self._cmd_sys_spec = cmd_sys_spec
        self._header = header
        self._code = code
        self._data = data
        self._fields = fields
        self._error = error

    @property
    def cmd_sys_spec(self) -> spec.OBCCmdSysSpec:
        return self._cmd_sys_spec

    @property
    def header(self) -> cmd_sys_protocol.OBCCmdSysMsgHeader:
        return self._header

    @property
    def code(self) -> Code:
        return self._code

    @property
    def data(self) -> bytes:
        return self._data

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

        return f"[OBCResponse] cmd={self.cmd_sys_spec.name} code={self.code.name}{data_str}{error_str}"

    @classmethod
    def create(cls, cmd_sys_spec: spec.OBCCmdSysSpec, resp_header: cmd_sys_protocol.OBCCmdSysMsgHeader, resp_data: bytes) -> "OBCResponse":
        """Create an `OBCResponse` object from a spec, header and data.

        Args:
            cmd_sys_spec:  The specification of the response to create.
            resp_header: The `OBCCmdSysMsgHeader` header that was sent with the response.
            resp_data: The raw response data following the header. Cannot be None.

        Raises:
            exc.OBCCmdSysResponseError: If the resp_data does not contain at least one byte for the response code.
                                        If the response code contained in the resp_data is invalid.

        Returns:
            An `OBCResponse` instance.
        """

        if len(resp_data) < 1:
            raise exc.OBCCmdSysResponseError(f"Response data missing response code for command: {cmd_sys_spec.name}")

        # Attempt to parse the response code
        try:
            code = cls.Code(resp_data[0])
        except ValueError:
            raise exc.OBCCmdSysResponseError(f"Invalid response code ({resp_data[0]}) for command: {cmd_sys_spec.name}")

        # Attempt to parse response fields
        fields: Dict[str, Any] = None
        error: exc.OBCDecodeError = None
        try:
            fields = cmd_sys_spec.decode_resp(resp_data[1:])
        except exc.OBCDecodeError as e:
            print(f"[OBCDecodeError] {str(e)}")
            fields = None
            error = e

        return OBCResponse(cmd_sys_spec, resp_header, code, resp_data[1:], fields, error=error)
