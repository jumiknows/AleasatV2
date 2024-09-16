#!/bin/bash

SCRIPT_DIR=$(dirname $( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd ))
WRAPPER_DIR=$(dirname ${SCRIPT_DIR})

${WRAPPER_DIR}/tools/scripts/docker_wrapper.sh -d obc-fw-build-tools:latest ../obc-firmware/tools/cmake/cmake_wrapper.sh $@
