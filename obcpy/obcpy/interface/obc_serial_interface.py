from obcpy.device import serial
from obcpy.obc_protocol import obc_serial_protocol
from obcpy.log_sys import log_spec

class OBCSerialInterface:
    """Serial interface to the OBC.

    This class uses the OBCSerialProtocol to send and receive data to/from the OBC.
    """

    def __init__(self, log_specs : log_spec.OBCLogGroupSpecs):
        self.protocol = obc_serial_protocol.OBCSerialProtocol(log_specs)

        # Serial Device
        self._device = serial.SerialDevice(self.protocol.rx_phy, self.protocol.tx_phy)

    @property
    def connected(self) -> bool:
        return self._device.connected

    def start(self, serial_port: str) -> bool:
        return self._device.start(serial_port)

    def stop(self):
        self._device.stop()
