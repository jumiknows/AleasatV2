#!/bin/bash

OWNER=$(stat -c '%u' ./)

# Check if we should be operating as root. This is absolutely not foolproof, but should cover most relevant cases
if [ "$OWNER" -eq "0" -o "$HOST_UID" -eq "0" ]; then
    if [ "$#" -ne 0 ]; then
        ./tools/docker/docker_git.sh "$@"
    else
        bash
    fi
else
    # The build outputs should be generated under the same user as the user on the host machine
    groupadd -g $HOST_GID dev
    useradd -m -u $HOST_UID -g $HOST_GID dev

    if [ "$#" -ne 0 ]; then
        su dev -c "./tools/docker/docker_git.sh $*"
    else
        su dev
    fi
fi
