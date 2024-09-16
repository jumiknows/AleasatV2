#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
WRAPPER_DIR=$(dirname ${SCRIPT_DIR})

${WRAPPER_DIR}/tools/scripts/docker_wrapper.sh -d obc-fw-build-tools:latest ceedling gcov:all utils:gcov
