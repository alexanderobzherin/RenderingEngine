#!/bin/sh

# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.

set -eu

BUILD_MODE="Debug"

if [ "${1:-}" = "--debug" ]; then
    BUILD_MODE="Debug"
elif [ "${1:-}" = "--release" ]; then
    BUILD_MODE="Release"
elif [ -n "${1:-}" ]; then
    echo "Unknown argument: $1"
    exit 1
fi

echo "Build mode:"
echo "$BUILD_MODE"

PROJECT_SOURCE_DIR=$(pwd)

echo "Project source dir:"
echo "$PROJECT_SOURCE_DIR"

ENGINE_ROOT_FOLDER="$PROJECT_SOURCE_DIR"

# Search for the Rendering Engine repository root.
while true; do
    if [ -d "RenderingEngine" ]; then
        ENGINE_ROOT_FOLDER=$(pwd)
        break
    fi

    if [ "$(pwd)" = "/" ]; then
        echo "RenderingEngine not found in parent directories"
        exit 1
    fi

    cd ..
done

ENGINE_INSTALL_DIR="$ENGINE_ROOT_FOLDER/Build/Installed"
ENGINE_CMAKE_DIR="$ENGINE_INSTALL_DIR/RenderingEngine/RenderingLibrary/Library/cmake/RenderingEngine"

echo "Engine source dir:"
echo "$ENGINE_ROOT_FOLDER"

PROJECT_BUILD_DIR="$ENGINE_ROOT_FOLDER/Build/Tests"

echo "Project build dir:"
echo "$PROJECT_BUILD_DIR"

echo "Engine install dir:"
echo "$ENGINE_INSTALL_DIR"

if [ ! -f "$ENGINE_CMAKE_DIR/RenderingEngineConfig.cmake" ]; then
    echo "Rendering Engine CMake package not found:"
    echo "  $ENGINE_CMAKE_DIR/RenderingEngineConfig.cmake"
    echo "Build and install the engine first using:"
    echo "  ./build_engine.sh --engine-only"
    exit 1
fi

rm -rf "$PROJECT_BUILD_DIR"
mkdir -p "$PROJECT_BUILD_DIR"

cmake \
    -S "$PROJECT_SOURCE_DIR" \
    -B "$PROJECT_BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_MODE" \
    -DRenderingEngine_DIR="$ENGINE_CMAKE_DIR"

cmake --build "$PROJECT_BUILD_DIR"

echo "Unit test binaries built successfully."
