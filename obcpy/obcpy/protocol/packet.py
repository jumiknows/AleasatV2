from abc import ABC, abstractmethod

from obcpy.utils import data as data_utils

class Packet(ABC):
    """Abstract base class for representing a packet.

    The fundamental properties of a packet are as follows:
      1. A packet can be serialized to a bytes object (`Packet.serialize()`)
      2. A packet may have a "data"-like field that contains raw data or the serialized bytes
      of another packet (`Packet.extract_data()`)
      3. A packet can be deserialized from a bytes object (`Packet.deserialize(data)`)

    Each of these properties corresponds to an abstract method defined in this class
    as indicated in the brackets.

    These properties allow packets to be abstractly passed up and down a protocol stack:
      - Down the stack: packets are repeatedly serialized and wrapped in additional headers/footers
      - Up the stack: packets have their "data" field extracted and the data is deserialized into a new packet
    """

    @abstractmethod
    def serialize(self) -> bytes:
        """Converts a packet into a bytes object such that it could be embedded inside another packet
        or passed directly to an application or hardware.

        Returns:
            The serialized packet.
        """
        pass

    @abstractmethod
    def extract_data(self) -> bytes:
        """If this packet has a "data" field, this method extracts the contents of that field to be
        passed to a higher layer in the protocol stack.

        Returns:
            The extracted packet data.
        """
        pass

    @classmethod
    @abstractmethod
    def deserialize(cls, data: bytes) -> "Packet":
        """Converts a bytes object into an instance of this class.

        Args:
            data: The serialized packet bytes

        Returns:
            An instance of this class deserialized from the packet data
        """
        pass

    def __str__(self) -> str:
        """Converts this Packet to a string representation by serializing it
        and converting the bytes to a hex string.

        Subclasses should override this method to provide a more useful string
        representation.
        """
        return f"[{self.__class__.__name__}] {data_utils.bytes_to_hexstr(self.serialize())}"

class RawPacket(Packet):
    """The simplest type of Packet: raw data.

    This packet has no headers/footers and all three packet operations are essentially NOPs.

    Attributes:
        data (readonly): The raw packet data
    """

    def __init__(self, data: bytes):
        """Initializes a RawPacket with the provided data bytes.

        Args:
            data: The bytes of the RawPacket.
        """
        self._data = data

    @property
    def data(self) -> bytes:
        """Raw packet bytes
        """
        return self._data

    def extract_data(self) -> bytes:
        return self._data

    def serialize(self) -> bytes:
        return self._data

    @classmethod
    def deserialize(cls, data: bytes) -> "RawPacket":
        return cls(data)

class PacketError(Exception):
    pass
