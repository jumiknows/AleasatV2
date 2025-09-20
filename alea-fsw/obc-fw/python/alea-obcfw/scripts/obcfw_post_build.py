#!/usr/bin/env python

import argparse

from alea.obcfw.bin_utils.fw_packager import FWPackager

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', dest='input_mot_file', action='store', required=True, help='Input s-record file (.mot)')
    parser.add_argument('-o', dest='output_mot_file', action='store', required=True, help='Output s-record file (.mot)')
    parser.add_argument('-e', dest='elf_file', action='store', required=True, help='ELF file (.elf)')

    args = parser.parse_args()

    packager = FWPackager()
    packager.package(args.input_mot_file, args.output_mot_file, args.elf_file)
