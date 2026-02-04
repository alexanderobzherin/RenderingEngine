# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.

BUILD_MODE="Release"
if [ "$1" = "--debug" ];then
  BUILD_MODE="Debug"
elif [ "$1" = "--release" ];then
  MODE="Release"
fi
echo "Build mode: "
echo "$BUILD_MODE"

PROJECT_SOURCE_DIR=$(pwd)
echo "Project source dir: "
echo $PROJECT_SOURCE_DIR

ENGINE_ROOT_FOLDER="$PROJECT_SOURCE_DIR"

# Search for RenderingEngine directory
while true; do
    if [ -d "RenderingEngine" ]; then
        ENGINE_ROOT_FOLDER=$(pwd)
        break
    fi    
    # If we're at root directory, stop searching
    if [ "$(pwd)" = "/" ]; then
        echo "RenderingEngine not found in parent directories"
        exit 1
    fi    
    cd ..
done

echo "Engine source dir: "
echo "$ENGINE_ROOT_FOLDER"

cd $ENGINE_ROOT_FOLDER
cd ..
PROJECT_BUILD_DIR=$ENGINE_ROOT_FOLDER/Build/UserApplications/TextShaping
echo "Project build dir: "
echo $PROJECT_BUILD_DIR

PATH_SHADERS_SOURCE=$PROJECT_BUILD_DIR/Content/Shaders
echo "Shader source dir: "
echo $PATH_SHADERS_SOURCE
PATH_SHADERS_BIN=$PATH_SHADERS_SOURCE

rm -rf $PROJECT_BUILD_DIR
mkdir -p $PROJECT_BUILD_DIR
cd $PROJECT_BUILD_DIR

mkdir -p $BUILD_MODE
#Run root cmake file
cmake -S "$PROJECT_SOURCE_DIR" -B "$PROJECT_BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_MODE
#Run make file
make

#Shader compilation
CompileShaders()
{
  # Compile buildin shaders
  $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/BasicTexture3D/BasicTexture3D.vert -o $PATH_SHADERS_BIN/BasicTexture3D/BasicTexture3D_vert.spv
  $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/BasicTexture3D/BasicTexture3D.frag -o $PATH_SHADERS_BIN/BasicTexture3D/BasicTexture3D_frag.spv
  $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/FlatColorFiltering/FlatColorFiltering.vert -o $PATH_SHADERS_BIN/FlatColorFiltering/FlatColorFiltering_vert.spv
  $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/FlatColorFiltering/FlatColorFiltering.frag -o $PATH_SHADERS_BIN/FlatColorFiltering/FlatColorFiltering_frag.spv
  $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/Quad2D/Quad2D.vert -o $PATH_SHADERS_BIN/Quad2D/Quad2D_vert.spv
  $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/Quad2D/Quad2D.frag -o $PATH_SHADERS_BIN/Quad2D/Quad2D_frag.spv
  $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/Font2D/Font2D.vert -o $PATH_SHADERS_BIN/Font2D/Font2D_vert.spv
  $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/Font2D/Font2D.frag -o $PATH_SHADERS_BIN/Font2D/Font2D_frag.spv

  # Compile custom user shader from json files via calling MaterialCompiler
  # TO DO
}

echo "Finding shader compiler..."

find / -name glslc 2>/dev/null |
while read -r PATH_TO_SHADER_COMPILER
do
if test -f $PATH_TO_SHADER_COMPILER; then
    echo "Shader compiler found $PATH_TO_SHADER_COMPILER"
    CompileShaders
    break
fi
done

