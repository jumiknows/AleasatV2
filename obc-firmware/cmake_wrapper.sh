#!/bin/bash

PROJECT_NAME="ALEA_OBC"

# exit when any command fails
set -e

start=`date +%s`

################################################################################
# Argument Parsing
################################################################################

# Reference: https://stackoverflow.com/a/29754866

# More safety, by turning some bugs into errors.
# Without `errexit` you don’t need ! and can replace
# ${PIPESTATUS[0]} with a simple $?, but I prefer safety.
set -o errexit -o pipefail -o noclobber -o nounset

# -allow a command to fail with !’s side effect on errexit
# -use return value from ${PIPESTATUS[0]}, because ! hosed $?
! getopt --test > /dev/null 
if [[ ${PIPESTATUS[0]} -ne 4 ]]; then
    echo 'Sorry, `getopt --test` failed in this environment.'
    exit 1
fi

LONGOPTS=standalone,comms-over-serial,verbose
OPTIONS=scv

# -regarding ! and PIPESTATUS see above
# -temporarily store output to be able to check for errors
# -activate quoting/enhanced mode (e.g. by writing out “--options”)
# -pass arguments only via   -- "$@"   to separate them correctly
! PARSED=$(getopt --options=$OPTIONS --longoptions=$LONGOPTS --name "$0" -- "$@")
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then
    # e.g. return value is 1
    #  then getopt has complained about wrong arguments to stdout
    exit 2
fi
# read getopt’s output this way to handle the quoting right:
eval set -- "$PARSED"

STANDALONE=0
COMMS_OVER_SERIAL=0
VERBOSE=0

# now enjoy the options in order and nicely split until we see --
while true; do
    case "$1" in
        -s|--standalone)
            STANDALONE=1
            shift
            ;;
        -c|--comms-over-serial)
            COMMS_OVER_SERIAL=1
            shift
            ;;
        -v|--verbose)
            VERBOSE=1
            shift
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Programming error"
            exit 3
            ;;
    esac
done

# Handle positional arguments
if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <launchpad|alea-v1> <startup|boot|core|ext> <A|B> [-s|--standalone] [-c|--comms-over-serial] [-v|--verbose]"
    exit 4
fi

PLATFORM=$1
TARGET=$2
FLASH_SLOT_NAME=$3

################################################################################

# Setup environment
export PYTHONPATH=../obcpy

# Get githash, tag and dirty status
git update-index --refresh

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
    -DCOMMS_OVER_SERIAL=${COMMS_OVER_SERIAL}      \
    -DVERBOSE=${VERBOSE}                          \
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
