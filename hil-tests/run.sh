#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

RESULTS_DIR="${SCRIPT_DIR}/results"

echo "Running all tests. Results will be generated in: ${RESULTS_DIR}"

python obc_tests/main.py --output_dir "${RESULTS_DIR}"
