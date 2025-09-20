#!/usr/bin/env python

import argparse

from alea.obcfw.bin_utils.fw_packager import FWPackager

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', dest='input_file', action='store', required=True, help='Input s-record file (.mot)')
    parser.add_argument('-o', dest='output_file', action='store', required=True, help='Output s-record file (.mot)')

    args = parser.parse_args()

    packager = FWPackager()
    packager.package(args.input_file, args.output_file)
