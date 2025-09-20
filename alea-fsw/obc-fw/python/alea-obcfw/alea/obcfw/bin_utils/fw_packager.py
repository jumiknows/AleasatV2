from pathlib import Path

import bincopy
import lief

from .fw_header import FWHeader
from . import srec

class FWPackager:
    EXCFLASH_SIZE  = 0x00000040 # Must match FW_MEMMAP_EXCFLASH_SIZE in fw_memmap.h
    FWHEADER_SIZE  = 16

    def package(self, input_mot_path: str | Path, output_mot_path: str | Path, elf_path: str | Path):
        if not isinstance(input_mot_path, Path):
            input_mot_path = Path(input_mot_path)
            elf_path = Path(elf_path)

        if not isinstance(output_mot_path, Path):
            output_mot_path = Path(output_mot_path)

        output_mot_path.parent.mkdir(parents=True, exist_ok=True)

        # Process MOT file
        print(f"Processing input file: {input_mot_path}")

        # Load input file
        bin_file = bincopy.BinFile()
        bin_file.add_srec_file(input_mot_path)

        # Process ELF file
        print(f"Processing input file: {elf_path}")
        elf_file: lief.ELF.Binary = lief.ELF.parse(elf_path)

        # Check for flash exception vectors at the beginning
        if bin_file.segments[0].address == 0:
            # Start CRC after EXCFLASH data from the segment
            if len(bin_file.segments[0].data) <= self.EXCFLASH_SIZE:
                fw_image_start = bin_file.segments[1].address + self.FWHEADER_SIZE
            else:
                fw_image_start = self.EXCFLASH_SIZE + self.FWHEADER_SIZE

            elf_fw_start_segment = elf_file.segments[1]
        else:
            fw_image_start = bin_file.minimum_address + self.FWHEADER_SIZE
            elf_fw_start_segment = elf_file.segments[0]

        # Generate header
        print(f"Calculating CRC over binary image from {hex(fw_image_start)} to {hex(bin_file.maximum_address)}")
        fw_image_binary = bin_file.as_binary(minimum_address=fw_image_start)
        header = FWHeader.generate_header(fw_image_binary)

        print(f"FW Image Size  : {header.size} bytes = {round((header.size / 1024), 2)} KB")
        print(f"FW Image CRC32 : {hex(header.crc32)}")

        header_bytes = header.pack()

        # Add header to original image
        bin_file.add_binary(header_bytes, fw_image_start - self.FWHEADER_SIZE, overwrite=True)

        # Write the new output srecord
        srec.save_srec(bin_file, output_mot_path)
        print(f"Generated output file: {output_mot_path}")

        # An "empty" header with no CRC and size is generated for the ELF file
        # This is because injecting the header into the ELF file and verifying
        # CRC on the OBC has proved to be difficult. The empty header allows the
        # CRC branch checks in the boot and startup firmwares to work when using
        # the ELF file
        #
        # Since the CI pipeline and flight software will be using MOT files rather than
        # ELF files, this is deemed acceptable
        header = FWHeader.generate_header(bytearray())
        header_bytes = header.pack()

        # Add header to original ELF file
        with open(elf_path, "rb+") as file:
            file.seek(elf_fw_start_segment.file_offset)
            file.write(header_bytes)

        print(f"Generated output file: {elf_path}")

        # Write an output binary as well
        output_bin = bin_file.as_binary()
        output_bin_path = output_mot_path.with_name(f"{output_mot_path.stem}.bin")
        with open(output_bin_path, "wb") as f:
            f.write(output_bin)
        print(f"Generated output file: {output_bin_path}")
