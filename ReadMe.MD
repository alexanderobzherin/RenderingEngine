List of required dependencies

VulkanSDK
GLFW
GLM
Boost::filesystem
gtest
libjpeg-turbo

**********Windows installing.**********
It might be possible to use vcpkg package manager

git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install

Now it is needed to define DCMAKE_TOOLCHAIN_FILE with path to vcpkg and use in build_windows.bat script like this:
cmake .. -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake

.\vcpkg.exe install vulkan-headers
.\vcpkg.exe install vulkan-headers:x64-windows

.\vcpkg.exe install ms-gsl gl3w glfw3 glm
.\vcpkg.exe install ms-gsl:x64-windows gl3w:x64-windows glfw3:x64-windows glm:x64-windows

.\vcpkg.exe install boost-filesystem
.\vcpkg.exe install boost-filesystem:x64-windows

.\vcpgg.exe install gtest
.\vcpgg.exe install gtest:x64-windows

.\vcpgg.exe install libjpeg-turbo
.\vcpgg.exe install libjpeg-turbo:x64-windows

Set environment variable glslc.
Example:
Variable name: glslc
Variable value:  D:\VulkanSDK\1.2.189.2\Bin\glslc.exe

Also for now it is needed to modify path in windows_compile_shaders.bat like this:
E:\Development\VulkanProject\rendering-engine\Shaders\basic_shader.vert -o E:\Development\VulkanProject\rendering-engine\Build\Intermediate\Shaders\basic_shader_vert.spv
windows_compile_shaders.bat is a script that will be employed by cmake and attached to pre-build script in Visual Studio Solution in order to compile glsl shader files into *.spv binaries.

**********Linux installing.**********
Install needed dependencies. See "List of required dependencies"
Run script build_linux.sh
cd Build
./VulkanProject