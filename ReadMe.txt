List of required dependencies

VulkanSDK
GLFW
GLM
Boost::filesystem

**********Windows installing.**********
It might be possible to use vcpkg package manager

git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install

Now it is needed to define DCMAKE_TOOLCHAIN_FILE with path to vcpkg and use in build script like this:
cmake .. -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake

.\vcpkg.exe install vulkan-headers
.\vcpkg.exe install vulkan-headers:x64-windows

.\vcpkg.exe install ms-gsl gl3w glfw3 glm
.\vcpkg.exe install ms-gsl:x64-windows gl3w:x64-windows glfw3:x64-windows glm:x64-windows

.\vcpkg.exe install boost-filesystem
.\vcpkg.exe install boost-filesystem:x64-windows
