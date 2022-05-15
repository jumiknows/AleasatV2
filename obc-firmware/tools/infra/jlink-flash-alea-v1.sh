#!/bin/bash

#
# This file is used by the Gitlab CI to flash an image to the OBC via the command line
#

JLINK_SCRIPT="jlink_script_tmp.jlink"

# parse args
while getopts "f:" opt; do
  case "$opt" in
    f)  elf_file=$OPTARG ;;
  esac
done

if [ -z "$elf_file" ]; then
        printf "Please specify an elf file with -f\n"
        printf "Exiting...\n"
        exit 1
fi

bin_file=${elf_file}.bin

# first, convert elf to bin file:
objcopy -I elf32-little -O binary $elf_file $bin_file

# generate jlink script
printf "erase
loadbin ${bin_file}, 0x0
verifybin ${bin_file}, 0x0
r
g
exit" > $JLINK_SCRIPT

JLinkExe -device TMS570LS0714 -if JTAG -jtagconf -1,-1 -speed 4000 -CommanderScript $JLINK_SCRIPT
