from obcpy.device import serial
from obcpy.obc_protocol import obc_serial_protocol

class OBCSerialInterface:
    """Serial interface to the OBC.

    This class uses the OBCSerialProtocol to send and receive data to/from the OBC.
    """

    def __init__(self):
        self.protocol = obc_serial_protocol.OBCSerialProtocol()

        # Serial Device
        self._device = serial.SerialDevice(self.protocol.rx_phy, self.protocol.tx_phy)

    @property
    def connected(self) -> bool:
        return self._device.connected

    def start(self, serial_port: str) -> bool:
        return self._device.start(serial_port)

    def stop(self):
        self._device.stop()
