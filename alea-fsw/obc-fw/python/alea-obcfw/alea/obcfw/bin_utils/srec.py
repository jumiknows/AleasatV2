from pathlib import Path

import bincopy

def save_srec(bin: bincopy.BinFile, output_path: Path):
    output_path.parent.mkdir(parents=True, exist_ok=True)

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

def merge_srecs(input_paths: list[Path], output_path: Path):
    bin_file = bincopy.BinFile(input_paths)
    save_srec(bin_file, output_path)
