#!/bin/bash

#
# This file is used by the Gitlab CI to flash an image to the OBC via the command line
#

JLINK_SCRIPT="jlink_script_tmp.jlink"

# parse args
while getopts "f:" opt; do
  case "$opt" in
    f)  mot_file=$OPTARG ;;
  esac
done

if [ -z "$mot_file" ]; then
    printf "Please specify a mot file with -f\n"
    printf "Exiting...\n"
    exit 1
fi

# generate jlink script
printf "erase
loadfile ${mot_file}
r
g
exit" > $JLINK_SCRIPT

JLinkExe -device TMS570LS0714 -if JTAG -jtagconf -1,-1 -speed 4000 -CommanderScript $JLINK_SCRIPT
