from typing import ClassVar, Tuple, List
from dataclasses import dataclass
from enum import IntEnum
import struct

from obcpy.protocol import packet
from obcpy.protocol import layer
from obcpy.protocol import layer_impl

@dataclass
class CommsFrame(packet.Packet):
    HEADER_FMT: ClassVar[str] = "<BBB"

    ESP_0: ClassVar[int] = 0x22
    ESP_1: ClassVar[int] = 0x69

    data: bytes = None

    local_len: int = 0

    def __post_init__(self):
        if self.data is not None:
            self.local_len = len(self.data)

    def extract_data(self) -> bytes:
        return self.data

    def serialize(self) -> bytes:
        local_len = len(self.data)

        fmt = f"{CommsFrame.HEADER_FMT}{local_len}s"

        return struct.pack(fmt, CommsFrame.ESP_0, CommsFrame.ESP_1, local_len, self.data)

    @classmethod
    def deserialize(cls, data: bytes) -> "CommsFrame":
        header_size = struct.calcsize(CommsFrame.HEADER_FMT)

        esp0, esp1, local_len = struct.unpack(CommsFrame.HEADER_FMT, data[:header_size])

        if not ((esp0 == CommsFrame.ESP_0) and (esp1 == CommsFrame.ESP_1)):
            raise packet.PacketError(f"Invalid start bytes: 0x{esp0:02x}, 0x{esp1:02x}")

        actual_data_len = (len(data) - header_size)
        if local_len != actual_data_len:
            raise packet.PacketError(f"Invalid local length: {local_len} (received {actual_data_len} bytes)")

        extracted_data = data[header_size:]

        return CommsFrame(data=extracted_data)

class CommsPhyRX(layer_impl.StreamToPacketProtocolLayer[CommsFrame]):
    class State(IntEnum):
        ESP_0 = 0,
        ESP_1 = 1,
        HEADER = 2,
        DATA   = 3,

    MAX_DATA_SIZE = 233

    def __init__(self):
        super().__init__(layer_impl.DirectionalProtocolLayer.Direction.RX, self.State.ESP_0, CommsFrame)

    def _handle_state(self, state: int, data: bytes) -> Tuple[int, int, bool]:
        state_handlers = [
            self._handle_esp_0,
            self._handle_esp_1,
            self._handle_header,
            self._handle_data,
        ]
        return state_handlers[state](data)

    def _handle_esp_0(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.ESP_0
        if data[0] == CommsFrame.ESP_0:
            next_state = self.State.ESP_1
        return (next_state, 1, False)

    def _handle_esp_1(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.ESP_0
        if data[0] == CommsFrame.ESP_1:
            next_state = self.State.HEADER
        return (next_state, 1, False)

    def _handle_header(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.ESP_0
        local_len = data[0]
        if (local_len >= 1) and (local_len <= self.MAX_DATA_SIZE):
            self._packet.local_len = local_len
            self._packet.data = bytearray()
            next_state = self.State.DATA
        return (next_state, 1, False)

    def _handle_data(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.DATA
        bytes_used = 0
        packet_complete = False

        # Wait for enough data
        if len(data) >= self._packet.local_len:
            self._packet.data = data[:self._packet.local_len]
            next_state = self.State.ESP_0
            bytes_used = self._packet.local_len
            packet_complete = True

        return (next_state, bytes_used, packet_complete)

class CommsPhyTX(layer.ProtocolLayer[CommsFrame]):
    def transform_packet(self, packet_in: packet.Packet) -> List[CommsFrame]:
        return [CommsFrame(data=packet_in.serialize())]
