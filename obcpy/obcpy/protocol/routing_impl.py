from typing import Generic, TypeVar, List
from queue import Queue, Full, Empty

from . import packet
from . import layer
from . import routing

AnyPacket = TypeVar("AnyPacket", bound=packet.Packet)

class ProtocolPacketSource(Generic[AnyPacket], routing.PacketSource[AnyPacket]):
    """Implementation of a PacketSource that reads packets from another source
    and transforms them according to a `ProtocolLayer`.
    """

    def __init__(self, protocol_layer: layer.ProtocolLayer[AnyPacket], src: routing.PacketSource[packet.Packet] = None):
        """Initializes a new ProtocolPacketSource.

        Args:
            protocol_layer: A `layer.ProtocolLayer` instance that handles packet transformation.
            src: A PacketSource from which this PacketSource reads packets.
                 If None, no packets will ever be read.
                 Defaults to None.
        """
        self._protocol_layer = protocol_layer
        self._src = src

    def read(self, timeout: float = None) -> List[AnyPacket]:
        all_packets_out: List[AnyPacket] = []

        if self._src is not None:
            packets_in = self._src.read(timeout=timeout)
            for packet_in in packets_in:
                packets_out = self._protocol_layer.transform_packet(packet_in)
                all_packets_out.extend(packets_out)

        return all_packets_out

PacketInType = TypeVar("PacketInType", bound=packet.Packet)
PacketOutType = TypeVar("PacketOutType", bound=packet.Packet)

class ProtocolPacketDest(Generic[PacketInType, PacketOutType], routing.PacketDest[PacketInType]):
    """Implementation of a PacketDest that transforms incoming packets according to a `ProtocolLayer`
    then writes them to another destination.
    """

    def __init__(self, protocol_layer: layer.ProtocolLayer[PacketOutType], dest: routing.PacketDest[PacketOutType] = None):
        """Initializes a new ProtocolPacketDest.

        Args:
            protocol_layer: A `layer.ProtocolLayer` instance that handles packet transformation.
            dest: A PacketDest to which this PacketDest writes packets.
                  If None, all written packets are dropped.
                  Defaults to None.
        """
        self._protocol_layer = protocol_layer
        self._dest = dest

    def write(self, packet_in: PacketInType, timeout: float = None):
        packets_out = self._protocol_layer.transform_packet(packet_in)
        self._write_packets(packets_out, timeout=timeout)

    def flush(self, timeout: float = None):
        packets = self._protocol_layer.flush()
        self._write_packets(packets, timeout=timeout)

    def _write_packets(self, packets: List[PacketOutType], timeout: float = None):
        if self._dest is not None:
            for packet_out in packets:
                self._dest.write(packet_out, timeout=timeout)

class MultiPacketDest(Generic[AnyPacket], routing.PacketDest[AnyPacket]):
    """Implementation of a PacketDest that writes every incoming packet to multiple destinations.
    """

    def __init__(self, *dests: routing.PacketDest[AnyPacket]):
        """Initializes a new SplitPacketDest with the provided list of destinations (if any)
        """
        self._dests = list(dests)

    def add_dest(self, *dests: routing.PacketDest[AnyPacket]):
        """Adds a new destination (or multiple).
        """
        self._dests.extend(dests)

    def write(self, packet_in: AnyPacket, timeout: float = None):
        for dest in self._dests:
            dest.write(packet_in, timeout=timeout)

    def flush(self, timeout: float = None):
        for dest in self._dests:
            dest.flush(timeout=timeout)

class FilterPacketDest(Generic[AnyPacket], routing.PacketDest[AnyPacket]):
    """Base class for implementing a PacketDest that selects a destination based on the packet.

    The filter operation can be implemented by overriding `get_packet_dest`.
    By default all packets are filtered out (discarded).
    """

    def get_packet_dest(self, packet_in: AnyPacket) -> routing.PacketDest[AnyPacket]:
        """Selects a destination for the given packet.

        Args:
            packet_in: A packet to be routed.

        Returns:
            The destination for the given packet or None if the packet should be dropped.
        """
        return None

    def write(self, packet_in: AnyPacket, timeout: float = None):
        dest = self.get_packet_dest(packet_in)
        if dest is not None:
            dest.write(packet_in, timeout=timeout)

class PacketToStreamSource(routing.StreamSource):
    """Implementation of a StreamSource that converts incoming discrete raw packets into a continuous stream.
    """

    def __init__(self, src: routing.PacketSource[packet.RawPacket]):
        """Initializes a new PacketToStreamSource that reads from the provided PacketSource.

        Args:
            src: A PacketSource from which this PacketToStreamSource reads packets.
        """
        self._src = src

        self._buffer = bytearray()

    def read(self, num_bytes: int, timeout: float = None) -> bytes:
        out_bytes = bytearray()

        while len(out_bytes) < num_bytes:
            buf_len = len(self._buffer)
            if buf_len > 0:
                # Use internal buffer first
                # Consume the smaller of: 1) the data available OR 2) the number of bytes we still need
                bytes_to_read = min(buf_len, (num_bytes - len(out_bytes)))
                out_bytes.extend(self._read_buffer(bytes_to_read))
            else:
                # Read more packets from the source
                packets_in = self._src.read(timeout=timeout) # TODO ALEA-841 timeout should be used for the overall operation

                if len(packets_in) == 0:
                    # If we got no packets then we timed out so abort
                    break

                # Fill up internal buffer for use on the next loop
                for packet_in in packets_in:
                    packet_data = packet_in.data
                    self._buffer.extend(packet_data)

        return out_bytes

    def _read_buffer(self, num_bytes: int) -> bytes:
        data = self._buffer[:num_bytes]
        del self._buffer[:num_bytes]
        return data

class QueuePacketBridge(Generic[AnyPacket], routing.PacketSource[AnyPacket], routing.PacketDest[AnyPacket]):
    """Bridge-like class that can be used at any level of a protocol stack to reverse the read/write behaviour
    by storing data in a Queue (which also provides thread-safety).

    An example setup for a protocol stack that is operated asynchronously at both ends is to have two independent
    stacks (RX and TX) and two threads per stack.

    In the TX stack:
        - App thread writes to the App layer of the stack
        - Phy thread reads from the Phy layer of the stack

    In the RX stack:
        - Phy thread writes to the Phy layer of the stack
        - App thread reads from the App layer of the stack

    In both cases, we need to switch reads to writes or writes to reads somewhere along the stack which can be
    accomplished by a Queue (inside a QueuePacketBridge) which acts as both a PacketSource and a PacketDest.
    """

    def __init__(self, maxsize: int = 0, log_tag: str = None):
        """Initializes a new QueuePacketBridge.

        Args:
            maxsize: Maximum size of the internal queue or 0 for an infinite queue. Defaults to 0.
            log_tag: If this is provided all packets written to this object will be logged (via print)
                     and prefixed with this tag.
                     Defaults to None.
        """

        self._q = Queue(maxsize=maxsize)
        self._log_tag = log_tag

    def read(self, timeout: float = None) -> List[AnyPacket]:
        try:
            return [self._q.get(block=True, timeout=timeout)]
        except Empty:
            return []

    def write(self, packet_in: AnyPacket, timeout: float = None):
        if self._log_tag is not None:
            print(f"[{self._log_tag}] {packet_in}")

        try:
            # TODO ALEA-841 handle errors
            self._q.put(packet_in, block=True, timeout=timeout)
        except Full:
            pass
