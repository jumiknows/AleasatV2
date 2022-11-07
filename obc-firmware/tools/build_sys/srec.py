import sys
import argparse

from pathlib import Path

import bincopy

def save_srec(bin: bincopy.BinFile, output_path: Path):
    output_srec = bin.as_srec(number_of_data_bytes=32, address_length_bits=32)
    with open(output_path, "w") as f:
        for line in output_srec.splitlines(keepends=True):
            # CCS doesn't understand all the s-record types that bincopy produces (specifically S5),
            # so only allow these srecord types:
            #   - S0: Header
            #   - S3: Data
            #   - S7: Start Address (Termination)
            if line.startswith("S0") or line.startswith("S3") or line.startswith("S7"):
                f.write(line)

def main():
    # Parse and validate args
    parser = argparse.ArgumentParser()
    parser.add_argument('operation', action='store', choices=['merge'], help='Operation')
    parser.add_argument('-i', dest='input_files', action='store', nargs='+', required='merge' in sys.argv, help='Input s-record files (.mot)')
    parser.add_argument('-o', dest='output_file', action='store', required='merge' in sys.argv, help='Output s-record file (.mot)')

    args = parser.parse_args()

    operation = args.operation
    input_paths = [Path(file) for file in args.input_files]
    output_path = Path(args.output_file)

    bin_file = bincopy.BinFile(input_paths)
    save_srec(bin_file, output_path)

    print(f"Generated output file: {output_path}")

if __name__ == "__main__":
    main()
