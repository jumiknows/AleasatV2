from enum import IntEnum
import struct

from alea.ttc.protocol.generic import packet
from alea.ttc.protocol.generic import layer
from alea.ttc.protocol.generic import layer_impl
from alea.ttc.protocol.generic import routing

from alea.ttc.util import data_utils

class OBCSerialDatagram(packet.Packet):
    class MsgType(IntEnum):
        LOG     = 0
        COMMS   = 1

    MAX_DATA_SIZE = 226

    def __init__(self, msg_type: MsgType, data: bytes):
        super().__init__()
        self.msg_type = msg_type
        self.data = data

    def extract_data(self) -> bytes:
        return self.data

    def serialize(self) -> bytes:
        fmt = f"!B{len(self.data)}s"
        return struct.pack(fmt, self.msg_type, self.data)

    @classmethod
    def deserialize(cls, data: bytes) -> "OBCSerialDatagram":
        return cls(
            OBCSerialDatagram.MsgType(data[0]),
            data[1:]
        )

    def __str__(self) -> str:
        return f"[{self.__class__.__name__}] {self.msg_type.name} {data_utils.bytes_to_hexstr(self.data)}"

def OBCSerialDataLinkRX() -> layer.ProtocolLayer[OBCSerialDatagram]:
    return layer_impl.PacketRXProtocolLayer[OBCSerialDatagram](OBCSerialDatagram)

class OBCSerialDataLinkRXRouter(routing.PacketDest[OBCSerialDatagram]):
    def __init__(self, log_dest: routing.PacketDest[OBCSerialDatagram] = None, comms_dest: routing.PacketDest[OBCSerialDatagram] = None):
        self._log_dest = log_dest
        self._comms_dest = comms_dest

    def set_log_dest(self, dest: routing.PacketDest[OBCSerialDatagram]):
        self._log_dest = dest

    def set_comms_dest(self, dest: routing.PacketDest[OBCSerialDatagram]):
        self._comms_dest = dest

    def write(self, packet_in: OBCSerialDatagram, timeout: float = None):
        if (packet_in.msg_type == OBCSerialDatagram.MsgType.LOG) and (self._log_dest is not None):
            self._log_dest.write(packet_in, timeout=timeout)
        elif (packet_in.msg_type == OBCSerialDatagram.MsgType.COMMS) and (self._comms_dest is not None):
            self._comms_dest.write(packet_in, timeout=timeout)

    def reset(self):
        if self._log_dest is not None:
            self._log_dest.reset()
        if self._comms_dest is not None:
            self._comms_dest.reset()
        super().reset()

class OBCSerialDataLinkTX(layer.ProtocolLayer[OBCSerialDatagram]):
    def __init__(self, msg_type: OBCSerialDatagram.MsgType):
        self._msg_type = msg_type

    def transform_packet(self, packet_in: packet.Packet) -> list[OBCSerialDatagram]:
        return [OBCSerialDatagram(self._msg_type, packet_in.serialize())]
