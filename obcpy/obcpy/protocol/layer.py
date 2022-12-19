from typing import Generic, TypeVar, List
from abc import ABC, abstractmethod
from enum import Enum

from . import packet

AnyPacket = TypeVar("AnyPacket", bound=packet.Packet)

class ProtocolLayer(Generic[AnyPacket], ABC):
    """Abstract base class for implementations of "protocol layers".

    A "protocol layer" can be thought of as one of the layers in an n-layer network model.
    More concretely, a protocol layer has one responsibility, which is to transform input packets
    into packets of type `AnyPacket`.

    Typically, a ProtocolLayer operates differently in the TX and RX direction:
      - In the TX direction, it accepts a packet from a higher layer and transforms it by wrapping
        the packet in headers/footers
      - In the RX direction, it accepts a packet from a lower layer and transforms it by extracting
        the data and parsing it

    In either direction a ProtocolLayer always produces packets of type `AnyPacket`. However, the input packet
    type can be anything as this layer may be inserted anywhere into a protocol stack.

    The abstract methods defined by `packet.Packet` allow a ProtocolLayer to handle any incoming packet
    since the packet itself know how to serialize itself into bytes for TX (`Packet.serialize()`) or
    extract its own "data" field for TX (`Packet.extract_data()`).
    """

    @abstractmethod
    def transform_packet(self, packet_in: packet.Packet) -> List[AnyPacket]:
        """Take a single input packet and convert it to one or more packets of type `AnyPacket`.

        Args:
            packet_in: An incoming packet.

        Returns:
            A list of transformed packets. If there are no packets ready (due to buffering etc.),
            an empty list should be returned. None should never be returned.
        """
        pass

    def flush(self) -> List[AnyPacket]:
        """If this ProtocolLayer implements any form of buffering, this method flushes the internal
        buffer into a collection of packets.

        The default flush implementation does nothing, so subclasses that employ buffering should override
        this method.

        Returns:
            A list of packets flushed from the internal buffer.
        """
        return []
