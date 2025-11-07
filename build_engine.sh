# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2025 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.

# Default options
BUILD_TYPE="Debug"
BUILD_ENGINE_ONLY="OFF"
SOURCE_DIR="$(pwd)"
BUILD_DIR="${SOURCE_DIR}/Build"
DOCS_ONLY=false

# Check for --docs-only argument
BuildDocumentation()
{
    # Generate documentation
    echo "Generating Doxygen documentation only..."
    rm -rf $BUILD_DIR/Docs
    mkdir -p $BUILD_DIR/Docs
    cd $BUILD_DIR/Docs
    doxygen $SOURCE_DIR/Doxyfile
}


# Parse command-line arguments
for arg in "$@"; do
  case $arg in
    --debug)
      BUILD_TYPE="Debug"
      shift
      ;;
    --release)
      BUILD_TYPE="Release"
      shift
      ;;
    --engine-only)
      BUILD_ENGINE_ONLY="ON"
      shift
      ;;
    --docs-only)
      DOCS_ONLY=true
      shift
      ;;
    --help|-h)
      echo "Usage: $0 [--debug|--release] [--engine-only] [--docs-only]"
      exit 0
      ;;
    *)
      echo "Unknown argument: $arg"
      exit 1
      ;;
  esac
done

if [ "$DOCS_ONLY" = true ]; then
    BuildDocumentation
    exit 0
fi

PROJECT_ROOT_PATH=$(pwd)
PATH_SHADERS_SOURCE=$PROJECT_ROOT_PATH/RenderingEngine/RenderingLibrary/BuiltInAssets/Shaders

# Clean previously created binaries
rm -rf Build
# Create new build directory and step into it
mkdir Build
cd Build
PATH_TO_BUILD=$(pwd)
# Run root cmake file
# Configure
cmake -S .. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DRE_BUILD_ENGINE_ONLY="${BUILD_ENGINE_ONLY}"

# Build
cmake --build .

cmake --install . --prefix "${BUILD_DIR}/Installed"

