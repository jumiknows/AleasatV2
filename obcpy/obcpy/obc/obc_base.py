from typing import List, Union, Dict
import pathlib
import json
import threading

from obcpy import cmd_sys
from obcpy import log_sys
from obcpy.utils import obc_time
from obcpy.utils import exc

from .interface import serial_interface
from .interface.protocol import app_cmd_sys

DEFAULT_CMD_TIMEOUT = 5

LOG_CMD_SYS_SCHED_RESP = 67

class OBCEventListener:
    """Interface for an object to be notified of all commands and responses sent on the OBC interface.
    """

    def cmd_event(self, cmd: cmd_sys.cmd.OBCCmd):
        """This function is called for every command sent to the OBC. The function is called immediately prior
        to sending the command.

        Args:
            cmd: The command that will be sent.
        """
        pass

    def resp_event(self, resp: Union[cmd_sys.resp.OBCResponse, cmd_sys.resp.OBCPendingResponse]):
        """This function is called for every response received from the OBC.
        
        For scheduled commands this function will be called twice. Once with the immediate response
        to the scheduling command and once with the later scheduled response.

        Both responses for a scheduled command will be passed as OBCPendingResponse objects.
        - For the first response, you can call resp.sched_resp to access the OBCResponse object for
          the scheduling command.
        - For the second response, you can call resp.wait(0) which will immediately return the actual
          OBCResponse object.

        Args:
            resp: The response that was received.
                  If the command was immediate an OBCResponse object is passed.
                  If the command was scheduled an OBCPendingResponse object is passed for both responses.
        """
        pass

    def error_event(self, cmd: cmd_sys.cmd.OBCCmd, error: exc.OBCError):
        """This function is called if an error occurs sending/receiving a command/response.

        Args:
            cmd: The command that was being sent when the error occurred.
            error: The error that occurred.
        """
        pass

class OBCBase:
    """Base API for interacting with an OBC.
    """

    def __init__(self, cmd_sys_specs_paths: List[pathlib.Path], log_specs_path: pathlib.Path):
        """Initializes a new OBC instance.

        Sets up a serial interface to the OBC but does not open the connection.
        You must call `OBC.start()` to open the connection.

        The provided command system and log specifications will be loaded at initialization time.
        The loaded specifications can be updated at runtime by calling `OBCBase.load_cmd_sys_specs`
        or `OBCBase.load_log_specs`.

        Args:
            cmd_sys_specs_paths: List of command system specification JSON files
            log_specs_path: Path to the log specifications JSON file

        The loaded specifications can be replaced at any time by calling `OBC.load_cmd_sys_specs`.
        """
        self._cmd_sys_specs: cmd_sys.spec.OBCCmdSysSpecs = None
        self.load_cmd_sys_specs(cmd_sys_specs_paths)

        self._log_specs: log_sys.log_spec.OBCLogGroupSpecs = None
        self.interface = serial_interface.OBCSerialInterface(None)
        self.load_log_specs(log_specs_path)

        # Unique ID counter for instances of commands (currently this is unique only during a single run of San Antonio).
        # TODO: Make the instance ID unique forever.
        self._cmd_inst_id = 0

        # Listen to responses to scheduled commands
        self._pending_responses: Dict[int, cmd_sys.resp.OBCPendingResponse] = {}
        self._sched_resp_source = self.interface.protocol.add_log_listener(queue_size=100)
        self._sched_resp_event = threading.Event()
        self._sched_resp_thread: threading.Thread = None

        self._event_listeners: List[OBCEventListener] = []

    @property
    def connected(self) -> bool:
        """True if the interface to the OBC is currently connected.
        """
        return self.interface.connected

    @property
    def cmd_sys_specs(self) -> cmd_sys.spec.OBCCmdSysSpecs:
        return self._cmd_sys_specs

    @property
    def log_specs(self) -> log_sys.log_spec.OBCLogGroupSpecs:
        return self._log_specs

    def load_cmd_sys_specs(self, specs_paths: List[pathlib.Path]):
        """Load one or more collections of command system specifications from the filesystem.
        """
        json_blobs = []

        for specs_path in specs_paths:
            with open(specs_path, "r") as specs_file:
                specs_json = json.load(specs_file)
                json_blobs.append(specs_json)

        self._cmd_sys_specs = cmd_sys.spec.OBCCmdSysSpecs.from_json(*json_blobs)

    def load_log_specs(self, specs_path: pathlib.Path):
        """Load a collection of log specifications from the filesystem.
        """
        json_blob = None

        with open(specs_path, "r") as specs_file:
            specs_json = json.load(specs_file)
            json_blob = specs_json

        self._log_specs = log_sys.log_spec.OBCLogGroupSpecs.from_json(json_blob)

        self.interface.update_log_specs(self._log_specs)

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

    def add_event_listener(self, listener: OBCEventListener):
        """Register a listener for commands/response events on this OBC interface.

        Args:
            listener: An instance of OBCEventListener that will be notified every time a command / response transaction occurs.
        """
        self._event_listeners.append(listener)

    def remove_event_listener(self, listener: OBCEventListener):
        """Remove a command/response event listener previously added through self.add_event_listener(...)

        Args:
            listener: An instance of OBCEventListener that was previously passed to self.add_event_listener(...)
        """
        self._event_listeners.remove(listener)

    def send_cmd_str(self, cmd_str: str, date_time: obc_time.OBCDateTime = obc_time.IMMEDIATE, timeout: int = DEFAULT_CMD_TIMEOUT) -> Union[cmd_sys.resp.OBCResponse, cmd_sys.resp.OBCPendingResponse]:
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

    def send_cmd(self, cmd_name: str, *args, date_time: obc_time.OBCDateTime = obc_time.IMMEDIATE, timeout: int = DEFAULT_CMD_TIMEOUT) -> Union[cmd_sys.resp.OBCResponse, cmd_sys.resp.OBCPendingResponse]:
        """Sends a command and waits for a response (if the command is configured to have a response).

        Args:
            cmd_name: A name of a command.
            *args: Any number of arguments for the command.
            date_time: A date/time for when to run the command. Defaults to obc_time.IMMEDIATE.
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Returns:
            The response to the command (if immediate) or an OBCPendingResponse (if scheduled).
        """
        cmd_sys_spec = self.cmd_sys_specs.get(cmd_name)

        if date_time.is_immediate:
            cmd_inst_id = self._cmd_inst_id
            self._cmd_inst_id += 1 # This must be incremented regardless of whether the command is successful
        else:
            # TODO This is a very bad HACK right now to make the UUID act like an instance ID
            # since the instance ID is not currently part of the packet
            cmd_inst_id = app_cmd_sys.OBCCmdSysMsgHeader(
                seq_num = self.interface.protocol._cmd_seq_num, # No accessing private members - straight to jail
                cmd_id = cmd_sys_spec.id,
                date_time = date_time,
                flags = 0,
            ).uuid

        cmd = cmd_sys.cmd.OBCCmd(cmd_sys_spec, cmd_inst_id, args, date_time)
        self._notify_cmd(cmd)

        # Actually send the command
        try:
            imm_resp = self.interface.protocol.send_cmd_recv_resp(cmd, timeout=timeout)
        except exc.OBCError as e:
            self._notify_error(cmd, e)
            raise e

        # Handle the response
        response = imm_resp

        if imm_resp and (not imm_resp.date_time.is_immediate) and (imm_resp.is_success_sched):
            # Command was scheduled

            if imm_resp.cmd_inst_id in self._pending_responses:
                err = exc.OBCError(f"Command with same instance ID ({imm_resp.cmd_inst_id}) already scheduled")
                self._notify_error(cmd, err)
                raise err

            response = cmd_sys.resp.OBCPendingResponse(imm_resp)
            self._pending_responses[imm_resp.cmd_inst_id] = response

        # Notify spies of the response
        self._notify_resp(response)

        return response

    def _notify_cmd(self, cmd: cmd_sys.cmd.OBCCmd):
        """Notify all registered event listeners that a command is being sent.

        Args:
            cmd: The command being sent.
        """
        for listener in self._event_listeners:
            listener.cmd_event(cmd)

    def _notify_resp(self, resp: Union[cmd_sys.resp.OBCResponse, cmd_sys.resp.OBCPendingResponse]):
        """Notify all registered event listeners that a response was received.

        Args:
            resp: The response received (either an immediate OBCResponse or an OBCPendingResponse)
        """
        for listener in self._event_listeners:
            listener.resp_event(resp)

    def _notify_error(self, cmd: cmd_sys.cmd.OBCCmd, error: exc.OBCError):
        """Notify all registered event listeners that an error occurred sending a command / receiving a response.

        Args:
            cmd: The command that was being sent when the error occurred.
            error: The error that occurred.
        """
        for listener in self._event_listeners:
            listener.error_event(cmd, error)

    def _run_sched_resp(self):
        """Run function for thread that listens for responses to scheduled commands that were sent over logs.
        """
        while not self._sched_resp_event.is_set():
            recvd_logs = self._sched_resp_source.read(timeout=0.1)
            if not recvd_logs:
                continue

            for recvd_log in recvd_logs:
                if recvd_log.log_id != LOG_CMD_SYS_SCHED_RESP:
                    # Ignore logs that aren't coming from the command system scheduled task
                    continue

                if recvd_log.payload_len < app_cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE:
                    print(f"CMD_SYS_SCHED_RESP log payload length too short: {recvd_log.payload_len} bytes (must be >= {app_cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE} bytes)")
                    continue

                # Parse the command header
                resp_header = app_cmd_sys.OBCCmdSysMsgHeader.deserialize(recvd_log.payload[:app_cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE])
                cmd_inst_id = resp_header.uuid
                if cmd_inst_id in self._pending_responses:
                    # Parse the response
                    try:
                        cmd_sys_spec = self.cmd_sys_specs.get(id=resp_header.cmd_id)
                    except exc.OBCCmdNotFoundError as e:
                        # Should never happen
                        print(f"[OBCCmdNotFoundError] {str(e)}")
                        continue

                    resp_data = recvd_log.payload[app_cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE:]

                    try:
                        # TODO UUID is not unique enough (see docs for resp_header.uuid)
                        response = cmd_sys.resp.OBCResponse.create(cmd_sys_spec, cmd_inst_id, resp_header.date_time, resp_header.flags, resp_data)
                        self._pending_responses[cmd_inst_id].set_response(response)
                    except exc.OBCCmdSysResponseError as e:
                        self._pending_responses[cmd_inst_id].set_error(e)

                    self._notify_resp(self._pending_responses[cmd_inst_id])

                    # Delete our reference to the OBCPendingResponse object.
                    # If the caller that sent the command didn't keep their reference to the OBCPendingResponse,
                    # then there will be no more references left for the object so it can be garbage collected.
                    # (This is fine because if the caller didn't keep their reference, they clearly don't care about
                    #  ever receiving the response)
                    del self._pending_responses[cmd_inst_id]
