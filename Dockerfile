FROM ubuntu:22.04

ENV HOME /root

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    git \
    cmake \

    libvulkan-dev \
    vulkan-tools \
    vulkan-validationlayers \
    libglfw3-dev \
    libglm-dev \
    libgtest-dev \
    libjpeg-dev \
    libpng-dev \
    libassimp-dev \
    pkg-config \
    libfreetype-dev \

    libboost-filesystem-dev

