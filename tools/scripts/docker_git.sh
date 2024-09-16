#!/bin/bash

# Since docker mounts the /usr/local/src/repo directory as root
# there is a mismatch in owner between the repo directory and
# everything inside it (owned by the current user). This causes
# git to throw the following error:
#
#    fatal: detected dubious ownership in repository at '/usr/local/src/repo'
#
# Marking the directory as safe in the git config suppresses the error.
git config --global --add safe.directory /usr/local/src/repo

# Run whatever command was passed
"$@"
