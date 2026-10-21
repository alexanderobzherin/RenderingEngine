
#!/bin/sh

# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.

set -eu

PROJECT_NAME="DrawTextBlocks"
BUILD_MODE="Debug"

case "${1:-}" in
    --debug|"")
        BUILD_MODE="Debug"
        ;;
    --release)
        BUILD_MODE="Release"
        ;;
    *)
        echo "Usage: $0 [--debug|--release]" >&2
        exit 1
        ;;
esac

# Resolve project root independently of the caller's current directory.
PROJECT_SOURCE_DIR=$(
    CDPATH= cd -- "$(dirname -- "$0")" && pwd -P
)

PROJECT_LOCAL_CONFIG="${PROJECT_SOURCE_DIR}/Local/rendering_engine.cmake"

# An explicitly registered project has a standalone build directory.
if [ -f "$PROJECT_LOCAL_CONFIG" ]; then
    PROJECT_BUILD_DIR="${PROJECT_SOURCE_DIR}/Build"
    echo "Registered project configuration found."
else
    # Preserve legacy engine-workspace layout for in-tree applications.
    SEARCH_DIR="$PROJECT_SOURCE_DIR"
    ENGINE_ROOT_FOLDER=""

    while :; do
        if [ -d "${SEARCH_DIR}/RenderingEngine" ] &&
           [ -d "${SEARCH_DIR}/UserApplications" ]; then
            ENGINE_ROOT_FOLDER="$SEARCH_DIR"
            break
        fi

        PARENT_DIR=$(dirname "$SEARCH_DIR")

        if [ "$PARENT_DIR" = "$SEARCH_DIR" ]; then
            break
        fi

        SEARCH_DIR="$PARENT_DIR"
    done

    if [ -n "$ENGINE_ROOT_FOLDER" ]; then
        PROJECT_BUILD_DIR="${ENGINE_ROOT_FOLDER}/Build/UserApplications/${PROJECT_NAME}"
        echo "Rendering Engine workspace found at: $ENGINE_ROOT_FOLDER"
    else
        PROJECT_BUILD_DIR="${PROJECT_SOURCE_DIR}/Build"
        echo "Rendering Engine workspace not found in parent directories."
        echo "Using project-local Build directory."
    fi
fi

echo "Build mode: $BUILD_MODE"
echo "Project source directory: $PROJECT_SOURCE_DIR"
echo "Project build directory: $PROJECT_BUILD_DIR"

# Reconfigure from scratch.
if [ -d "$PROJECT_BUILD_DIR" ]; then
    rm -rf -- "$PROJECT_BUILD_DIR"
fi

# Locate shader compiler through PATH.
if ! command -v glslc >/dev/null 2>&1; then
    echo "ERROR: glslc shader compiler not found in PATH." >&2
    exit 1
fi

SHADER_COMPILER=$(command -v glslc)
echo "Shader compiler: $SHADER_COMPILER"

# Configure project.
cmake -S "$PROJECT_SOURCE_DIR" \
      -B "$PROJECT_BUILD_DIR" \
      -DCMAKE_BUILD_TYPE="$BUILD_MODE"

# Build project.
cmake --build "$PROJECT_BUILD_DIR" --parallel

# Compile built-in shaders.
SHADER_DIR="${PROJECT_BUILD_DIR}/Content/Shaders"

compile_shader()
{
    source_file="$1"
    output_file="$2"

    if [ ! -f "$source_file" ]; then
        echo "ERROR: Missing shader: $source_file" >&2
        exit 1
    fi

    "$SHADER_COMPILER" "$source_file" -o "$output_file"
}

compile_shader "$SHADER_DIR/BasicTexture3D/BasicTexture3D.vert" \
               "$SHADER_DIR/BasicTexture3D/BasicTexture3D_vert.spv"
compile_shader "$SHADER_DIR/BasicTexture3D/BasicTexture3D.frag" \
               "$SHADER_DIR/BasicTexture3D/BasicTexture3D_frag.spv"

compile_shader "$SHADER_DIR/FlatColorFiltering/FlatColorFiltering.vert" \
               "$SHADER_DIR/FlatColorFiltering/FlatColorFiltering_vert.spv"
compile_shader "$SHADER_DIR/FlatColorFiltering/FlatColorFiltering.frag" \
               "$SHADER_DIR/FlatColorFiltering/FlatColorFiltering_frag.spv"

compile_shader "$SHADER_DIR/Quad2D/Quad2D.vert" \
               "$SHADER_DIR/Quad2D/Quad2D_vert.spv"
compile_shader "$SHADER_DIR/Quad2D/Quad2D.frag" \
               "$SHADER_DIR/Quad2D/Quad2D_frag.spv"

compile_shader "$SHADER_DIR/Font2D/Font2D.vert" \
               "$SHADER_DIR/Font2D/Font2D_vert.spv"
compile_shader "$SHADER_DIR/Font2D/Font2D.frag" \
               "$SHADER_DIR/Font2D/Font2D_frag.spv"

compile_shader "$SHADER_DIR/Rectangle2D/Rectangle2D.vert" \
               "$SHADER_DIR/Rectangle2D/Rectangle2D_vert.spv"
compile_shader "$SHADER_DIR/Rectangle2D/Rectangle2D.frag" \
               "$SHADER_DIR/Rectangle2D/Rectangle2D_frag.spv"

echo "Project build completed successfully."
