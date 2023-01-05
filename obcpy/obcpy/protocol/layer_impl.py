from typing import Generic, TypeVar, Type, List, Tuple
from abc import abstractmethod
from enum import Enum

from . import packet
from . import layer

class PassthroughProtocolLayer(layer.ProtocolLayer[packet.Packet]):
    """Simplest possible implementation of a ProtocolLayer.

    This layer does nothing, it simply passes the packets through untouched.
    """

    def transform_packet(self, packet_in: packet.Packet) -> List[packet.Packet]:
        return [packet_in]

AnyPacket = TypeVar("AnyPacket", bound=packet.Packet)

class PacketRXProtocolLayer(Generic[AnyPacket], layer.ProtocolLayer[AnyPacket]):
    """Simplest implementation of a packet-level protocol layer in the RX direction.

    This layer transforms incoming packets by extracting the data bytes and deserializing
    the bytes to an instance of a provided `packet.Packet` subclass.
    """

    def __init__(self, packet_cls: Type[AnyPacket]):
        """Initializes a PacketRXProtocolLayer to deserialize packets of the provided type

        Args:
            packet_cls: A reference to the `packet.Packet` subclass to use for deserialization.
        """
        self._packet_cls = packet_cls

    def transform_packet(self, packet_in: packet.Packet) -> List[AnyPacket]:
        return [self._packet_cls.deserialize(packet_in.extract_data())]

class DirectionalProtocolLayer(Generic[AnyPacket], layer.ProtocolLayer[AnyPacket]):
    """Abstract base class for ProtocolLayer's that do not themselves prescribe a specific direction,
    but accept a direction parameter at initialization time.

    A convenience method (`ProtocolLayer.packet_to_bytes`) is provided in this class for calling
    either `Packet.serialize()` or `Packet.extract_data()` depending on the ProtocolLayer's direction.
    """

    class Direction(Enum):
        TX = 0
        RX = 1

    def __init__(self, direction: Direction):
        """Initializes a new ProtocolLayer for operation in a particular direction.

        Subclasses must call through to this super constructor.

        Args:
            direction: The direction of the ProtocolLayer.
        """
        self._direction = direction

    def packet_to_bytes(self, packet_in: packet.Packet) -> bytes:
        """Converts an input packet into a bytes object by calling the appropriate method
        on the packet depending on this ProtocolLayer's direction.

        In the TX direction, the packet will be serialized (`Packet.serialize()`).

        In the RX direction, the packet data will be extracted (`Packet.extract_data()`).

        Args:
            packet: An incoming packet.

        Raises:
            ValueError: If this ProtocolLayer does not have a valid direction set.

        Returns:
            The relevant bytes for the packet.
        """
        if self._direction == DirectionalProtocolLayer.Direction.TX:
            # In the TX direction we want to take the entire packet and serialize it
            # before passing it to the next layer
            return packet_in.serialize()
        elif self._direction == DirectionalProtocolLayer.Direction.RX:
            # In the RX direction we want to extract the data from the packet (strip the headers/footers)
            # before passing it to the next layer
            return packet_in.extract_data()
        else:
            raise ValueError("Invalid ProtocolLayer direction")

class RawProtocolLayer(DirectionalProtocolLayer[packet.RawPacket]):
    """Concrete implementation of a DirectionalProtocolLayer that converts all incoming packets
    to RawPackets containing the result of calling `DirectionalProtocolLayer.packet_to_bytes()`
    """

    def transform_packet(self, packet_in: packet.Packet) -> List[packet.RawPacket]:
        return [packet.RawPacket(self.packet_to_bytes(packet_in))]

class RXRawProtocolLayer(RawProtocolLayer):
    """RawProtocolLayer in the RX direction
    """

    def __init__(self):
        super().__init__(DirectionalProtocolLayer.Direction.RX)

class TXRawProtocolLayer(RawProtocolLayer):
    """RawProtocolLayer in the TX direction
    """

    def __init__(self):
        super().__init__(DirectionalProtocolLayer.Direction.TX)

class BufferedProtocolLayer(DirectionalProtocolLayer[packet.RawPacket]):
    """Concrete implementation of a DirectionalProtocolLayer that buffers packets.

    The incoming packets are converted to bytes according to the `direction` parameter.
    The packet bytes are then buffered until enough bytes are collected (`buf_size` parameter).

    Once enough bytes are collected an output packet (of type `packet.RawPacket`) is produced
    and any leftover data remains in the buffer.

    The internal buffer can be flushed to packets at any time by calling `ProtocolLayer.flush()`.
    """

    def __init__(self, direction: DirectionalProtocolLayer.Direction, buf_size: int):
        """Initializes a new BufferedProtocolLayer.

        Args:
            direction: Direction of the protocol layer.
            buf_size: Size of the internal buffer.
        """

        super().__init__(direction)

        self._buf_size = buf_size
        self._buffer = bytearray()

    def transform_packet(self, packet_in: packet.Packet) -> List[packet.RawPacket]:
        data = self.packet_to_bytes(packet_in)
        self._buffer.extend(data)

        packets: List[packet.RawPacket] = []

        # Write in chunks of self._buf_size
        while len(self._buffer) >= self._buf_size:
            packet_data = self._read_buffer(self._buf_size)
            packets.append(packet.RawPacket(packet_data))
        return packets

    def flush(self) -> List[packet.RawPacket]:
        packet_data = self._read_buffer(len(self._buffer))
        flush_packet = packet.RawPacket(packet_data)
        return [flush_packet]

    def _read_buffer(self, num_bytes: int) -> bytes:
        data = self._buffer[:num_bytes]
        del self._buffer[:num_bytes]
        return data

class RXBufferedProtocolLayer(BufferedProtocolLayer):
    """BufferedProtocolLayer for use in the RX direction
    """

    def __init__(self, buf_size: int):
        """Initializes a new RXBufferedProtocolLayer.

        Args:
            buf_size: Size of the internal buffer.
        """
        super().__init__(DirectionalProtocolLayer.Direction.RX, buf_size)

class TXBufferedProtocolLayer(BufferedProtocolLayer):
    """BufferedProtocolLayer for use in the TX direction
    """

    def __init__(self, buf_size: int):
        """Initializes a new TXBufferedProtocolLayer.

        Args:
            buf_size: Size of the internal buffer.
        """
        super().__init__(DirectionalProtocolLayer.Direction.TX, buf_size)

class StreamToPacketProtocolLayer(Generic[AnyPacket], DirectionalProtocolLayer[AnyPacket]):
    """Base class for implementing a DirectionalProtocolLayer that processes packets as they arrive
    in a stream-like manner to re-construct a packet on-the-fly.

    The re-construction of the packet is handled as a state machine where subclasses provided the
    state handlers and state transitions through the `StreamToPacketProtocolLayer._handle_state()`
    method.
    """

    def __init__(self, direction: DirectionalProtocolLayer.Direction, default_state: int, packet_cls: Type[AnyPacket]):
        """Initializes a new StreamToPacketProtocolLayer

        Args:
            direction: Direction of the protocol layer.
            default_state: Starting state for the packet re-construction state machine.
            packet_cls: Packet subclass that this protocol layer assembles.
                        This class must provide a default constructor with no parameters.
        """

        super().__init__(direction)

        self._default_state = default_state
        self._packet_cls = packet_cls

        self._state = self._default_state
        self._packet = self._packet_cls()
        self._buffer = bytearray()

    def transform_packet(self, packet_in: packet.Packet) -> List[AnyPacket]:
        # Add new data to the internal buffer
        self._buffer.extend(self.packet_to_bytes(packet_in))

        packets: List[AnyPacket] = []

        # Process the buffer until:
        #   - there's none left: len(self._buffer) == 0
        #     OR
        #   - we need more data: bytes_used == 0 (returned from _handle_state)
        while len(self._buffer) > 0:
            next_state, bytes_used, packet_complete = self._handle_state(self._state, self._buffer)

            self._state = next_state

            if bytes_used == 0:
                break

            del self._buffer[:bytes_used]

            if packet_complete:
                packets.append(self._packet)
                self._packet = self._packet_cls()

        return packets

    @abstractmethod
    def _handle_state(self, state: int, data: bytes) -> Tuple[int, int, bool]:
        """This method should parse the provided data according the current state
        and return the described state information.

        Args:
            state: The current state of the state machine
            data: The remaining available data bytes

        Returns:
            (new_state, bytes_used, packet_complete)
            new_state: The state machine will transition to this state
            bytes_used: The number of bytes consumed in processing this state
            packet_complete: True if all the necessary data for constructing one packet has been parsed
        """
        pass
