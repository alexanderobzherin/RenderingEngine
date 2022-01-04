
echo "Finding shader compiler..."

mkdir Intermediate
cd Intermediate
mkdir Shaders
cd Shaders

d:

echo "Shaders compilation..."
 VulkanSDK\1.2.189.2\Bin\glslc.exe E:\Development\VulkanProject\vulkanproject\Shaders\basic_shader.vert -o E:\Development\VulkanProject\vulkanproject\Build\Intermediate\Shaders\basic_shader_vert.spv
 VulkanSDK\1.2.189.2\Bin\glslc.exe E:\Development\VulkanProject\vulkanproject\Shaders\basic_shader.frag -o E:\Development\VulkanProject\vulkanproject\Build\Intermediate\Shaders\basic_shader_frag.spv


