from obcpy.device import serial

from obcpy import log_sys

from .protocol import serial_protocol

class OBCSerialInterface:
    """Serial interface to the OBC.

    This class uses the OBCSerialProtocol to send and receive data to/from the OBC over a SerialDevice.
    """

    def __init__(self, log_specs: log_sys.log_spec.OBCLogGroupSpecs):
        self.protocol = serial_protocol.OBCSerialProtocol(log_specs)

        # Serial Device
        self._device = serial.SerialDevice(self.protocol.rx_phy, self.protocol.tx_phy)

    @property
    def connected(self) -> bool:
        return self._device.connected
    
    def update_log_specs(self, log_specs: log_sys.log_spec.OBCLogGroupSpecs):
        """Update the log specifications used to parse incoming logs

        Args:
            log_specs: New log specifications
        """
        self.protocol.update_log_specs(log_specs)


    def start(self, serial_port: str) -> bool:
        return self._device.start(serial_port)

    def stop(self):
        self._device.stop()
