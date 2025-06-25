#Build script for VulkanProject
#Written by Alexander Obzherin

PROJECT_ROOT_PATH=$(pwd)
PATH_SHADERS_SOURCE=$PROJECT_ROOT_PATH/Shaders

#Clean previousle created binaries
rm -rf Build
#Create new build directory and step into it
mkdir Build
cd Build
PATH_TO_BUILD=$(pwd)
#Run root cmake file
cmake ..
#Run make file
make

#Shader compilation
CompileShaders()
{
    echo "Shaders compilation..."
    cd $PATH_TO_BUILD
    if [ -d "Intermediate" ]
    then
        echo "Intermediate folder exists."
    else
        mkdir Intermediate
    fi
    cd Intermediate
    mkdir Shaders
    cd Shaders
    PATH_SHADERS_BIN=$(pwd)

    $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/basic_shader.vert -o $PATH_SHADERS_BIN/basic_shader_vert.spv
    $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/basic_shader.frag -o $PATH_SHADERS_BIN/basic_shader_frag.spv
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
