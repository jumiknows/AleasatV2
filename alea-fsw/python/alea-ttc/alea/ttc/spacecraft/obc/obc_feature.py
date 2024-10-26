import datetime

from alea.common import alea_time
from alea.common.data_field import data_field, data_field_impl

from alea.ttc import cmd_sys
from alea.ttc import ttc_base
from alea.ttc.ttc_base import DEFAULT_CMD_TIMEOUT

from . import obc_rtos_trace

class OBCFeature(ttc_base.TTCBase):
    """Implementation of OBC features.
    """

    def ping(self, timeout=DEFAULT_CMD_TIMEOUT) -> cmd_sys.OBCResponse:
        """Sends a PING command to the OBC.

        Args:
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Raises:
            exc.OBCCmdSysResponseError: If the response data does not contain at least one byte for the response code.
                                        If the response code contained in the response data is invalid.
        Returns:
            The response to the command.
        """
        return self.send_obc_cmd("PING", timeout=timeout)
    
    def get_uptime(self, timeout=DEFAULT_CMD_TIMEOUT) -> cmd_sys.OBCResponse:
        """Sends a GET_UPTIME command to the OBC.

        Args:
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Raises:
            exc.OBCCmdSysResponseError: If the response data does not contain at least one byte for the response code.
                                        If the response code contained in the response data is invalid.
        Returns:
            The response to the command.
        """
        return self.send_obc_cmd("GET_UPTIME", timeout=timeout)

    def reset(self, timeout=DEFAULT_CMD_TIMEOUT):
        """Sends a RESET command to the OBC.

        Args:
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.
        """
        self.send_obc_cmd("RESET", timeout=timeout)

    def get_time(self, timeout=DEFAULT_CMD_TIMEOUT) -> alea_time.ALEADateTime:
        """Sends a GET_TIME command to OBC to get the current on-board date/time.

        Args:
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Returns:
            The on-board date/time as an `alea_time.ALEADateTime` instance, or None if the command failed.
        """
        resp = self.send_obc_cmd("GET_TIME", timeout=timeout)
        if "timestamp" not in resp.data:
            return None

        timestamp: alea_time.ALEADateTime = resp.data["timestamp"]
        return timestamp

    def set_time(self, new_datetime: alea_time.ALEADateTime | datetime.datetime = None, timeout=DEFAULT_CMD_TIMEOUT) -> cmd_sys.OBCResponse:
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

        if isinstance(new_datetime, alea_time.ALEADateTime):
            new_obc_datetime = new_datetime
        else:
            new_obc_datetime = alea_time.ALEADateTime(new_datetime)

        return self.send_obc_cmd("SET_TIME", new_obc_datetime.to_timestamp(), timeout=timeout)

    def get_stack_usages(self, timeout=DEFAULT_CMD_TIMEOUT) -> dict[int, tuple[int, int]]:
        stack_usages = {}

        resp = self.send_obc_cmd("GET_MIN_STACK_SPACE", timeout=timeout)
        if resp.is_success:
            # Manually decode the data field
            field = data_field_impl.U16("min_stack_space", (self.task_specs.count,))

            resp_data = resp.data["min_stack_space"]

            if len(resp_data) != field.size:
                raise data_field.DataFieldError(f"Response data has wrong size. Expected: {field.size}. Actual: {len(resp_data)}.")

            min_stack_space: list[int] = field.decode(resp_data)

            for task_spec in self.task_specs:
                stack_usages[task_spec.id] = ((task_spec.stack_size - min_stack_space[task_spec.id]), task_spec.stack_size)

        return stack_usages

    def capture_rtos_trace(self, length: int, timeout=DEFAULT_CMD_TIMEOUT) -> obc_rtos_trace.OBCRTOSTrace:
        resp = self.send_obc_cmd("CAPTURE_RTOS_TRACE", length, timeout=timeout)
        if not resp.is_success:
            return None

        data = resp.data["trace"]
        return obc_rtos_trace.OBCRTOSTrace.from_bytes(data)
