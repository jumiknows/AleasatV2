FROM ubuntu:18.04

RUN apt-get update && apt-get install -y \
  wget \
  libc6-i386 \
  libusb-0.1-4 \
  libgconf-2-4 \
  build-essential \
  udev \
  sudo 

RUN wget https://dr-download.ti.com/software-development/software-programming-tool/MD-QeJBJLj8gq/8.8.0/uniflash_sl.8.8.0.4946.run && \
  chmod +x uniflash_sl.8.8.0.4946.run && \
  ./uniflash_sl.8.8.0.4946.run --mode unattended && \
  rm uniflash_sl.8.8.0.4946.run

WORKDIR /root