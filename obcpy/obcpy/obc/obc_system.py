from typing import Union, Dict, Tuple, List
import datetime

from obcpy import cmd_sys
from obcpy.utils import obc_time
from obcpy.data_fmt import data_field_impl
from obcpy.utils import exc

from .obc_base import OBCBase
from .obc_base import DEFAULT_CMD_TIMEOUT

class OBCSystemFeature(OBCBase):
    """Implementation of "system" features of the OBC (features that do not necessarily belong to a particular sub-system of the OBC).
    """

    def ping(self, timeout=DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        """Sends a PING command to the OBC.

        Args:
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Raises:
            exc.OBCCmdSysResponseError: If the response data does not contain at least one byte for the response code.
                                        If the response code contained in the response data is invalid.
        Returns:
            The response to the command.
        """
        return self.send_cmd("PING", timeout=timeout)

    def reset(self, timeout=DEFAULT_CMD_TIMEOUT):
        """Sends a RESET command to the OBC.

        Args:
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.
        """
        self.send_cmd("RESET", timeout=timeout)

    def get_time(self, timeout=DEFAULT_CMD_TIMEOUT) -> obc_time.OBCDateTime:
        """Sends a GET_TIME command to OBC to get the current on-board date/time.

        Args:
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Returns:
            The on-board date/time as an `obc_time.OBCDateTime` instance, or None if the command failed.
        """
        resp = self.send_cmd("GET_TIME", timeout=timeout)
        if "timestamp" not in resp.data:
            return None

        timestamp: obc_time.OBCDateTime = resp.data["timestamp"]
        return timestamp

    def set_time(self, new_datetime: Union[obc_time.OBCDateTime, datetime.datetime] = None, timeout=DEFAULT_CMD_TIMEOUT) -> cmd_sys.resp.OBCResponse:
        """Sends a SET_TIME command to the OBC to set the on-board date/time.

        Args:
            new_datetime: The date/time to set. If None, the current date/time will be set.
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Raises:
            exc.OBCCmdSysResponseError: If the response data does not contain at least one byte for the response code.
                                        If the response code contained in the response data is invalid.

        Returns:
            The response to the SET_TIME command.
        """
        if new_datetime is None:
            new_datetime = datetime.datetime.now()

        if isinstance(new_datetime, obc_time.OBCDateTime):
            new_obc_datetime = new_datetime
        else:
            new_obc_datetime = obc_time.OBCDateTime(new_datetime)

        return self.send_cmd("SET_TIME", new_obc_datetime.to_timestamp(), timeout=timeout)

    def get_stack_usages(self, timeout=DEFAULT_CMD_TIMEOUT) -> Dict[int, Tuple[int, int]]:
        stack_usages = {}

        resp = self.send_cmd("GET_MIN_STACK_SPACE", timeout=timeout)
        if resp.is_success:
            # Manually decode the data field
            data_field = data_field_impl.U16("min_stack_space", (self.task_specs.count,))

            resp_data = resp.data["min_stack_space"]

            if len(resp_data) != data_field.size:
                raise exc.OBCDecodeError(f"Response data has wrong size. Expected: {data_field.size}. Actual: {len(resp_data)}.")

            min_stack_space: List[int] = data_field.decode(resp_data)

            for task_spec in self.task_specs:
                stack_usages[task_spec.id] = ((task_spec.stack_size - min_stack_space[task_spec.id]), task_spec.stack_size)

        return stack_usages
