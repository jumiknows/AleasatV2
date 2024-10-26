from alea.ttc.protocol.generic import packet
from alea.ttc.protocol.generic import routing

from . import comms_datalink

class CommsAppProtocol:
    def __init__(self):
        self._tx_dest: routing.PacketDest[packet.RawPacket]
        self._rx_src: routing.PacketSource[comms_datalink.CommsDatagram]

    def set_tx_dest(self, dest: routing.PacketDest[packet.RawPacket]):
        self._tx_dest = dest

    def set_rx_src(self, src: routing.PacketSource[comms_datalink.CommsDatagram]):
        self._rx_src = src

    def send_cmd(self, cmd: comms_datalink.CommsCommand, data: bytes = b'', timeout: float = None):
        packet_data = bytearray()
        packet_data.append(cmd.value)
        packet_data.extend(data)

        self._tx_dest.write(packet.RawPacket(packet_data), timeout=timeout)

    def recv_resp(self, timeout: float = None) -> comms_datalink.CommsDatagram:
        # WARNING packets beyond the first will be discarded
        # The rx_src should be an implementation that returns one packet at a time
        recv_packets = self._rx_src.read(timeout=timeout)
        if len(recv_packets) > 0:
            return recv_packets[0]
        else:
            return None

    def send_cmd_recv_resp(self, cmd: comms_datalink.CommsCommand, data: bytes = b'', timeout: float = None) -> comms_datalink.CommsDatagram:
        self.send_cmd(cmd, data, timeout=timeout)
        return self.recv_resp(timeout=timeout)
