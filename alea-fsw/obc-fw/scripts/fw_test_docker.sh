#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
REPO_ROOT_DIR=$(dirname $(dirname $(dirname ${SCRIPT_DIR})))

${REPO_ROOT_DIR}/tools/docker/docker_wrapper.sh -d alea-obc-fw-build:latest ./alea-fsw/obc-fw/scripts/fw_test.sh $@
