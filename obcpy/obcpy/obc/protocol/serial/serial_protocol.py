from obcpy.protocol import packet
from obcpy.protocol import routing
from obcpy.protocol import routing_impl

from . import serial_datalink as datalink
from . import serial_phy as phy

MAX_DATA_SIZE = datalink.OBCSerialDatagram.MAX_DATA_SIZE

class OBCSerialProtocol:
    """Serial (datalink and PHY) layers of the OBC protocol stack
    """

    def __init__(self):
        """Initializes the protocol stack.
        """
        # RX Stack
        self._rx_datalink_router  = datalink.OBCSerialDataLinkRXRouter()
        self._rx_datalink         = routing_impl.ProtocolPacketDest[datalink.OBCSerialDatagram](datalink.OBCSerialDataLinkRX(), self._rx_datalink_router)
        self._rx_phy              = routing_impl.ProtocolPacketDest[phy.OBCSerialFrame](phy.OBCSerialPhyRX(), self._rx_datalink)

        # TX Stack
        self._tx_datalink = routing_impl.ProtocolPacketSource[datalink.OBCSerialDatagram](datalink.OBCSerialDataLinkTX(datalink.OBCSerialDatagram.MsgType.COMMS))
        self._tx_phy      = routing_impl.ProtocolPacketSource[phy.OBCSerialFrame](phy.OBCSerialPhyTX(), self._tx_datalink)

    def set_rx_datalink_log_dest(self, dest: routing.PacketDest[datalink.OBCSerialDatagram]):
        self._rx_datalink_router.set_log_dest(dest)

    def set_rx_datalink_comms_dest(self, dest: routing.PacketDest[datalink.OBCSerialDatagram]):
        self._rx_datalink_router.set_comms_dest(dest)

    @property
    def rx_phy(self) -> routing.PacketDest[packet.Packet]:
        return self._rx_phy

    def set_tx_datalink_src(self, src: routing.PacketSource[packet.Packet]):
        self._tx_datalink.set_src(src)

    @property
    def tx_phy(self) -> routing.PacketSource[phy.OBCSerialFrame]:
        return self._tx_phy

    def reset(self):
        self.rx_phy.reset()
        self.tx_phy.reset()
