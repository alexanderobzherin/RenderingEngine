FROM ubuntu:24.04

ENV HOME /root

WORKDIR /rendering-engine

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    git \
    cmake \
    doxygen \
    graphviz \
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
    nlohmann-json3-dev \
    libboost-filesystem-dev