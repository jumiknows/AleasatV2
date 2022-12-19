from typing import List
from enum import IntEnum
import struct

from obcpy.protocol import packet
from obcpy.protocol import layer
from obcpy.protocol import layer_impl
from obcpy.protocol import routing
from obcpy.protocol import routing_impl

from obcpy.utils.data import bytes_to_hexstr

class OBCSerialDatagram(packet.Packet):
    class MsgType(IntEnum):
        LOG     = 0
        COMMS   = 1

    MAX_DATA_SIZE = 233

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
        return f"[{self.__class__.__name__}] {self.msg_type.name} {bytes_to_hexstr(self.data)}"

def OBCSerialDataLinkRX() -> layer.ProtocolLayer[OBCSerialDatagram]:
    return layer_impl.PacketRXProtocolLayer[OBCSerialDatagram](OBCSerialDatagram)

class OBCSerialDataLinkRXDest(routing_impl.FilterPacketDest[OBCSerialDatagram]):
    def __init__(self, log_dest: routing.PacketDest[OBCSerialDatagram], comms_dest: routing.PacketDest[OBCSerialDatagram]):
        self._log_dest = log_dest
        self._comms_dest = comms_dest

    def get_packet_dest(self, packet_in: OBCSerialDatagram) -> routing.PacketDest[OBCSerialDatagram]:
        if packet_in.msg_type == OBCSerialDatagram.MsgType.LOG:
            return self._log_dest
        elif packet_in.msg_type == OBCSerialDatagram.MsgType.COMMS:
            return self._comms_dest
        else:
            return None

class OBCSerialDataLinkTX(layer.ProtocolLayer[OBCSerialDatagram]):
    def __init__(self, msg_type: OBCSerialDatagram.MsgType):
        self._msg_type = msg_type

    def transform_packet(self, packet_in: packet.Packet) -> List[OBCSerialDatagram]:
        return [OBCSerialDatagram(self._msg_type, packet_in.serialize())]
