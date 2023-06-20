from obcpy.device import serial
from obcpy.protocol import routing_impl
from obcpy.obc.protocol import obc_upper_protocol
from obcpy.obc.protocol.comms import comms_phy as phy

from . import obc_interface

class CommsSerialInterface(obc_interface.OBCInterface):
    """Serial interface to the OBC through a COMMS board.

    This class uses the COMMS PHY to send and receive data to/from the OBC through a COMMS board over a SerialDevice.
    """

    def __init__(self, upper_protocol: obc_upper_protocol.OBCUpperProtocol):
        self._rx_phy = routing_impl.ProtocolPacketDest[phy.CommsFrame](phy.CommsPhyRX(), upper_protocol.rx_comms_dest)
        self._tx_phy = routing_impl.ProtocolPacketSource[phy.CommsFrame](phy.CommsPhyTX(), upper_protocol.tx_comms_src)

        # Connect Serial Device
        self._device = serial.SerialDevice(self._rx_phy, self._tx_phy)

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
        self._rx_phy.reset()
        self._tx_phy.reset()
        return self._device.start(serial_port)

    def stop(self):
        self._device.stop()
