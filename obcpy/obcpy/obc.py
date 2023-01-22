from typing import Union, Dict
import datetime
import pathlib
import json
import threading
import queue

from obcpy.interface import obc_serial_interface
from obcpy.obc_protocol import cmd_sys
from obcpy.cmd_sys import spec
from obcpy.cmd_sys import resp
from obcpy.utils import obc_time
from obcpy.utils import exc

DEFAULT_CMD_TIMEOUT = 5

LOG_CMD_SYS_SCHED_RESP = 67

class OBCPendingResponse:
    """A response that is pending to a scheduled command.

    Attributes:
        sched_resp (readonly): The response to the command that scheduled this command.
    """

    def __init__(self, sched_resp: resp.OBCResponse):
        self._sched_resp = sched_resp
        self._q = queue.Queue(maxsize=1)

    @property
    def sched_resp(self) -> resp.OBCResponse:
        """The response to the command that scheduled this command.
        """
        return self._sched_resp

    def set_response(self, response: resp.OBCResponse):
        """Sets the response (unblocking anyone who called `OBCPendingResponse.wait()`)

        Args:
            response: The actual response to the command.
        """
        self._q.put_nowait(response)

    def set_error(self, error: exc.OBCCmdSysResponseError):
        """Sets the response to an error (unblocking anyone who called `OBCPendingResponse.wait()`)

        Args:
            error: The exception that was raised.
        """
        self._q.put_nowait(error)

    def wait(self, timeout: float = None) -> resp.OBCResponse:
        """Waits for the response to this command to be supplied (blocks the current thread)
        for up to timeout seconds.

        Args:
            timeout: The maximum time to wait for a response to this command. None waits indefinitely. Defaults to None.

        Raises:
            exc.OBCCmdSysResponseError: If an error occurred handling the response once it was received.

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
        self._specs: spec.OBCCmdSysSpecs = None
        self.load_cmd_sys_specs(*cmd_sys_specs_paths)
        self.interface = obc_serial_interface.OBCSerialInterface()

        # Listen to responses to scheduled commands
        self._pending_responses: Dict[int, OBCPendingResponse] = {}
        self._sched_resp_source = self.interface.protocol.add_log_listener(queue_size=100)
        self._sched_resp_event = threading.Event()
        self._sched_resp_thread: threading.Thread = None

    @property
    def connected(self) -> bool:
        """True if the interface to the OBC is currently connected.
        """
        return self.interface.connected

    @property
    def specs(self) -> spec.OBCCmdSysSpecs:
        return self._specs

    def load_cmd_sys_specs(self, *specs_paths: pathlib.Path):
        """Load a set of command system specifications from the filesystem.
        """
        json_blobs = []

        for specs_path in specs_paths:
            with open(specs_path, "r") as specs_file:
                specs_json = json.load(specs_file)
                json_blobs.append(specs_json)

        self._specs = spec.OBCCmdSysSpecs.from_json(*json_blobs)

    def start(self, serial_port: str) -> bool:
        """Open a connection to the OBC on the provided serial port.

        This spawns several background threads to manage communication with the OBC.

        Args:
            serial_port: A name of a serial port (e.g. "COM7", "/dev/ttyS0")

        Returns:
            True if the connection was opened successfully, otherwise False.
        """
        self._sched_resp_thread = threading.Thread(target=self._run_sched_resp, daemon=True)
        self._sched_resp_event.clear()
        self._sched_resp_thread.start()
        return self.interface.start(serial_port)

    def stop(self):
        """Closes a connection to the OBC and terminates the background threads.
        """
        if self._sched_resp_thread is not None:
            self._sched_resp_event.set()
            self._sched_resp_thread.join()
            self._sched_resp_thread = None
        return self.interface.stop()

    def send_cmd_str(self, cmd_str: str, date_time: obc_time.OBCDateTime = obc_time.IMMEDIATE, timeout: int = DEFAULT_CMD_TIMEOUT) -> Union[resp.OBCResponse, OBCPendingResponse]:
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
            cmd_str: The command string including the command name and arguments.
            date_time: A date/time for when to run the command. Defaults to obc_time.IMMEDIATE.
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Returns:
            The response to the command (if immediate) or an OBCPendingResponse (if scheduled).
        """
        cmd_fields = cmd_str.split()
        if len(cmd_fields) == 0:
            raise exc.OBCError(f"Invalid command string: {cmd_str}")

        cmd_name = cmd_fields[0]
        del cmd_fields[0]

        return self.send_cmd(cmd_name, *cmd_fields, date_time=date_time, timeout=timeout)

    def send_cmd(self, cmd: str, *args, date_time: obc_time.OBCDateTime = obc_time.IMMEDIATE, timeout: int = DEFAULT_CMD_TIMEOUT) -> Union[resp.OBCResponse, OBCPendingResponse]:
        """Sends a command and waits for a response (if the command is configured to have a response).

        Args:
            cmd: A name of a command.
            *args: Any number of arguments for the command.
            date_time: A date/time for when to run the command. Defaults to obc_time.IMMEDIATE.
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Returns:
            The response to the command (if immediate) or an OBCPendingResponse (if scheduled).
        """
        cmd_sys_spec = self.specs.get(cmd)
        response = self.interface.protocol.send_cmd_recv_resp(cmd_sys_spec, *args, date_time=date_time, timeout=timeout)

        if response and (not response.header.date_time.is_immediate) and (response.is_success_sched):
            # Command was scheduled
            cmd_uuid = response.header.uuid

            if cmd_uuid in self._pending_responses:
                raise exc.OBCError(f"Command with same UUID ({cmd_uuid}) already scheduled: {str(response.header)}")

            pending_resp = OBCPendingResponse(response)
            self._pending_responses[cmd_uuid] = pending_resp
            return pending_resp
        else:
            return response

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

    def _run_sched_resp(self):
        """Run function for thread that listens for responses to scheduled commands that were sent over logs.
        """
        while not self._sched_resp_event.is_set():
            recvd_logs = self._sched_resp_source.read(timeout=0.1)
            if not recvd_logs:
                continue

            for recvd_log in recvd_logs:
                if recvd_log.func_id != LOG_CMD_SYS_SCHED_RESP:
                    # Ignore logs that aren't coming from the command system scheduled task
                    continue

                if recvd_log.payload_len < cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE:
                    print(f"CMD_SYS_SCHED_RESP log payload length too short: {recvd_log.payload_len} bytes (must be >= {cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE} bytes)")
                    continue

                # Parse the command header
                resp_header = cmd_sys.OBCCmdSysMsgHeader.deserialize(recvd_log.payload[:cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE])
                cmd_uuid = resp_header.uuid
                if cmd_uuid in self._pending_responses:
                    # Parse the response
                    try:
                        cmd_sys_spec = self.specs.get(id=resp_header.cmd_id)
                    except exc.OBCCmdNotFoundError as e:
                        # Should never happen
                        print(f"[OBCCmdNotFoundError] {str(e)}")
                        continue

                    resp_data = recvd_log.payload[cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE:]

                    try:
                        response = resp.OBCResponse.create(cmd_sys_spec, resp_header, resp_data)
                        self._pending_responses[cmd_uuid].set_response(response)
                    except exc.OBCCmdSysResponseError as e:
                        self._pending_responses[cmd_uuid].set_error(e)

                    # Delete our reference to the OBCPendingResponse object.
                    # If the caller that sent the command didn't keep their reference to the OBCPendingResponse,
                    # then there will be no more references left for the object so it can be garbage collected.
                    # (This is fine because if the caller didn't keep their reference, they clearly don't care about
                    #  ever receiving the response)
                    del self._pending_responses[cmd_uuid]
