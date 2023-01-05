#!/bin/bash

PROJECT_NAME="ALEA_OBC"

# exit when any command fails
set -e

start=`date +%s`

if [[ $# -lt 3 ]]; then
    echo "Usage: $0 <launchpad|alea-v1> <startup|boot|core|ext> <A|B> [--standalone]"
    exit 1
fi

# Setup environment
export PYTHONPATH=../obcpy

# Process arguments
PLATFORM=$1
TARGET=$2
FLASH_SLOT_NAME=$3

if [[ "$4" == "--standalone" ]]; then
    STANDALONE=1
else
    STANDALONE=0
fi

# Get githash, tag and dirty status
GITHASH_FULL=$(git rev-parse HEAD)
GITHASH=${GITHASH_FULL:0:8}

GIT_TAG="$(git describe --exact-match --tags HEAD 2>/dev/null || true | grep -E '^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$' || true)"
if [ -z "$GIT_TAG" ]; then
    VERSION="0.0.0.0"
else
    VERSION=$GIT_TAG
fi

GIT_DIRTY=0
git diff-index --quiet HEAD -- || GIT_DIRTY=$?
if [[ $GIT_DIRTY -eq 0 ]]; then
    GIT_DIRTY=0
else
    GIT_DIRTY=1
fi
echo "GIT_DIRTY=$GIT_DIRTY"

# Directories
BUILD_DIR="./build/${PLATFORM}/${TARGET}/${FLASH_SLOT_NAME}"
SOURCE_DIR="../../../../" # Relative to BUILD_DIR
BIN_DIR="./build/_bin"

mkdir -p "${BUILD_DIR}"
mkdir -p "${BIN_DIR}"

# Move to build directory
cd "${BUILD_DIR}"

# Run build
echo "Building in ${BUILD_DIR}"
cmake                                             \
    -DPROJECT_NAME=${PROJECT_NAME}                \
    -DCMAKE_TOOLCHAIN_FILE=cmake/ti-cgt-arm.cmake \
    -G "Unix Makefiles"                           \
    --clean-first                                 \
    -DPLATFORM=${PLATFORM}                        \
    -DTARGET=${TARGET}                            \
    -DFLASH_SLOT_NAME=${FLASH_SLOT_NAME}          \
    -DGITHASH=0x${GITHASH}                        \
    -DGIT_DIRTY=${GIT_DIRTY}                      \
    -DVERSION=${VERSION}                          \
    -DSTANDALONE=${STANDALONE}                    \
    "${SOURCE_DIR}"

cmake --build .

# Copy binaries
GIT_DIRTY_STR=""
if [[ $GIT_DIRTY -eq 1 ]]; then
    GIT_DIRTY_STR="-dirty"
fi
OUTPUT_PATH="${BIN_DIR}/${PROJECT_NAME}_${PLATFORM}_${TARGET}_${FLASH_SLOT_NAME}_${VERSION}_${GITHASH}${GIT_DIRTY_STR}.mot"
cd "$SOURCE_DIR"
cp "${BUILD_DIR}/${PROJECT_NAME}.mot" "${OUTPUT_PATH}"

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
