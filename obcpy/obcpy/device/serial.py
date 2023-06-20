from abc import ABC, abstractmethod
import threading

import serial

from obcpy.protocol import packet
from obcpy.protocol import routing

class SerialWorker(ABC):
    """Abstract base class for serial worker implementations.

    This class handles all the infrastructure around threading, so the subclasses
    only have to implement the "work" method (`SerialWorker.loop`).
    """

    def __init__(self, device: serial.Serial):
        """Initializes a new SerialWorker but does not start it.

        Subclasses must call through to this super constructor.
        
        Args:
            device: A `serial.Serial` instance connected to a serial port.
        """
        self._device = device

        self._stop_event = threading.Event()
        self._thread: threading.Thread = None

    def start(self):
        """Start a new SerialWorker as a background, daemon thread.
        """
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._stop_event.clear()
        self._thread.start()

    def stop(self):
        """Stop a running SerialWorker.

        This method blocks waiting for the thread to stop.
        """
        if self._thread is not None:
            self._stop_event.set()
            self._thread.join()

            self._thread = None

    def _run(self):
        while not self._stop_event.is_set():
            if self._device.is_open:
                try:
                    if not self.loop():
                        break
                except serial.SerialException as e:
                    print(f"[Serial Exception] {str(e)}")

    @abstractmethod
    def loop(self) -> bool:
        """Work function of the SerialWorker to be implemented by subclasses.

        Stopping the SerialWorker thread requires that this method return periodically.
        This means that however long this method spends blocked is the maximum amount of
        time that it may take to stop the thread.

        Returns:
            True if the SerialWorker should continue looping.
            If False is returned, the SerialWorker thread will exit.
        """
        pass

class SerialRXWorker(SerialWorker):
    """Implementation of `SerialWorker` for receiving bytes on the serial port.
    """

    def __init__(self, device: serial.Serial, dest: routing.PacketDest[packet.Packet]):
        """Initializes a new SerialRXWorker that will pass bytes received from the device
        to the dest.

        Args:
            device: A `serial.Serial` instance connected to a serial port.
            dest: A destination for the received bytes/packets.
        """
        super().__init__(device)
        self._dest = dest

    def loop(self) -> bool:
        rx_data = self._device.read()
        if rx_data:
            self._dest.write(packet.RawPacket(rx_data))
        return True

# Not snake case ---> snek sad
class SerialTXWorker(SerialWorker):
    """Implementation of `SerialWorker` for sending bytes on the serial port.
    """

    def __init__(self, device: serial.Serial, src: routing.PacketSource[packet.Packet]):
        """Initializes a new SerialTXWorker that will pass bytes received from the src
        to the device.

        Args:
            device: A `serial.Serial` instance connected to a serial port.
            src: A source for the bytes/packets to send.
        """
        super().__init__(device)
        self._src = src

    def loop(self) -> bool:
        tx_packets = self._src.read(timeout=0.1)
        for tx_packet in tx_packets:
            self._device.write(tx_packet.serialize())
        return True

class SerialDevice:
    """Wrapper around the serial.Serial module that provides a multi-threaded
    interface for reading/writing from/to serial devices.

    When `SerialDevice.start()` is called, two Daemon worker threads are created,
    one for RX and one for TX.

    The RX thread continually waits for bytes from the serial port and sends
    all received bytes (wrapped in `packet.RawPacket` instances) to the
    `routing.PacketDest` provided in the constructor.

    The TX thread listens for packets from the `routing.PacketSource` provided in
    the constructor, serializes each packet (by calling `Packet.serialize()`) and
    sends the bytes out over the serial port.

    Both threads listen in infinite loops but can be cancelled by calling
    `SerialDevice.stop()`

    Attributes:
        connected (readonly): If True the serial device is currently connected.
    """
    def __init__(self, rx_dest: routing.PacketDest[packet.Packet], tx_src: routing.PacketSource[packet.Packet]):
        """Initializes (but does not connect to) a new SerialDevice instance.

        Args:
            rx_dest: A destination for packets received from the serial port
            tx_src: A source for packets to send over the serial port
        """
        self._rx_dest = rx_dest
        self._tx_src = tx_src

        self._device: serial.Serial = None

        self._rx_worker: SerialRXWorker = None
        self._tx_worker: SerialTXWorker = None

    @property
    def connected(self) -> bool:
        """True if the serial device is currently connected.

        Reading / writing will have no effect if the serial device is not connected.
        """
        return (self._device is not None and self._device.is_open)

    def start(self, serial_port: str) -> bool:
        """Connects to a serial port and starts the RX and TX worker threads if the connection
        was successful.

        If this instance is already connected to a serial port, you must call `SerialDevice.stop()`
        first before connecting to a new serial port. You can re-use the same `SerialDevice` instance
        across multiple connect/disconnect cycles.

        Args:
            serial_port: A serial port name (e.g. "/dev/ttyS0" on Linux or "COM7" on Windows)

        Returns:
            True if the connection was successful, otherwise False.
        """
        if self.connected:
            # Must disconnect first
            return False

        if not self._connect(serial_port):
            # Failed to connect
            return False

        if self._rx_dest is not None:
            print("Starting serial RX worker")
            self._rx_worker = SerialRXWorker(self._device, self._rx_dest)
            self._rx_worker.start()

        if self._tx_src is not None:
            print("Starting serial TX worker")
            self._tx_worker = SerialTXWorker(self._device, self._tx_src)
            self._tx_worker.start()

        return True

    def stop(self):
        """Stops the RX and TX worker threads and disconnect from the serial port.

        This method blocks waiting for the RX and TX worker threads to stop
        which should take a maximum of 0.1 seconds per thread.
        """
        if self._rx_worker is not None:
            print("Stopping serial RX worker")
            self._rx_worker.stop()
            self._rx_worker = None

        if self._tx_worker is not None:
            print("Stopping serial TX worker")
            self._tx_worker.stop()
            self._tx_worker = None

        self._disconnect()

    def _connect(self, serial_port: str, baud_rate: int = 115200, parity: str = serial.PARITY_NONE, stop_bits: int = serial.STOPBITS_ONE, byte_size: int = serial.EIGHTBITS) -> bool:
        try:
            print(f"Connecting to serial device on port: {serial_port}")
            device = serial.Serial(
                port     = serial_port,
                baudrate = baud_rate,
                timeout  = 1,
                parity   = parity,
                stopbits = stop_bits,
                bytesize = byte_size,
            )
            if not device.is_open:
                device.open()

            device.reset_output_buffer()
            device.reset_input_buffer()

            self._device = device
            return True
        except serial.SerialException as e:
            print(f"[Serial Connect Exception] {str(e)}")
            return False

    def _disconnect(self):
        if self.connected:
            print(f"Disconnecting from serial device")
            try:
                self._device.close()
                self._device = None
            except serial.SerialException as e:
                print(f"[Serial Disconnect Exception] {str(e)}")
