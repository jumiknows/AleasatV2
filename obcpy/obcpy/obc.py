from typing import Union
import datetime
import pathlib
import json

from obcpy.interface import obc_serial_interface
from obcpy.cmd_sys import spec
from obcpy.cmd_sys import resp
from obcpy.utils import obc_time
from obcpy.utils import exc

DEFAULT_CMD_TIMEOUT = 5

class OBC:
    """API for interacting with an OBC.
    """

    def __init__(self, *cmd_sys_specs_paths: pathlib.Path):
        """Initializes a new OBC instance.

        Sets up a serial interface to the OBC but does not open the connection.
        You must call `OBC.start()` to open the connection.

        Multiple paths to command system specification JSON files can be passed to this constructor.
        All of the files will be loaded at initialization time.

        The loaded specifications can be replaced at any time by calling `OBC.load_cmd_sys_specs`.
        """
        self.load_cmd_sys_specs(*cmd_sys_specs_paths)
        self.interface = obc_serial_interface.OBCSerialInterface()

    @property
    def connected(self) -> bool:
        """True if the interface to the OBC is currently connected.
        """
        return self.interface.connected

    def load_cmd_sys_specs(self, *specs_paths: pathlib.Path):
        """Load a set of command system specifications from the filesystem.
        """
        json_blobs = []

        for specs_path in specs_paths:
            with open(specs_path, "r") as specs_file:
                specs_json = json.load(specs_file)
                json_blobs.append(specs_json)

        self.specs = spec.OBCCmdSysSpecs.from_json(*json_blobs)

    def start(self, serial_port: str) -> bool:
        """Open a connection to the OBC on the provided serial port.

        This spawns several background threads to manage communication with the OBC.

        Args:
            serial_port: A name of a serial port (e.g. "COM7", "/dev/ttyS0")

        Returns:
            True if the connection was opened successfully, otherwise False.
        """
        return self.interface.start(serial_port)

    def stop(self):
        """Closes a connection to the OBC and terminates the background threads.
        """
        return self.interface.stop()

    def send_cmd_str(self, cmd_str: str, timeout: int = DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
        """Parses a command string and sends it to the OBC.

        The command string should be in the format:
        ```
        <command name> <arg 1> <arg 2> ...
        ```

        Scheduling is not currently supported.

        Raises:
            exc.OBCCmdNotFoundError: If the command name specified in the string cannot be found.
            exc.OBCError: If the command string is invalid.
            exc.OBCEncodeError: If the incorrect number of arguments is provided or one of the arguments has an invalid value.
            exc.OBCCmdSysResponseError: If a response is expected and the response data does not contain at least one byte for the response code.
                                        If a response is expected and the response code contained in the response data is invalid.

        Args:
            cmd_str: The command string.
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Returns:
            The response to the command.
        """

        cmd_fields = cmd_str.split()
        if len(cmd_fields) == 0:
            raise exc.OBCError(f"Invalid command string: {cmd_str}")

        # Find the command system specification
        cmd_sys_spec = self.specs.get(cmd_fields[0])
        del cmd_fields[0]

        return self.interface.protocol.send_cmd_recv_resp(cmd_sys_spec, *cmd_fields, timeout=timeout)

    def send_cmd(self, cmd: str, *args, date_time: obc_time.OBCDateTime = obc_time.IMMEDIATE, timeout: int = DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
        """Sends a command and waits for a response (if the command is configured to have a response).

        Args:
            cmd: A name of a command.
            *args: Any number of arguments for the command.
            date_time: A date/time for when to run the command. Defaults to obc_time.IMMEDIATE.
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Returns:
            The response to the command.
        """
        cmd_sys_spec = self.specs.get(cmd)
        return self.interface.protocol.send_cmd_recv_resp(cmd_sys_spec, *args, date_time=date_time, timeout=timeout)

    def ping(self, timeout=DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
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
        if "timestamp" not in resp:
            return None

        timestamp: obc_time.OBCDateTime = resp["timestamp"]
        return timestamp

    def set_time(self, new_datetime: Union[obc_time.OBCDateTime, datetime.datetime] = None, timeout=DEFAULT_CMD_TIMEOUT) -> resp.OBCResponse:
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
