#!/bin/bash

# --no-cache required for "apt-get update" to work properly
docker build --no-cache -t obc-fw-build-tools:latest .
