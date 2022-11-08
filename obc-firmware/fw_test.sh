#!/bin/bash -i

MOUNT_DIR=/usr/local/src/repo
FW_DIR=${PWD##*/}

docker run --rm -it                  \
    --user $(id -u):$(id -g)         \
    -v ${PWD}/..:${MOUNT_DIR}        \
    --workdir ${MOUNT_DIR}/${FW_DIR} \
    obc-fw-build-tools:latest        \
    ceedling gcov:all utils:gcov
