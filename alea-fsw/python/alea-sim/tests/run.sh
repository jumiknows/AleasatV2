#!/bin/bash

set -ex

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
RESULTS_DIR="${SCRIPT_DIR}/results"

echo "Running all ALEASIM tests. Results will be generated in: ${RESULTS_DIR}"

pushd "$SCRIPT_DIR"
poetry run python main.py --output_dir "${RESULTS_DIR}" "$@"
popd
