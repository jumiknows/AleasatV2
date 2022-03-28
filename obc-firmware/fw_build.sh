#!/bin/bash -i

# the -i above is required to run this script in interactive mode
# if this script isn't run in interactive mode, it won't be able to attach to the docker container with a tty

MOUNT_DIR=/usr/local/src/repo
FW_DIR=${PWD##*/}

CMD=
if [ "$#" -ne 0 ]; then
    CMD+="./cmake_wrapper.sh $@"
fi

docker run --rm -it                  \
    --user $(id -u):$(id -g)         \
    -v ${PWD}/..:${MOUNT_DIR}        \
    --workdir ${MOUNT_DIR}/${FW_DIR} \
    obc-fw-build-tools:latest        \
    ${CMD}
