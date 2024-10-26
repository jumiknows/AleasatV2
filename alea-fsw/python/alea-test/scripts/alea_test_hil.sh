#!/bin/bash

set -ex

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
ALEA_TEST_DIR="$(dirname "$SCRIPT_DIR")"
RESULTS_DIR="${ALEA_TEST_DIR}/results"

echo "Running all HIL tests. Results will be generated in: ${RESULTS_DIR}"

pushd "$ALEA_TEST_DIR"
poetry run python -m alea.test.hil --output_dir "${RESULTS_DIR}" "$@"
popd
