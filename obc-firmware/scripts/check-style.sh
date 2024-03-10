#!/bin/bash

#
# Code style verification tool
#
# To check your code for formatting without modifying any code,
# run with argument "--dry-run", i.e. "./check-style.sh --dry-run"
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

ROOT_DIR="."

INCLUDE_PATH="$ROOT_DIR/main/*.c,*.h"
EXCLUDE_PATH=""

STYLE_OPTIONS="--style=java --indent=spaces=4 --lineend=linux --attach-closing-while --indent-preproc-define --convert-tabs \
                --break-blocks --pad-comma --pad-header --align-pointer=name --add-braces --attach-return-type \
                --max-code-length=150 --suffix=none --pad-oper --unpad-paren"

if astyle "$@" $STYLE_OPTIONS --recursive ${INCLUDE_PATH} --exclude=$EXCLUDE_PATH | grep "Formatted";
then
    echo "Error: code style is incorrect. Please run the "Format code style" task"
    exit 1
else
    echo "Code style is OK."
    exit 0
fi
