from typing import Tuple, Dict, Any
from enum import IntEnum

from obcpy.protocol import packet
from obcpy.protocol import layer_impl

from obcpy import log_sys

from obcpy.utils import obc_time
from obcpy.utils import data as data_utils
from obcpy.utils import exc

LOG_CMD_SYS_SCHED_RESP_LOG_ID = 67

class OBCLog(packet.Packet):
    def __init__(self, date_time: obc_time.OBCDateTime = None, log_id: int = None, signal_id: int = None, payload_len: int = None, group_name: str = None, group_desc: str = None, signal_level: log_sys.log_spec.OBCLogLevel = None, signal_name: str = None, signal_desc: str = None, data: Dict[str, Any] = None):
        super().__init__()
        self.date_time   = date_time
        self.log_id      = log_id
        self.signal_id   = signal_id
        self.payload_len = payload_len
        
        self.group_name  = group_name
        self.group_desc  = group_desc
        self.signal_level= signal_level
        self.signal_name = signal_name
        self.signal_desc = signal_desc
        self.data        = data

    def extract_data(self) -> bytes:
        raise NotImplementedError()

    def serialize(self) -> bytes:
        data = bytearray()
        data.extend(self.date_time.to_timestamp().to_bytes(4, "big"))
        data.append(self.log_id)
        data.append(self.signal_id)
        data.append(self.payload_len)
        
        group_spec, signal_spec = self._log_specs.get(group_id=self._packet.log_id, signal_id=self._packet.signal_id)
        data.extend(signal_spec.encode_data(self.data))
        return data

    @classmethod
    def deserialize(cls, data: bytes) -> "OBCLog":
        raise NotImplementedError()

    def __str__(self) -> str:
        line = "[LOG] "
        line += f"[{self.signal_level.name:7s}] "
        line += f"[{self.date_time.date_time.strftime('%Y-%m-%d %H:%M:%S')}] "
        line += f"[Log ID: {self.log_id:03d}, Name: {self.group_name}] "
        line += f"[Sig ID: {self.signal_id:03d}, Name: {self.signal_name}] "
        line += f"[Signal Description: {self.signal_desc}] "
        line += f"{self.data}"
        return line

class OBCAppLogRX(layer_impl.StreamToPacketProtocolLayer[OBCLog]):
    class State(IntEnum):
        HEADER  = 0,
        PAYLOAD = 1,

    HEADER_SIZE = 7

    def __init__(self, log_specs: log_sys.log_spec.OBCLogGroupSpecs):
        super().__init__(layer_impl.DirectionalProtocolLayer.Direction.RX, self.State.HEADER, OBCLog)
        
        self._log_specs = log_specs

    def update_log_specs(self, log_specs: log_sys.log_spec.OBCLogGroupSpecs):
        """Update the log specifications used to parse incoming logs

        Args:
            log_specs: New log specifications
        """
        self._log_specs = log_specs

    def _handle_state(self, state: int, data: bytes) -> Tuple[int, int, bool]:
        state_handlers = {
            self.State.HEADER: self._handle_header,
            self.State.PAYLOAD: self._handle_payload,
        }
        return state_handlers[state](data)

    def _handle_header(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.HEADER
        bytes_used = 0
        packet_complete = False

        # Wait for enough data
        if len(data) >= self.HEADER_SIZE:
            self._packet.date_time = obc_time.OBCDateTime.from_timestamp(int.from_bytes(data[:4], "big"))
            self._packet.log_id = int(data[4])
            self._packet.signal_id = int(data[5])
            self._packet.payload_len = int(data[6])
            
            # Parse the log by looking up the corresponding specification
            try:
                group_spec, signal_spec = self._log_specs.get(group_id=self._packet.log_id, signal_id=self._packet.signal_id)
                
                self._packet.group_name = group_spec.name
                self._packet.group_desc = group_spec.desc
                self._packet.signal_level = signal_spec.level
                self._packet.signal_name = signal_spec.name
                self._packet.signal_desc = signal_spec.desc
            except exc.OBCLogNotFoundError as e:
                # Should never happen
                print(f"[OBCLogNotFoundError] {str(e)}")

            # Only continue to PAYLOAD state if there is a payload
            if self._packet.payload_len == 0:
                packet_complete = True
                next_state = self.State.HEADER
            else:
                next_state = self.State.PAYLOAD

            bytes_used = self.HEADER_SIZE

        return (next_state, bytes_used, packet_complete)

    def _handle_payload(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.PAYLOAD
        bytes_used = 0
        packet_complete = False

        # Wait for enough data
        if len(data) >= self._packet.payload_len:
            if self._packet.log_id == LOG_CMD_SYS_SCHED_RESP_LOG_ID:
                self._packet.data = { 'data': data[:self._packet.payload_len] }
            else:
                group_spec, signal_spec = self._log_specs.get(group_id=self._packet.log_id, signal_id=self._packet.signal_id)
                self._packet.data = signal_spec.decode_data(data[:self._packet.payload_len])

            next_state = self.State.HEADER
            bytes_used = self._packet.payload_len
            packet_complete = True

        return (next_state, bytes_used, packet_complete)
