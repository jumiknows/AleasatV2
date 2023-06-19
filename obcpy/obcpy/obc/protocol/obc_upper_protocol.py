from obcpy.protocol import packet
from obcpy.protocol import routing
from obcpy.protocol import routing_impl

from obcpy import log_sys

from .app import app_protocol
from .comms import comms_datalink
from .serial import serial_protocol

class OBCUpperProtocol:
    """Implementation of the upper layers of the OBC Protocol stack which are the same regardless of the
    underlying physical connection to the OBC.

    This includes the application layer, the COMMS datalink layer, and a layer of packet buffers.
    """

    def __init__(self, log_specs: log_sys.log_spec.OBCLogGroupSpecs):
        """Initializes the protocol stack.
        """
        # Instantiate protocols
        self._app = app_protocol.OBCAppProtocol(log_specs, serial_protocol.MAX_DATA_SIZE)
        self._comms_datalink = comms_datalink.CommsDataLink()

        # Instantiate bridges (packet buffers)
        self._tx_bridge = routing_impl.QueuePacketBridge[packet.Packet](maxsize=100)
        self._rx_bridge = routing_impl.QueuePacketBridge[packet.Packet](maxsize=100)

        # Connect RX protocols
        self._app.set_rx_resp_src(self._rx_bridge)
        self._comms_datalink.set_rx_dest(self._rx_bridge)

        # Connect TX protocols
        self._app.set_tx_cmd_dest(self._tx_bridge)
        self._comms_datalink.set_tx_src(self._tx_bridge)

    @property
    def app(self) -> app_protocol.OBCAppProtocol:
        return self._app

    @property
    def rx_comms_dest(self) -> routing.PacketDest[packet.Packet]:
        return self._comms_datalink

    @property
    def rx_log_dest(self) -> routing.PacketDest[packet.Packet]:
        return self.app.rx_log_dest

    @property
    def tx_comms_src(self) -> routing.PacketSource[packet.Packet]:
        return self._comms_datalink
