import struct
import zlib

class FWHeader:
    # > = Big Endian
    FMT = ">IIII"

    # Must match FW_HEADER_MAGIC in fw_memmap.h
    MAGIC = 0xA1EA50BC

    # Value will be overwritten by bootloader when programming the internal flash
    BURN_NUMBER = 0xFFFFFFFF

    def __init__(self, size: int, crc32: int):
        self.size        = size
        self.crc32       = crc32

    def pack(self) -> bytearray:
        # Packing here must match the fw_header_t struct in fw_structs.h
        return struct.pack(self.FMT, self.BURN_NUMBER, self.MAGIC, self.size, self.crc32)

    @staticmethod
    def generate_header(fw_image: bytearray):
        size = len(fw_image)
        crc32 = zlib.crc32(fw_image)
        return FWHeader(size, crc32)
