#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

MOUNT_DIR=/usr/local/src/repo

FW_DIR="$(dirname "$SCRIPT_DIR")"
FW_DIR_NAME=${FW_DIR##*/}
FW_DIR_MOUNT="${MOUNT_DIR}/${FW_DIR_NAME}"

# Extract the docker image name.
while true; do
    case "$1" in
        -d|--docker_image)
            shift
            FW_DOCKER_IMAGE="$1"
            shift
            break
            ;;
        *)
            echo "Please provide docker image name"
            exit 1
            ;;
    esac
done

docker run --rm -it                       \
    --platform linux/amd64                \
    -e HOST_UID=$(id -u)                  \
    -e HOST_GID=$(id -g)                  \
    -e REPO_ROOT=${MOUNT_DIR}             \
    -e FW_DIR=${FW_DIR_MOUNT}             \
    -v ${FW_DIR}/..:${MOUNT_DIR}:z        \
    --workdir ${MOUNT_DIR}/${FW_DIR_NAME} \
    ${FW_DOCKER_IMAGE}                    \
    ${FW_DIR_MOUNT}/docker/docker_user.sh "$@"
