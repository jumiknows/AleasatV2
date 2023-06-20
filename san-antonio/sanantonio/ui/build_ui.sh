#!/bin/bash

# Usage: ./build_ui.sh [UI...]
#
# If no UIs are specified, all UIs will be built.
# Otherwise only the specified UIs will be built.
#
# Examples:
#    ./build_ui.sh
#    ./build_ui.sh obc_serial_log
#    ./build_ui.sh control_panels/cmds_panel

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

gen_python () {
    # Generate the Python code for a given UI
    #
    # Arguments:
    #    ui: Name of the .ui file (including path relative to this script's directory) without the ".ui"
    echo "Building: $1"
    pipenv run pyuic5 $1.ui -o $1_ui.py
}

build_dir () {
    # Iterate over the .ui files in the given directory and generate the Python code for each UI
    #
    # Arguments:
    #    directory (optional): Directory to search for .ui files relative to this script's directory
    #                          (must have trailing slash if provided)
    #                          If omitted, this script's directory will be searched

    for filename in $SCRIPT_DIR/$1*.ui
    do
        ui_name=$(basename "$filename" .ui)
        gen_python "$1$ui_name"
    done
}

build_all () {
    build_dir
    build_dir "control_panels/"
}

if [ "$#" -eq "0" ]; then
    build_all
else
    for i in "$@"
    do
        gen_python $i
    done
fi
