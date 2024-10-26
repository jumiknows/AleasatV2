#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
SA_DIR="$(dirname "$SCRIPT_DIR")"

pushd "$SA_DIR"
poetry run python -m alea.sa
popd
