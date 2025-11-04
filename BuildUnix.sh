#Written by Alexander Obzherin

# Check for --docs-only argument
if [ "$1" == "--docs-only" ]; then
    # Generate documentation
    echo "Generating Doxygen documentation only..."
    rm -rf Build/Docs
    mkdir -p Build/Docs
    cd Build/Docs
    doxygen ../../Doxyfile
    exit 0
fi

PROJECT_ROOT_PATH=$(pwd)
PATH_SHADERS_SOURCE=$PROJECT_ROOT_PATH/Shaders

#Clean previously created binaries
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
    mkdir BasicTexture3D
    mkdir FlatColorFiltering
    mkdir Quad2D
    PATH_SHADERS_BIN=$(pwd)

    $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/BasicTexture3D/BasicTexture3D.vert -o $PATH_SHADERS_BIN/BasicTexture3D/BasicTexture3D_vert.spv
    $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/BasicTexture3D/BasicTexture3D.frag -o $PATH_SHADERS_BIN/BasicTexture3D/BasicTexture3D_frag.spv
    $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/FlatColorFiltering/FlatColorFiltering.vert -o $PATH_SHADERS_BIN/FlatColorFiltering/FlatColorFiltering_vert.spv
    $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/FlatColorFiltering/FlatColorFiltering.frag -o $PATH_SHADERS_BIN/FlatColorFiltering/FlatColorFiltering_frag.spv
    $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/Quad2D/Quad2D.vert -o $PATH_SHADERS_BIN/Quad2D/Quad2D_vert.spv
    $PATH_TO_SHADER_COMPILER $PATH_SHADERS_SOURCE/Quad2D/Quad2D.frag -o $PATH_SHADERS_BIN/Quad2D/Quad2D_frag.spv
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
