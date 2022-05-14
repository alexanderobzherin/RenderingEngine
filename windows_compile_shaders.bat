
echo "Finding shader compiler..."

mkdir Intermediate
cd Intermediate
mkdir Shaders
cd Shaders

d:

echo "Shaders compilation..."
%glslc% E:\Development\VulkanProject\rendering-engine\Shaders\basic_shader.vert -o E:\Development\VulkanProject\rendering-engine\Build\Intermediate\Shaders\basic_shader_vert.spv
%glslc% E:\Development\VulkanProject\rendering-engine\Shaders\basic_shader.frag -o E:\Development\VulkanProject\rendering-engine\Build\Intermediate\Shaders\basic_shader_frag.spv


