from alea.ttc.device import serial_device
from alea.ttc.protocol.generic import routing_impl
from alea.ttc.protocol import obc_upper_protocol
from alea.ttc.protocol.serial import serial_protocol
from alea.ttc.protocol.comms import comms_phy

from . import ttc_interface

class OBCSerialInterface(ttc_interface.TTCInterface):
    """TTC interface through a serial connection to the OBC.

    This class uses the OBCSerialProtocol to send and receive data to/from ALEASAT over a SerialDevice.
    """

    def __init__(self, upper_protocol: obc_upper_protocol.OBCUpperProtocol):
        # Instantiate Protocols
        self._serial_protocol = serial_protocol.OBCSerialProtocol()
        self._comms_rx_phy = routing_impl.ProtocolPacketDest[comms_phy.CommsFrame](comms_phy.CommsPhyRX())
        self._comms_tx_phy = routing_impl.ProtocolPacketSource[comms_phy.CommsFrame](comms_phy.CommsPhyTX())

        # Connect RX
        self._serial_protocol.set_rx_datalink_comms_dest(self._comms_rx_phy)
        self._comms_rx_phy.set_dest(upper_protocol.rx_comms_dest)

        self._serial_protocol.set_rx_datalink_log_dest(upper_protocol.rx_log_dest)

        # Connect TX
        self._comms_tx_phy.set_src(upper_protocol.tx_comms_src)
        self._serial_protocol.set_tx_datalink_src(self._comms_tx_phy)

        # Connect Serial Device
        self._device = serial_device.SerialDevice(self._serial_protocol.rx_phy, self._serial_protocol.tx_phy)

    @property
    def connected(self) -> bool:
        return self._device.connected

    def start(self, serial_port: str) -> bool:
        """Open a connection to the OBC on the provided serial port.

        This spawns several background threads to manage communication with the OBC.

        Args:
            serial_port: A name of a serial port (e.g. "COM7", "/dev/ttyS0")

        Returns:
            True if the connection was opened successfully, otherwise False.
        """
        self._serial_protocol.reset()
        return self._device.start(serial_port)

    def stop(self):
        self._device.stop()
