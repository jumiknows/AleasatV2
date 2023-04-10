from typing import List, Tuple
from enum import IntEnum
import struct

from obcpy.protocol import packet
from obcpy.protocol import layer
from obcpy.protocol import layer_impl

class OBCSerialFrame(packet.Packet):
    SYNC_0_VALUE = 0xAB
    SYNC_1_VALUE = 0xCD

    TEMP_CRC = 0x1234

    def __init__(self, data: bytes = None):
        super().__init__()
        self.data = data

        self.data_len: int = None
        self.crc: int = None

    def serialize(self) -> bytes:
        data_len = len(self.data)
        fmt = f"!BBB{data_len}sH"
        crc = self._calculate_crc()
        return struct.pack(fmt, self.SYNC_0_VALUE, self.SYNC_1_VALUE, data_len, self.data, crc)

    def extract_data(self) -> bytes:
        return self.data

    @classmethod
    def deserialize(cls, data: bytes) -> "OBCSerialFrame":
        raise NotImplementedError()

    def validate(self) -> bool:
        calc_crc = self._calculate_crc()
        return (calc_crc == self.crc)

    def _calculate_crc(self) -> int:
        # TODO ALEA-842 calculate CRC
        return self.TEMP_CRC

class OBCSerialPhyRX(layer_impl.StreamToPacketProtocolLayer[OBCSerialFrame]):
    class State(IntEnum):
        SYNC_0 = 0,
        SYNC_1 = 1,
        HEADER = 2,
        DATA   = 3,
        CRC    = 4,

    MAX_DATA_SIZE = 234

    def __init__(self):
        super().__init__(layer_impl.DirectionalProtocolLayer.Direction.RX, self.State.SYNC_0, OBCSerialFrame)

    def _handle_state(self, state: int, data: bytes) -> Tuple[int, int, bool]:
        state_handlers = [
            self._handle_sync_0,
            self._handle_sync_1,
            self._handle_header,
            self._handle_data,
            self._handle_crc
        ]
        return state_handlers[state](data)

    def _handle_sync_0(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.SYNC_0
        if data[0] == OBCSerialFrame.SYNC_0_VALUE:
            next_state = self.State.SYNC_1
        return (next_state, 1, False)

    def _handle_sync_1(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.SYNC_0
        if data[0] == OBCSerialFrame.SYNC_1_VALUE:
            next_state = self.State.HEADER
        return (next_state, 1, False)

    def _handle_header(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.SYNC_0
        data_len = data[0]
        if (data_len >= 1) and (data_len <= self.MAX_DATA_SIZE):
            self._packet.data_len = data_len
            self._packet.data = bytearray()
            next_state = self.State.DATA
        return (next_state, 1, False)

    def _handle_data(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.DATA
        bytes_used = 0

        # Wait for enough data
        if len(data) >= self._packet.data_len:
            self._packet.data = data[:self._packet.data_len]
            next_state = self.State.CRC
            bytes_used = self._packet.data_len

        return (next_state, bytes_used, False)

    def _handle_crc(self, data: bytes) -> Tuple[State, int, bool]:
        next_state = self.State.CRC
        bytes_used = 0
        packet_complete = False

        # Wait for enough data
        if len(data) >= 2:
            self._packet.crc = struct.unpack("!H", data[:2])[0] # 2-byte CRC
            next_state = self.State.SYNC_0
            bytes_used = 2

            if self._packet.validate():
                packet_complete = True

        return (next_state, bytes_used, packet_complete)

class OBCSerialPhyTX(layer.ProtocolLayer[OBCSerialFrame]):
    def transform_packet(self, packet_in: packet.Packet) -> List[OBCSerialFrame]:
        return [OBCSerialFrame(packet_in.serialize())]
