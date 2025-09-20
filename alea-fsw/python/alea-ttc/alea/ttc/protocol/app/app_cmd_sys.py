import struct

from alea.common import alea_time

from alea.ttc.protocol.generic import packet
from alea.ttc.protocol.generic import routing
from alea.ttc.protocol.generic import routing_impl

class OBCCmdSysMsgHeader(packet.Packet):
    FMT = f"!BBIHI"
    HEADER_SIZE = 12
    RESP_BIT_MASK = (1 << 31)

    def __init__(self, seq_num: int = None, cmd_id: int = None, date_time: alea_time.ALEADateTime = None, flags: int = None, resp: bool = False, data_len: int = None):
        self.seq_num = seq_num
        self.cmd_id = cmd_id
        self.date_time = date_time
        self.flags = flags
        self.resp = resp
        self.data_len = data_len

    @property
    def uuid(self) -> int:
        # UUID consists of the seq_num, cmd_id, date_time and flags fields encoded as binary then interpreted as a 64-bit integer
        # TODO THIS WILL NOT WORK (a seq_num may be repeated for commands scheduled within the same second if they get scheduled at different times)
        # For now, the (1 << 15) is to provide a 16-bit range of UUIDs for immediate commands
        uuid_bytes = struct.pack("!BBIH", self.seq_num, self.cmd_id, self.date_time.to_timestamp(), self.flags | (1 << 15))
        return struct.unpack("!Q", uuid_bytes)[0]

    def extract_data(self) -> bytes:
        raise NotImplementedError()

    def serialize(self) -> bytes:
        data_len = self.data_len
        if self.resp:
            data_len |= self.RESP_BIT_MASK

        timestamp = 0
        if self.date_time:
            timestamp = self.date_time.to_timestamp()

        return struct.pack(self.FMT, self.seq_num, self.cmd_id, timestamp, self.flags, data_len)

    @classmethod
    def deserialize(self, data: bytes) -> "OBCCmdSysMsgHeader":
        seq_num, cmd_id, timestamp, flags, data_len = struct.unpack(self.FMT, data)
        resp = False if (data_len & self.RESP_BIT_MASK) == 0 else True
        data_len = data_len & ~(self.RESP_BIT_MASK)
        return OBCCmdSysMsgHeader(seq_num, cmd_id, alea_time.ALEADateTime.from_timestamp(timestamp), flags, resp, data_len)

    def __str__(self) -> str:
        return f"{'RESP' if self.resp else 'CMD'} ({self.seq_num}) {self.cmd_id} {self.date_time.date_time.strftime('%Y-%m-%d %H:%M:%S')} - {self.data_len} bytes"

class OBCAppCmdSysSource(routing_impl.PacketToStreamSource):
    def __init__(self, src: routing.PacketSource[packet.RawPacket]):
        super().__init__(src)
        self._header_buffer = bytearray()

    def read_header(self, timeout: float = None) -> OBCCmdSysMsgHeader:
        bytes_to_read = (OBCCmdSysMsgHeader.HEADER_SIZE - len(self._header_buffer))

        if bytes_to_read > 0:
            header_bytes = self.read(bytes_to_read, timeout=timeout)
            self._header_buffer.extend(header_bytes)

        if len(self._header_buffer) == OBCCmdSysMsgHeader.HEADER_SIZE:
            header = OBCCmdSysMsgHeader.deserialize(self._header_buffer)
            del self._header_buffer[:]
            return header
        else:
            return None

class OBCAppCmdSysDest(routing_impl.ProtocolPacketDest[packet.RawPacket]):
    def write_header(self, header: OBCCmdSysMsgHeader, timeout: float = None):
        self.write(header, timeout=timeout)

    def write_data(self, data: bytes, timeout: float = None):
        self.write(packet.RawPacket(data), timeout=timeout)
