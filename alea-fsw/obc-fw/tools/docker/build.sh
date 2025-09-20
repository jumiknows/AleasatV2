#!/bin/bash

# The TI compiler is only available for AMD64 (x86_64) architecture --platform=linux/amd64
docker buildx build --platform linux/amd64 -t alea-obc-fw-build:latest --load .
