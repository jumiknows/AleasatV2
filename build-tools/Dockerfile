# syntax=docker/dockerfile:1
FROM ubuntu:20.04

# Update and install packages
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive TZ=Canada/Pacific apt-get install --no-install-recommends -y \
        wget \
        ca-certificates \
        cmake \
        make \
        git

# Download and install the TI ARM Code Generation Tools (i.e. compiler toochain)
ARG COMPILER_VERSION="20.2.6.LTS"
ARG COMPILER_INSTALL_FILE="ti_cgt_tms470_${COMPILER_VERSION}_linux-x64_installer.bin"
ARG COMPILER_DL_URL="https://software-dl.ti.com/codegen/esd/cgt_public_sw/TMS470/${COMPILER_VERSION}/${COMPILER_INSTALL_FILE}"

WORKDIR /opt/ti
RUN wget ${COMPILER_DL_URL} \
    && chmod +x ${COMPILER_INSTALL_FILE} \
    && ./${COMPILER_INSTALL_FILE}

# Setup build environment variables
ENV OBC_FIRMWARE_DOCKER=1
ENV TI_CGT_ARM_DIR="/opt/ti/ti-cgt-arm_${COMPILER_VERSION}"

WORKDIR /
