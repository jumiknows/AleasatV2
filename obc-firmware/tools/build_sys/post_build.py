import argparse
from pathlib import Path

import bincopy

from fw_header import FWHeader
import srec

EXCFLASH_SIZE  = 0x00000040 # Must match FW_MEMMAP_EXCFLASH_SIZE in fw_memmap.h

def main():
    # Parse and validate args
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', dest='input_file', action='store', required=True, help='Input s-record file (.mot)')
    parser.add_argument('-o', dest='output_file', action='store', required=True, help='Output s-record file (.mot)')

    args = parser.parse_args()
    input_path = Path(args.input_file)
    output_path = Path(args.output_file)

    print(f"Processing input file: {input_path}")

    # Load input file
    bin_file = bincopy.BinFile()
    bin_file.add_srec_file(input_path)

    # Check for flash exception vectors at the beginning
    if bin_file.segments[0].address == 0:
        # Start CRC after EXCFLASH data from the segment
        if len(bin_file.segments[0].data) <= EXCFLASH_SIZE:
            fw_image_start = bin_file.segments[1].address
        else:
            fw_image_start = EXCFLASH_SIZE
    else:
        fw_image_start = bin_file.minimum_address

    # Generate header
    print(f"Calculating CRC over binary image from {hex(fw_image_start)} to {hex(bin_file.maximum_address)}")
    fw_image_binary = bin_file.as_binary(minimum_address=fw_image_start)
    header = FWHeader.generate_header(fw_image_binary)

    print(f"FW Image Size  : {header.size} bytes = {round((header.size / 1024), 2)} KB")
    print(f"FW Image CRC32 : {hex(header.crc32)}")

    header_bytes = header.pack()

    # Add header to original image
    bin_file.add_binary(header_bytes, (fw_image_start - len(header_bytes)))

    # Write the new output srecord
    srec.save_srec(bin_file, output_path)
    print(f"Generated output file: {output_path}")

    # Write an output binary as well
    output_bin = bin_file.as_binary()
    output_bin_path = output_path.with_name(f"{output_path.stem}.bin")
    with open(output_bin_path, "wb") as f:
        f.write(output_bin)
    print(f"Generated output file: {output_bin_path}")

if __name__ == '__main__':
    main()
