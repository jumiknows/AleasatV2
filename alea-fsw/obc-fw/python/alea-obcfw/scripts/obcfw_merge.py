#!/usr/bin/env python

import argparse
from pathlib import Path

from alea.obcfw.bin_utils import srec

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', dest='input_files', action='store', nargs='+', required=True, help='Input s-record files (.mot)')
    parser.add_argument('-o', dest='output_file', action='store', required=True, help='Output s-record file (.mot)')

    args = parser.parse_args()

    input_paths = [Path(file) for file in args.input_files]
    output_path = Path(args.output_file)

    srec.merge_srecs(input_paths, output_path)

    print(f"Generated output file: {output_path}")
