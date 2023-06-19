from typing import List, ClassVar
from dataclasses import dataclass
from enum import IntEnum
import struct
import queue
import threading

from obcpy.protocol import packet
from obcpy.protocol import layer
from obcpy.protocol import layer_impl
from obcpy.protocol import routing

class HWID(IntEnum):
    OBC    = 0x7000
    COMMS  = 0x0107
    GROUND = 0x8000

class CommsCommand(IntEnum):
    ACK      = 0x10
    NACK     = 0xFF
    OBC_DATA = 0x60

@dataclass
class CommsDatagram(packet.Packet):
    HEADER_FMT: ClassVar[str] = "<HHHB"

    seq_num: int
    resp: bool
    dest_hwid: int
    src_hwid: int
    cmd_num: int
    data: bytes

    def extract_data(self) -> bytes:
        return self.data

    def serialize(self) -> bytes:
        seq_num = (int(self.resp) << 15) | self.seq_num

        fmt = f"{CommsDatagram.HEADER_FMT}{len(self.data)}s"

        return struct.pack(fmt, seq_num, self.dest_hwid, self.src_hwid, self.cmd_num, self.data)

    @classmethod
    def deserialize(cls, data: bytes) -> "CommsDatagram":
        header_size = struct.calcsize(CommsDatagram.HEADER_FMT)

        seq_num, dest_hwid, src_hwid, cmd_num = struct.unpack(CommsDatagram.HEADER_FMT, data[:header_size])

        # Handle response bit
        mask = (1 << 15)
        resp = (seq_num & mask) != 0
        seq_num = seq_num & (~mask) # Clear resp bit

        extracted_data = data[header_size:]

        return CommsDatagram(seq_num, resp, dest_hwid, src_hwid, cmd_num, extracted_data)

def CommsDataLinkRX() -> layer.ProtocolLayer[CommsDatagram]:
    return layer_impl.PacketRXProtocolLayer[CommsDatagram](CommsDatagram)

class CommsDataLinkTX(layer.ProtocolLayer[CommsDatagram]):
    def __init__(self, src_hwid: HWID, dest_hwid: HWID, cmd_num: CommsCommand):
        self._src_hwid = src_hwid
        self._dest_hwid = dest_hwid
        self._cmd_num = cmd_num

        self._seq_num = 0

    def transform_packet(self, packet_in: packet.Packet) -> List[CommsDatagram]:
        packet_out = CommsDatagram(
                        seq_num   = self._seq_num,
                        resp      = False,
                        dest_hwid = self._dest_hwid.value,
                        src_hwid  = self._src_hwid.value,
                        cmd_num   = self._cmd_num.value,
                        data      = packet_in.serialize()
                    )
        self._seq_num += 1
        return [packet_out]

    def reset(self):
        self._seq_num = 0
        super().reset()

class CommsDataLink(routing.PacketDest[packet.Packet], routing.PacketSource[CommsDatagram]):
    RESP_QUEUE_SIZE = 100 # Allow 100 in flight packets (should be plenty)

    def __init__(self, rx_dest: routing.PacketDest[CommsDatagram] = None, tx_src: routing.PacketSource[packet.Packet] = None):
        self._rx_protocol_layer = CommsDataLinkRX()
        self._rx_dest = rx_dest

        self._tx_protocol_layer = CommsDataLinkTX(HWID.GROUND, HWID.OBC, CommsCommand.OBC_DATA)
        self._tx_src = tx_src

        self._ack_sent = threading.Event()

        self.reset()

    def set_rx_dest(self, dest: routing.PacketDest[CommsDatagram]):
        self._rx_dest = dest

    def set_tx_src(self, src: routing.PacketSource[packet.Packet]):
        self._tx_src = src

    def write(self, packet_in: packet.Packet, timeout: float = None):
        # RX Stack

        packet_out = self._rx_protocol_layer.transform_packet(packet_in)[0] # 1 -> 1 packet transform

        if packet_out.resp:
            # If it's a response, handle it here
            self._handle_response(packet_out)
        else:
            # If it's not a response, send an ACK then pass it to the next layer
            self._send_ack(packet_out)
            if self._rx_dest is not None:
                self._rx_dest.write(packet_out, timeout=timeout)

    def read(self, timeout: float = None) -> List[CommsDatagram]:
        # TX Stack

        # Transmit pending response packets first
        try:
            resp = self._resp_queue.get_nowait()
            self._ack_sent.set()
            return [resp]
        except queue.Empty:
            pass

        # Read new data to transmit from the source
        # TODO Maybe caring about ACKs would be a good idea
        all_packets_out: List[CommsDatagram] = []

        if self._tx_src is not None:
            packets_in = self._tx_src.read(timeout=timeout)
            for packet_in in packets_in:
                packets_out = self._tx_protocol_layer.transform_packet(packet_in)
                all_packets_out.extend(packets_out)

        return all_packets_out

    def reset(self):
        self._rx_protocol_layer.reset()
        self._tx_protocol_layer.reset()

        self._resp_queue = queue.Queue(maxsize=self.RESP_QUEUE_SIZE)

        if self._rx_dest is not None:
            self._rx_dest.reset()
        if self._tx_src is not None:
            self._tx_src.reset()

        self._ack_sent.set()

        super().reset()

    def _handle_response(self, rx_packet: CommsDatagram):
        # Yay we got a response packet, we don't have any error handling for a lack of
        # response packet so I guess we'll just ignore it for now
        pass

    def _send_ack(self, rx_packet: CommsDatagram):
        resp_packet = CommsDatagram(
                        seq_num   = rx_packet.seq_num,
                        resp      = True,
                        dest_hwid = rx_packet.src_hwid, # Swap src and dest for response
                        src_hwid  = rx_packet.dest_hwid, # Swap src and dest for response
                        cmd_num   = CommsCommand.ACK.value,
                        data      = b''
                    )

        self._ack_sent.clear()
        self._resp_queue.put(resp_packet)
        self._ack_sent.wait()
