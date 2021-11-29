#Build script for VulkanProject
#Written by Alexander Obzherin

PROJECT_ROOT_PATH=$(pwd)
PATH_SHADERS_SOURCE=$PROJECT_ROOT_PATH/Shaders

#Clean previousle created binaries
rm -rf Build
#Create new build directory and step into it
mkdir Build
cd Build
#Run root cmake file
cmake ..
#Run make file
make

#Shader compilation

#find shader compiler and store path to it.
PATH_TO_SHADER_COMPILER=$(find / -name glslc 2>/dev/null)
if ! test -f "$PATH_TO_SHADER_COMPILER"; then
    echo "Shader compiler not found."
    exit 1
fi

echo "Shaders compilation..."

mkdir Intermediate
cd Intermediate
mkdir Shaders
cd Shaders
PATH_SHADERS_BIN=$(pwd)

$PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/basic_shader.vert -o $PATH_SHADERS_BIN/basic_shader_vert.spv
$PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/basic_shader.frag -o $PATH_SHADERS_BIN/basic_shader_frag.spv
