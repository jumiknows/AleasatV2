# syntax=docker/dockerfile:1

FROM ubuntu:22.04

# Update and install packages
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive TZ=Canada/Pacific apt-get install --no-install-recommends -y \
        wget \
        build-essential \
        ca-certificates \
        cmake \
        make \
        git \
        ruby-full \
        astyle \
        python3.10 \
        python3-pip

# Download and install the TI ARM Code Generation Tools (i.e. compiler toochain)
ARG COMPILER_VERSION="20.2.6.LTS"
ARG COMPILER_INSTALL_FILE="ti_cgt_tms470_${COMPILER_VERSION}_linux-x64_installer.bin"
ARG COMPILER_DL_URL="https://software-dl.ti.com/codegen/esd/cgt_public_sw/TMS470/${COMPILER_VERSION}/${COMPILER_INSTALL_FILE}"

WORKDIR /opt/ti
RUN wget ${COMPILER_DL_URL} \
    && chmod +x ${COMPILER_INSTALL_FILE} \
    && ./${COMPILER_INSTALL_FILE}

ENV TI_CGT_ARM_DIR="/opt/ti/ti-cgt-arm_${COMPILER_VERSION}"

# Install ceedling and gcovr for running unit tests
RUN gem install ceedling -v 0.31.1 \
    && python3.10 -m pip install gcovr

# Python Environment Configuration

# Disable generating .pyc files (not necessary since the container process only runs once)
ENV PYTHONDONTWRITEBYTECODE=1
# Disable buffering of output streams so we get all logs in real-time through docker
ENV PYTHONUNBUFFERED=1

# Configure Poetry
ENV POETRY_VERSION=1.8.3
ENV POETRY_HOME=/opt/poetry
ENV POETRY_NO_INTERACTION=1
ENV POETRY_VIRTUALENVS_IN_PROJECT=1
ENV POETRY_VIRTUALENVS_CREATE=1

# Set a default location for the POETRY_CACHE_DIR.
# If you want to take advantage of caching in GitLab CI/CD this should be overriden
# by GitLab to be a location within the project directory and the CI cache feature
# should be enabled.
ENV POETRY_CACHE_DIR=/opt/.cache

# Install poetry
RUN python3.10 -m pip install "poetry==${POETRY_VERSION}"

WORKDIR /
