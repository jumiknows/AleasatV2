#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
FW_DIR="$(dirname "$SCRIPT_DIR")"

pushd "${FW_DIR}"
ceedling gcov:all utils:gcov
popd
