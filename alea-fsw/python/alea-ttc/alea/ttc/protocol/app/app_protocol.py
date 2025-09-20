from typing import Callable

from alea.common import alea_time

from alea.obcfw.log import log_spec
from alea.obcfw.cmd_sys import cmd_sys_spec

from alea.ttc.protocol.generic import packet
from alea.ttc.protocol.generic import layer_impl
from alea.ttc.protocol.generic import routing
from alea.ttc.protocol.generic import routing_impl
from alea.ttc import cmd_sys
from alea.ttc.util import data_utils

from . import app_log
from . import app_cmd_sys

class OBCAppProtocolError(Exception):
    def __init__(self, msg: str = None, cmd: cmd_sys.OBCCmd = None, spec: cmd_sys_spec.OBCCmdSysSpec = None,
                 cmd_err: cmd_sys_spec.OBCCmdSysSpecEncodeError = None, resp_err: cmd_sys.OBCCmdSysResponseError = None):
        if spec is None:
            spec = cmd.spec

        self._cmd = cmd
        self._spec = spec
        self._cmd_err = cmd_err
        self._resp_err = resp_err

        full_msg = ""
        if spec is not None:
            full_msg += f"{self.__class__.__name__} on command: {cmd.spec.name} ({cmd.spec.id})."
        if cmd_err is not None:
            full_msg += f" {cmd_err}."
        if resp_err is not None:
            full_msg += f" {resp_err}."
        if msg is not None:
            full_msg += f" {msg}"

        super().__init__(full_msg)

    @property
    def cmd(self) -> cmd_sys.OBCCmd:
        return self._cmd

    @property
    def spec(self) -> cmd_sys_spec.OBCCmdSysSpec:
        return self._spec

    @property
    def cmd_err(self) -> cmd_sys_spec.OBCCmdSysSpecEncodeError:
        return self._cmd_err

    @property
    def resp_err(self) -> cmd_sys.OBCCmdSysResponseError:
        return self._resp_err

class OBCAppProtocol:
    """Application layer of the OBC protocol stack
    """

    CHUNK_SIZE = 256

    def __init__(self, log_specs: log_spec.OBCLogGroupSpecs, tx_cmd_buf_size: int):
        """Initializes the protocol stack.
        """
        self._cmd_seq_num = 0

        # Commands (TX)
        self._tx_app_cmd      = app_cmd_sys.OBCAppCmdSysDest(layer_impl.TXBufferedProtocolLayer(tx_cmd_buf_size))

        # Command Responses (RX)
        self._rx_resp_raw     = routing_impl.ProtocolPacketSource[packet.RawPacket](layer_impl.RXRawProtocolLayer())
        self._rx_resp         = app_cmd_sys.OBCAppCmdSysSource(self._rx_resp_raw)

        # Logs (RX)
        self._rx_log_multi    = routing_impl.MultiPacketDest[app_log.OBCLog]()
        self._rx_log_protocol = app_log.OBCAppLogRX(log_specs)
        self._rx_log          = routing_impl.ProtocolPacketDest[app_log.OBCLog](self._rx_log_protocol, self._rx_log_multi)

    def set_tx_cmd_dest(self, dest: routing.PacketDest[packet.RawPacket]):
        self._tx_app_cmd.set_dest(dest)

    def set_rx_resp_src(self, src: routing.PacketSource[packet.Packet]):
        self._rx_resp_raw.set_src(src)

    @property
    def rx_log_dest(self) -> routing.PacketDest[packet.Packet]:
        return self._rx_log

    def reset(self):
        # Reset the entire TX chain
        self._tx_app_cmd.reset()
        # Reset the entire RX chain
        self._rx_resp.reset()
        # Logs will be reset by whoever writes to them

    def update_log_specs(self, log_specs: log_spec.OBCLogGroupSpecs):
        """Update the log specifications used to parse incoming logs

        Args:
            log_specs: New log specifications
        """
        self._rx_log_protocol.update_log_specs(log_specs)

    def add_log_listener(self, queue_size: int = 0) -> routing.PacketSource[app_log.OBCLog]:
        """Adds a bridge to the protocol stack to receive OBC logs.

        Any other PacketSource's created by calling this method will still receive all of the logs.

        Args:
            queue_size: Maximum size of the queue (number of logs)

        Returns:
            A PacketSource for OBC logs.
        """

        bridge = routing_impl.QueuePacketBridge[app_log.OBCLog](maxsize=queue_size)
        self._rx_log_multi.add_dest(bridge)
        return bridge

    def remove_log_listener(self, listener: routing.PacketSource[app_log.OBCLog]):
        """Removes a log listener previously added by `OBCSerialProtocol.add_log_listener()`

        Args:
            listener: Listener returned from `OBCSerialProtocol.add_log_listener()`
        """
        self._rx_log_multi.remove_dest(listener)

    # Send Command + Receive Response ------------------------------------------------------------------

    def send_cmd_recv_resp(self, cmd: cmd_sys.OBCCmd, timeout: float = None, progress_callback: Callable[[data_utils.DataProgress], None] = None) -> cmd_sys.OBCResponse:
        """Sends a command (with automatic argument encoding) and waits for the response (with automatic data decoding).

        Args:
            cmd: The command, its arguments and when it should be executed.
            timeout: Timeout to send the command and receive the response (None waits indefinitely if necessary). Defaults to None.
            progress_callback: Optional callback that will be called after every `CHUNK_SIZE` bytes of response data is transferred.

        Raises:
            OBCAppProtocolError: If the incorrect number of arguments is provided or one of the arguments has an invalid type or value.
                                 If a response is expected and the response data does not contain at least one byte for the response code.
                                 If a response is expected and the response code contained in the response data is invalid.

        Returns:
            An `OBCResponse` object if the command has a response (raw or fields), otherwise None.
        """
        # Send Command
        self.send_cmd(cmd.spec, *cmd.args, date_time=cmd.date_time, timeout=timeout)

        if cmd.spec.resp or (not cmd.date_time.is_immediate):
            # Command that are specified to have a response or any command that is scheduled will have a response.

            # Read Response Header
            resp_header = self.recv_resp_header(timeout=timeout)

            if resp_header is None:
                raise OBCAppProtocolError(f"No response header received", cmd=cmd)

            # Read Response Data
            if progress_callback is None:
                # Read data all at once
                resp_data = self.recv_resp_data(resp_header.data_len, timeout=timeout)
            else:
                # Read data in chunks
                resp_data = bytearray()
                bytes_left = resp_header.data_len
                while bytes_left > 0:
                    new_data = self.recv_resp_data(min(self.CHUNK_SIZE, bytes_left), timeout=timeout)
                    resp_data.extend(new_data)
                    bytes_left -= len(new_data)

                    progress_callback(data_utils.DataProgress(len(resp_data), resp_header.data_len))

            if len(resp_data) != resp_header.data_len:
                raise OBCAppProtocolError(f"Insufficient response data received (received {len(resp_data)} bytes, expected {resp_header.data_len} bytes)", cmd=cmd)

            if resp_header.cmd_id != cmd.spec.id:
                raise OBCAppProtocolError(f"Command ID mismatch. Expected: {cmd.spec.id}. Received: {resp_header.cmd_id}", cmd=cmd)

            if resp_header.date_time != cmd.date_time:
                raise OBCAppProtocolError(f"Date/time mismatch. Expected: {cmd.date_time}. Received: {resp_header.date_time}", cmd=cmd)

            # The immediate response to scheduling a command doesn't contain the data fields
            ignore_fields = not cmd.date_time.is_immediate

            try:
                resp = cmd_sys.OBCResponse.create(cmd.spec, cmd.inst_id, resp_header.date_time, resp_header.flags, resp_data, ignore_fields=ignore_fields)
            except cmd_sys.OBCCmdSysResponseError as e:
                raise OBCAppProtocolError(resp_err=e)

            return resp

        return None

    # Send Command -------------------------------------------------------------------------------------

    def send_cmd(self, spec: cmd_sys_spec.OBCCmdSysSpec, *args, date_time: alea_time.ALEADateTime = alea_time.IMMEDIATE, timeout: float = None) -> app_cmd_sys.OBCCmdSysMsgHeader:
        """Sends a command with automatic encoding of the command arguments.

        Args:
            spec: Specification for the command ID and automatic argument encoding.
            date_time: When the command should be executed. Defaults to IMMEDIATE.
            timeout: Timeout to send the command (None waits indefinitely if necessary). Defaults to None.

        Raises:
            OBCAppProtocolError: If the incorrect number of arguments is provided or one of the arguments
                                 has an invalid type or value.

        Returns:
            The `app_cmd_sys.OBCCmdSysMsgHeader` instance that was sent at the start of the command.
        """
        try:
            data = spec.encode_args(*args)
        except cmd_sys_spec.OBCCmdSysSpecEncodeError as e:
            raise OBCAppProtocolError(cmd_err=e)

        header = app_cmd_sys.OBCCmdSysMsgHeader(self._cmd_seq_num, spec.id, date_time, 0, False, len(data))
        self.send_cmd_raw(header, data, timeout=timeout)
        self._tx_app_cmd.flush(timeout=timeout)
        return header

    def send_cmd_raw(self, header: app_cmd_sys.OBCCmdSysMsgHeader, data: bytes = None, timeout: float = None):
        """Sends a raw command (no automatic encoding of arguments).

        The call to this method may optionally include some or all of the command data.
        If not all of the data is sent with this call, the remaining data can be sent
        through subsequent calls to `OBCSerialProtocol.send_cmd_data()`.

        Args:
            header: A populated command header.
            data: Optional data to send immediately after the header. Defaults to None.
            timeout: Timeout to send the command (None waits indefinitely if necessary). Defaults to None.
        """
        self._tx_app_cmd.write_header(header, timeout=timeout)
        if data is not None:
            self.send_cmd_data(data, timeout=timeout)
        self._cmd_seq_num  = (self._cmd_seq_num + 1) & 0xFF

    def send_cmd_data(self, data: bytes, timeout: float = None):
        """Sends data for a command.

        This can be called after `OBCSerialProtocol.send_cmd_raw()` to finish sending command data.

        Args:
            data: Raw data to send.
            timeout: Timeout to send the command (None waits indefinitely if necessary). Defaults to None.
        """
        self._tx_app_cmd.write_data(data, timeout=timeout)

    # Receive Response ---------------------------------------------------------------------------------

    def recv_resp_header(self, timeout: float = None) -> app_cmd_sys.OBCCmdSysMsgHeader:
        """Reads enough bytes from the RX stack to get a command system message header for a response
        and parse it.

        Args:
            timeout: Timeout to read the response header (None waits indefinitely if necessary). Defaults to None.

        Returns:
            A parsed `app_cmd_sys.OBCCmdSysMsgHeader` or None if a timeout occurred
        """
        return self._rx_resp.read_header(timeout=timeout)

    def recv_resp_data(self, num_bytes: int, timeout: float = None) -> bytes:
        """Reads num_bytes of response data from the RX stack.

        Args:
            num_bytes: The number of bytes to read.
            timeout: Timeout to read num_bytes (None waits indefinitely if necessary). Defaults to None.

        Returns:
            The bytes read. If a timeout occurred, fewer than num_bytes may be returned.
        """
        return self._rx_resp.read(num_bytes, timeout=timeout)
