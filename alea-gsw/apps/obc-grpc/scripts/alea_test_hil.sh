#!/bin/bash

set -ex

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
OBC_GRPC_DIR="$(dirname "$SCRIPT_DIR")"
RESULTS_DIR="${OBC_GRPC_DIR}/results"

echo "Running all HIL tests. Results will be generated in: ${RESULTS_DIR}"

pushd "$OBC_GRPC_DIR"
ALEA_OBC_PORT=localhost:50051 poetry run python -m tests.hil.main --output_dir "${RESULTS_DIR}" "$@"
popd
