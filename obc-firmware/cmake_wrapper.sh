#!/bin/bash

start=`date +%s`

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <launchpad|alea-v1>"
    exit 0
fi

# exit when any command fails
set -e

OUTPUT_BASENAME="OBCFirmware_$1"

# Directories
BUILD_DIR=./build/$1
SOURCE_DIR=../../ # Relative to BUILD_DIR
BIN_DIR=./build/_bin

mkdir -p ${BUILD_DIR}
mkdir -p ${BIN_DIR}

# Move to build directory
cd ${BUILD_DIR}

# Run build
echo "Building in ${BUILD_DIR}"
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/ti-cgt-arm.cmake -G "Unix Makefiles" --clean-first -DPLATFORM=$1 ${SOURCE_DIR}
cmake --build .

# Get githash
GITHASH_FULL=$(git rev-parse HEAD)
GITHASH=${GITHASH_FULL:0:8}

# Copy binaries
OUTPUT_PATH=${BIN_DIR}/${OUTPUT_BASENAME}_${GITHASH}.elf
cd $SOURCE_DIR
cp ${BUILD_DIR}/${OUTPUT_BASENAME}.elf ${OUTPUT_PATH}

end=`date +%s`

runtime=$((end-start))

GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo -e "${GREEN}"
echo "================================================================================"
echo "BUILD SUCCEEDED (${runtime} s)"
echo ""
echo "Output Files:"
echo "    - ${OUTPUT_PATH}"
echo "================================================================================"
echo -e "${NC}"