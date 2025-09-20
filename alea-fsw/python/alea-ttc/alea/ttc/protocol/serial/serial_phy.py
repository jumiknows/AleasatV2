from enum import IntEnum
import struct
import logging

from alea.ttc.protocol.generic import packet
from alea.ttc.protocol.generic import layer
from alea.ttc.protocol.generic import layer_impl

logger = logging.getLogger(__name__)

class OBCSerialFrame(packet.Packet):
    SYNC_0_VALUE = 0xAB
    SYNC_1_VALUE = 0xCD

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
        table = (
            0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
            0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
            0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
            0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
            0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
            0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
            0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
            0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
            0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
            0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
            0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
            0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
            0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
            0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
            0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
            0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
            0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
            0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
            0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
            0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
            0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
            0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
            0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
            0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
            0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
            0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
            0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
            0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
            0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
            0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
            0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
            0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
        )
     
        crc = 0xFFFF
        for byte in self.data:
            crc = table[(crc ^ byte) & 0x00FF] ^ (crc >> 8)
            crc &= 0xFFFF
        return crc

class OBCSerialPhyRX(layer_impl.StreamToPacketProtocolLayer[OBCSerialFrame]):
    class State(IntEnum):
        SYNC_0 = 0,
        SYNC_1 = 1,
        HEADER = 2,
        DATA   = 3,
        CRC    = 4,

    MAX_DATA_SIZE = 237

    def __init__(self):
        super().__init__(layer_impl.DirectionalProtocolLayer.Direction.RX, self.State.SYNC_0, OBCSerialFrame)

    def _handle_state(self, state: int, data: bytes) -> tuple[int, int, bool]:
        state_handlers = [
            self._handle_sync_0,
            self._handle_sync_1,
            self._handle_header,
            self._handle_data,
            self._handle_crc
        ]
        return state_handlers[state](data)

    def _handle_sync_0(self, data: bytes) -> tuple[State, int, bool]:
        next_state = self.State.SYNC_0
        if data[0] == OBCSerialFrame.SYNC_0_VALUE:
            next_state = self.State.SYNC_1
        return (next_state, 1, False)

    def _handle_sync_1(self, data: bytes) -> tuple[State, int, bool]:
        next_state = self.State.SYNC_0
        if data[0] == OBCSerialFrame.SYNC_1_VALUE:
            next_state = self.State.HEADER
        return (next_state, 1, False)

    def _handle_header(self, data: bytes) -> tuple[State, int, bool]:
        next_state = self.State.SYNC_0
        data_len = data[0]
        if (data_len >= 1) and (data_len <= self.MAX_DATA_SIZE):
            self._packet.data_len = data_len
            self._packet.data = bytearray()
            next_state = self.State.DATA
        return (next_state, 1, False)

    def _handle_data(self, data: bytes) -> tuple[State, int, bool]:
        next_state = self.State.DATA
        bytes_used = 0

        # Wait for enough data
        if len(data) >= self._packet.data_len:
            self._packet.data = data[:self._packet.data_len]
            next_state = self.State.CRC
            bytes_used = self._packet.data_len

        return (next_state, bytes_used, False)

    def _handle_crc(self, data: bytes) -> tuple[State, int, bool]:
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
            else:
                logger.error("Error: CRC calculated on response data was wrong")

        return (next_state, bytes_used, packet_complete)

class OBCSerialPhyTX(layer.ProtocolLayer[OBCSerialFrame]):
    def transform_packet(self, packet_in: packet.Packet) -> list[OBCSerialFrame]:
        return [OBCSerialFrame(packet_in.serialize())]
