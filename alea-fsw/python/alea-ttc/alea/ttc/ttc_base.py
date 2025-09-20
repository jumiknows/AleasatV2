from typing import Callable
from enum import Enum
import pathlib
import threading
import logging
from opentelemetry import trace

from alea.common import alea_time

from alea.obcfw.cmd_sys import cmd_sys_spec
from alea.obcfw.log import log_spec
from alea.obcfw.task import task_spec
from alea.obcfw.util import spec_utils

from alea.ttc.protocol.generic import routing
from alea.ttc.protocol import obc_upper_protocol
from alea.ttc.protocol.comms import comms_app_protocol
from alea.ttc.protocol.comms import comms_datalink
from alea.ttc.protocol.app import app_protocol
from alea.ttc.protocol.app import app_cmd_sys
from alea.ttc.protocol.app import app_log
from alea.ttc.protocol.comms import comms_datalink
from alea.ttc import cmd_sys
from alea.ttc.interface import ttc_interface
from alea.ttc.interface import obc_serial_interface
from alea.ttc.interface import comms_serial_interface
from alea.ttc.util import data_utils

logger = logging.getLogger(__name__)
tracer = trace.get_tracer(__name__)

DEFAULT_CMD_TIMEOUT = 10

LOG_CMD_SYS_SCHED_RESP = 67

class TTCError(Exception):
    def __init__(self, msg: str = None, protocol_err: app_protocol.OBCAppProtocolError = None):
        self._protocol_err = protocol_err
        if protocol_err is None:
            super().__init__(msg)
        else:
            super().__init__(str(protocol_err))

    @property
    def protocol_err(self) -> app_protocol.OBCAppProtocolError:
        return self._protocol_err

class OBCEventListener:
    """Interface for an object to be notified of all commands and responses sent on the OBC interface.
    """

    def cmd_event(self, cmd: cmd_sys.OBCCmd):
        """This function is called for every command sent to the OBC. The function is called immediately prior
        to sending the command.

        Args:
            cmd: The command that will be sent.
        """
        pass

    def resp_event(self, resp: cmd_sys.OBCResponse | cmd_sys.OBCPendingResponse):
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

    def error_event(self, cmd: cmd_sys.OBCCmd, error: TTCError):
        """This function is called if an error occurs sending/receiving a command/response.

        Args:
            cmd: The command that was being sent when the error occurred.
            error: The error that occurred.
        """
        pass

class TTCBase:
    """Base API for telemetry and telecommand of ALEASAT.
    """

    class InterfaceType(Enum):
        OBC_SERIAL   = (0, obc_serial_interface.OBCSerialInterface, comms_datalink.HWID.GROUND)
        COMMS_SERIAL = (1, comms_serial_interface.CommsSerialInterface, comms_datalink.HWID.LOCAL)

        @property
        def id(self) -> int:
            return self.value[0]

        @property
        def class_(self) -> type[ttc_interface.TTCInterface]:
            return self.value[1]

        @property
        def hwid(self) -> comms_datalink.HWID:
            return self.value[2]

    def __init__(self, interface_type: InterfaceType, cmd_sys_specs_paths: list[pathlib.Path] = None, log_specs_paths: list[pathlib.Path] = None, task_specs_paths: list[pathlib.Path] = None):
        """Initializes a new TTCBase instance.

        Sets up ether a Serial or RF interface to the TTCBase but does not open the connection.
        You must call `TTCBase.start()` to open the connection.

        The provided command system, log and task specifications will be loaded at initialization time.
        The loaded specifications can be updated at runtime by calling `TTCBase.load_cmd_sys_specs`,
        `TTCBase.load_log_specs` or `TTCBase.load_task_specs`.

        Args:
            interface_type: The physical interface over which the TTC will operate
            cmd_sys_specs_paths: List of command system specification JSON files
            log_specs_path: Path to the log specifications JSON file
            task_specs_paths: List of task specification JSON files
        """
        self._upper_protocol = obc_upper_protocol.OBCUpperProtocol(interface_type.hwid, log_specs=None) # Log specs will be loaded afterwards
        self._interface = interface_type.class_(self._upper_protocol)
        self._interface_type = interface_type

        self._cmd_sys_specs: cmd_sys_spec.OBCCmdSysSpecs = None
        self.load_cmd_sys_specs(cmd_sys_specs_paths)

        self._log_specs: log_spec.OBCLogGroupSpecs = None
        self.load_log_specs(log_specs_paths)

        self._task_specs: task_spec.OBCTaskSpecs = None
        self.load_task_specs(task_specs_paths)

        # Unique ID counter for instances of commands (currently this is unique only during a single run of San Antonio).
        # TODO: Make the instance ID unique forever.
        self._cmd_inst_id = 0

        # Listen to responses to scheduled commands
        self._pending_responses: dict[int, cmd_sys.OBCPendingResponse] = {}
        self._sched_resp_source = self.add_log_listener(queue_size=100)
        self._sched_resp_event = threading.Event()
        self._sched_resp_thread: threading.Thread = None

        self._obc_event_listeners: list[OBCEventListener] = []

    @property
    def interface_type(self) -> InterfaceType:
        return self._interface_type

    @property
    def connected(self) -> bool:
        """True if the interface to the OBC is currently connected.
        """
        return self._interface.connected

    @property
    def cmd_sys_specs(self) -> cmd_sys_spec.OBCCmdSysSpecs:
        return self._cmd_sys_specs

    @property
    def log_specs(self) -> log_spec.OBCLogGroupSpecs:
        return self._log_specs

    @property
    def task_specs(self) -> task_spec.OBCTaskSpecs:
        return self._task_specs

    @property
    def _app_protocol(self) -> app_protocol.OBCAppProtocol:
        return self._upper_protocol.app

    @property
    def _comms_app_protocol(self) -> comms_app_protocol.CommsAppProtocol:
        return self._upper_protocol.app_comms

    def load_cmd_sys_specs(self, specs_paths: list[pathlib.Path] = None):
        """Load one or more collections of command system specifications from the filesystem.
        """
        data = None

        if specs_paths is not None:
            data = spec_utils.load_json_files(specs_paths)

        self._cmd_sys_specs = cmd_sys_spec.OBCCmdSysSpecs.load(data)

    def load_log_specs(self, specs_paths: list[pathlib.Path] = None):
        """Load a collection of log specifications from the filesystem.
        """
        data = None

        if specs_paths is not None:
            data = spec_utils.load_json_files(specs_paths)

        self._log_specs = log_spec.OBCLogGroupSpecs.load(data)

        self._app_protocol.update_log_specs(self._log_specs)

    def load_task_specs(self, specs_paths: list[pathlib.Path] = None):
        """Load one or more collections of task specifications from the filesystem.
        """
        data = None

        if specs_paths is not None:
            data = spec_utils.load_json_files(specs_paths)

        self._task_specs = task_spec.OBCTaskSpecs.load(data)

    def start(self, *args, **kwargs) -> bool:
        # Reset protocols
        self._app_protocol.reset()

        # Start scheduled response thread
        self._sched_resp_thread = threading.Thread(target=self._run_sched_resp, daemon=True)
        self._sched_resp_event.clear()
        self._sched_resp_thread.start()

        return self._interface.start(*args, **kwargs)

    def stop(self):
        """Closes a connection and terminates the background threads.
        """
        if self._sched_resp_thread is not None:
            self._sched_resp_event.set()
            self._sched_resp_thread.join()
            self._sched_resp_thread = None

        return self._interface.stop()

    def _check_connected(self):
        if not self.connected:
            raise TTCError("Not connected")

    def add_log_listener(self, queue_size: int) -> routing.PacketSource[app_log.OBCLog]:
        return self._app_protocol.add_log_listener(queue_size=queue_size)

    def remove_log_listener(self, listener: routing.PacketSource[app_log.OBCLog]):
        self._app_protocol.remove_log_listener(listener)

    def add_obc_event_listener(self, listener: OBCEventListener):
        """Register a listener for commands/response events on this TTC interface.

        Args:
            listener: An instance of OBCEventListener that will be notified every time a command / response transaction occurs.
        """
        self._obc_event_listeners.append(listener)

    def remove_obc_event_listener(self, listener: OBCEventListener):
        """Remove a command/response event listener previously added through self.add_obc_event_listener(...)

        Args:
            listener: An instance of OBCEventListener that was previously passed to self.add_obc_event_listener(...)
        """
        self._obc_event_listeners.remove(listener)

    def clear_event_listeners(self):
        self._obc_event_listeners.clear()

    def send_obc_cmd_str(self, cmd_str: str, date_time: alea_time.ALEADateTime = alea_time.IMMEDIATE, timeout: int = DEFAULT_CMD_TIMEOUT) -> cmd_sys.OBCResponse | cmd_sys.OBCPendingResponse:
        """Parses a command string and sends it to the OBC.

        The command string should be in the format:
        ```
        <command name> <arg 1> <arg 2> ...
        ```

        Scheduling is not currently supported.

        Raises:
            OBCCmdSysSpecNotFoundError: If the command name specified in the string cannot be found.
            TTCError: If an error occurs sending the command or handling the response (if a response is expected).

        Args:
            cmd_str: The command string including the command name and arguments.
            date_time: A date/time for when to run the command. Defaults to alea_time.IMMEDIATE.
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Returns:
            The response to the command (if immediate) or an OBCPendingResponse (if scheduled).
        """
        cmd_fields = cmd_str.split()
        if len(cmd_fields) == 0:
            raise TTCError(f"Invalid command string: {cmd_str}")

        cmd_name = cmd_fields[0]
        del cmd_fields[0]

        return self.send_obc_cmd(cmd_name, *cmd_fields, date_time=date_time, timeout=timeout)

    @tracer.start_as_current_span("send_obc_cmd")
    def send_obc_cmd(self, cmd_name: str, *args, date_time: alea_time.ALEADateTime = alea_time.IMMEDIATE, timeout: int = DEFAULT_CMD_TIMEOUT, progress_callback: Callable[[data_utils.DataProgress], None] = None) -> cmd_sys.OBCResponse | cmd_sys.OBCPendingResponse:
        """Sends a command and waits for a response (if the command is configured to have a response).

        Args:
            cmd_name: A name of a command.
            *args: Any number of arguments for the command.
            date_time: A date/time for when to run the command. Defaults to alea_time.IMMEDIATE.
            timeout: Timeout (in seconds). None to block indefinitely. Defaults to DEFAULT_CMD_TIMEOUT.

        Raises:
            OBCCmdSysSpecNotFoundError: If the command name specified in the string cannot be found.
            TTCError: If an error occurs sending the command or handling the response (if a response is expected)

        Returns:
            The response to the command (if immediate) or an OBCPendingResponse (if scheduled).
        """
        self._check_connected()

        spec = self.cmd_sys_specs.get(cmd_name)

        if date_time.is_immediate:
            cmd_inst_id = self._cmd_inst_id
            self._cmd_inst_id += 1 # This must be incremented regardless of whether the command is successful
        else:
            # TODO This is a very bad HACK right now to make the UUID act like an instance ID
            # since the instance ID is not currently part of the packet
            cmd_inst_id = app_cmd_sys.OBCCmdSysMsgHeader(
                seq_num = self._app_protocol._cmd_seq_num, # No accessing private members - straight to jail
                cmd_id = spec.id,
                date_time = date_time,
                flags = 0,
            ).uuid

        cmd = cmd_sys.OBCCmd(spec, cmd_inst_id, args, date_time)
        self._notify_cmd(cmd)

        # Actually send the command
        try:
            imm_resp = self._app_protocol.send_cmd_recv_resp(cmd, timeout=timeout, progress_callback=progress_callback)
        except app_protocol.OBCAppProtocolError as e:
            e = TTCError(protocol_err=e)
            self._notify_error(cmd, e)
            raise e

        # Handle the response
        response = imm_resp

        if imm_resp and (not imm_resp.date_time.is_immediate) and (imm_resp.is_success_sched):
            # Command was scheduled

            if imm_resp.cmd_inst_id in self._pending_responses:
                err = TTCError(f"Command with same instance ID ({imm_resp.cmd_inst_id}) already scheduled")
                self._notify_error(cmd, err)
                raise err

            response = cmd_sys.OBCPendingResponse(imm_resp)
            self._pending_responses[imm_resp.cmd_inst_id] = response

        # Notify spies of the response
        self._notify_resp(response)

        return response

    def _notify_cmd(self, cmd: cmd_sys.OBCCmd):
        """Notify all registered event listeners that a command is being sent.

        Args:
            cmd: The command being sent.
        """
        for listener in self._obc_event_listeners:
            listener.cmd_event(cmd)

    def _notify_resp(self, resp: cmd_sys.OBCResponse | cmd_sys.OBCPendingResponse):
        """Notify all registered event listeners that a response was received.

        Args:
            resp: The response received (either an immediate OBCResponse or an OBCPendingResponse)
        """
        for listener in self._obc_event_listeners:
            listener.resp_event(resp)

    def _notify_error(self, cmd: cmd_sys.OBCCmd, error: TTCError):
        """Notify all registered event listeners that an error occurred sending a command / receiving a response.

        Args:
            cmd: The command that was being sent when the error occurred.
            error: The error that occurred.
        """
        for listener in self._obc_event_listeners:
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
                    logger.warning(f"CMD_SYS_SCHED_RESP log payload length too short: {recvd_log.payload_len} bytes (must be >= {app_cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE} bytes)")
                    continue

                payload = recvd_log.data['data']

                # Parse the command header
                resp_header = app_cmd_sys.OBCCmdSysMsgHeader.deserialize(payload[:app_cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE])
                cmd_inst_id = resp_header.uuid
                if cmd_inst_id in self._pending_responses:
                    # Parse the response
                    try:
                        spec = self.cmd_sys_specs.get(id=resp_header.cmd_id)
                    except cmd_sys_spec.OBCCmdSysSpecNotFoundError as e:
                        # Should never happen
                        logger.error(f"[OBCCmdSysSpecNotFoundError] {str(e)}", exc_info=True)
                        continue

                    resp_data = payload[app_cmd_sys.OBCCmdSysMsgHeader.HEADER_SIZE:]

                    try:
                        # TODO UUID is not unique enough (see docs for resp_header.uuid)
                        response = cmd_sys.OBCResponse.create(spec, cmd_inst_id, resp_header.date_time, resp_header.flags, resp_data)
                        self._pending_responses[cmd_inst_id].set_response(response)
                    except cmd_sys.OBCCmdSysResponseError as e:
                        self._pending_responses[cmd_inst_id].set_error(e)

                    self._notify_resp(self._pending_responses[cmd_inst_id])

                    # Delete our reference to the OBCPendingResponse object.
                    # If the caller that sent the command didn't keep their reference to the OBCPendingResponse,
                    # then there will be no more references left for the object so it can be garbage collected.
                    # (This is fine because if the caller didn't keep their reference, they clearly don't care about
                    #  ever receiving the response)
                    del self._pending_responses[cmd_inst_id]

    @tracer.start_as_current_span("send_comms_cmd")
    def send_comms_cmd(self, cmd: comms_datalink.CommsCommand, data: bytes = b'', timeout: float = None) -> comms_datalink.CommsDatagram:
        return self._comms_app_protocol.send_cmd_recv_resp(cmd, data=data, timeout=timeout)
