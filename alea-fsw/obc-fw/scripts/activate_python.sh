#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
FW_DIR="$(dirname "$SCRIPT_DIR")"

# Move to the alea.obcfw python package folder
PY_ALEA_OBCFW_DIR=${FW_DIR}/python/alea-obcfw
pushd "${PY_ALEA_OBCFW_DIR}"

# Use a cache directory inside the project so we have permission to write to it
export POETRY_CACHE_DIR=${PY_ALEA_OBCFW_DIR}/.cache

# Create virtual environment and install dependencies
poetry install

# Activate virtual environment
source $(poetry env info --path)/bin/activate

# Restore previous working directory
popd
