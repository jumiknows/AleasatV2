#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
FW_DIR="$(dirname "$SCRIPT_DIR")"

# Move to the alea.obcfw python package folder
PY_ALEA_OBCFW_DIR=${FW_DIR}/python/alea-obcfw
pushd "${PY_ALEA_OBCFW_DIR}"

# Use a cache directory inside the project so we have permission to write to it
export POETRY_CACHE_DIR=${PY_ALEA_OBCFW_DIR}/.cache
export POETRY_VENV_DIR=${PY_ALEA_OBCFW_DIR}/.venv

attempt_setup() {
    # Create virtual environment and install dependencies
    poetry install

    # Activate virtual environment
    source $(poetry env info --path)/bin/activate

    # Check if python command works
    if ! python --version >/dev/null 2>&1; then
        echo "Python command failed after activation."
        return 1
    fi

    return 0
}

# 
# Attempt to set up the virtual environment. Sometimes this can fail and the 
# python command will end up not being recognized, but since the cache
# is still valid further attempts won't try to set up the venv
#
# Simple solution, delete the venv and try a second time, forces poetry to 
# reinstall dependencies
#
if ! attempt_setup; then
    echo "Retrying: Deleting virtual environment and cache."

    # Deactivate if in a subshell (not strictly needed)
    deactivate 2>/dev/null || true

    # Remove the Poetry virtual environment
    poetry env info --path 2>/dev/null | xargs rm -rf

    # Remove cache
    rm -rf "${POETRY_VENV_DIR}"

    # Second attempt
    if ! attempt_setup; then
        echo "Failed to set up the virtual environment after retrying."
        exit 1
    fi
fi

# Restore previous working directory
popd
