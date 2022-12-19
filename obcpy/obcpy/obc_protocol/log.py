from typing import Tuple
from enum import IntEnum

from obcpy.protocol import packet
from obcpy.protocol import layer_impl

from obcpy.utils import obc_time
import obcpy.utils.data

class OBCLog(packet.Packet):
    class Level(IntEnum):
        DEBUG   = 0
        INFO    = 1
        WARNING = 2
        ERROR   = 3

    def __init__(self, date_time: obc_time.OBCDateTime = None, level: Level = None, func_id: int = None, payload: bytes = None):
        super().__init__()
        self.date_time   = date_time
        self.level       = level
        self.func_id     = func_id
        self.payload_len: int = None
        self.payload     = payload

    def extract_data(self) -> bytes:
        raise NotImplementedError()

    def serialize(self) -> bytes:
        data = bytearray()
        data.extend(self.date_time.to_timestamp().to_bytes(4, "big"))
        data.append(self.level.value | ((self.func_id & 0b0000111111) << 2))
        data.append((self.func_id & 0b1111000000) >> 6)
        data.append(len(self.payload))
        data.extend(self.payload)
        return data

    @classmethod
    def deserialize(cls, data: bytes) -> "OBCLog":
        raise NotImplementedError()

    def __str__(self) -> str:
        line = "[LOG] "
        line += f"[{self.level.name:7s}] "
        line += f"[{self.date_time.date_time.strftime('%Y-%m-%d %H:%M:%S')}] "
        line += f"[ID: {self.func_id:03d}] "
        line += obcpy.utils.data.bytes_to_hexstr(self.payload)
        return line

class OBCAppLogRX(layer_impl.StreamToPacketProtocolLayer[OBCLog]):
    class State(IntEnum):
        HEADER  = 0,
        PAYLOAD = 1,

    HEADER_SIZE = 7

    def __init__(self):
        super().__init__(layer_impl.DirectionalProtocolLayer.Direction.RX, self.State.HEADER, OBCLog)

    def _handle_state(self, state: int, data: bytes) -> Tuple[int, int, bool]:
        state_handlers = {
            self.State.HEADER: self._handle_header,
            self.State.PAYLOAD: self._handle_payload,
        }
        return state_handlers[state](data)

    def _handle_header(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.HEADER
        bytes_used = 0

        # Wait for enough data
        if len(data) >= self.HEADER_SIZE:
            self._packet.date_time = obc_time.OBCDateTime.from_timestamp(int.from_bytes(data[:4], "big"))
            self._packet.level = OBCLog.Level(int(data[4] & 0b00000011))
            self._packet.func_id = int(((data[4] & 0b11111100) >> 2) | ((data[5] & 0b00001111) << 6))
            self._packet.payload_len = int(data[6])

            next_state = self.State.PAYLOAD
            bytes_used = self.HEADER_SIZE

        return (next_state, bytes_used, False)

    def _handle_payload(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.PAYLOAD
        bytes_used = 0
        packet_complete = False

        # Wait for enough data
        if len(data) >= self._packet.payload_len:
            self._packet.payload = data[:self._packet.payload_len]
            next_state = self.State.HEADER
            bytes_used = self._packet.payload_len
            packet_complete = True

        return (next_state, bytes_used, packet_complete)
