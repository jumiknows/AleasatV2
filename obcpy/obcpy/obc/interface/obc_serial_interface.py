from obcpy.device import serial

from obcpy.protocol import routing_impl

from obcpy.obc.protocol import obc_upper_protocol
from obcpy.obc.protocol.serial import serial_protocol
from obcpy.obc.protocol.comms import comms_phy

class OBCSerialInterface:
    """Serial interface to the OBC.

    This class uses the OBCSerialProtocol to send and receive data to/from the OBC over a SerialDevice.
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
        self._device = serial.SerialDevice(self._serial_protocol.rx_phy, self._serial_protocol.tx_phy)

    @property
    def connected(self) -> bool:
        return self._device.connected
    
    @property
    def protocol(self) -> serial_protocol.OBCSerialProtocol:
        return self._serial_protocol

    def start(self, serial_port: str) -> bool:
        return self._device.start(serial_port)

    def stop(self):
        self._device.stop()
