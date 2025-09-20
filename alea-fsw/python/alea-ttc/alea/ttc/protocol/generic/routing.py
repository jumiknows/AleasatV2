from typing import Generic, TypeVar
from abc import ABC, abstractmethod

from . import packet

AnyPacket = TypeVar("AnyPacket", bound=packet.Packet)

class PacketSource(ABC, Generic[AnyPacket]):
    """Abstract base class for any object that acts as a source of packets.

    You can call the `PacketSource.read()` method to retrieve packets from this source.
    """

    @abstractmethod
    def read(self, timeout: float = None) -> list[AnyPacket]:
        """Reads packets from the source.

        Args:
            timeout: If None, this method will block until it can return at least one packet.
                     Otherwise this method will block until it can return at least one packet
                     up to maximum of `timeout` seconds.
                     Defaults to None.

        Returns:
            A list of packets from this source.
        """
        pass

    def reset(self):
        """Reset any internal state. Default implementation does nothing.
        """
        pass

class PacketDest(ABC, Generic[AnyPacket]):
    """Abstract base class for any object that acts as a destination for packets.

    You can call the `PacketSource.write()` method to send packets to this destination.
    """

    @abstractmethod
    def write(self, packet_in: AnyPacket, timeout: float = None):
        """Writes a packet to this destination.

        Args:
            packet_in: A packet to write.
            timeout: If None, this method will block until it can write the packet to this destination.
                     Otherwise this method will block up to maximum of `timeout` seconds.
                     Defaults to None.
        """
        pass

    def flush(self, timeout: float = None):
        """If this destination implements buffering, this method flushes all buffered packets
        to this destination.

        Args:
            timeout: If None, this method will block until it can write the packets to this destination.
                     Otherwise this method will block up to maximum of `timeout` seconds.
                     Defaults to None.
        """
        pass

    def reset(self):
        """Reset any internal state. Default implementation does nothing.
        """
        pass

class StreamSource(ABC):
    """Abstract base class for any object that acts as a source of a stream of bytes.
    """

    @abstractmethod
    def read(self, num_bytes: int, timeout: float = None) -> bytes:
        """Reads up to `num_bytes` from the stream.

        Args:
            num_bytes: The number of bytes requested.
            timeout: If None, this method will block until it has read num_bytes from the stream.
                     Otherwise this method will block up to maximum of `timeout` seconds.
                     Defaults to None.

        Returns:
            The bytes read from the stream up to a maximum of num_bytes.
            Fewer than num_bytes may be returned if the operation timed-out.
        """
        pass

    def reset(self):
        """Reset any internal state. Default implementation does nothing.
        """
        pass
