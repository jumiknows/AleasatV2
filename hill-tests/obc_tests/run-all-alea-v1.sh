#!/bin/bash

#
# run-all.sh
#
# This script runs all supported python unit tests. Whenever a new
# test or test suite is added, make sure to include it here or the
# CI will not run the test
#

# to add a new test, append the file name to the end of this command
python3 -m unittest pingTest systemTest
