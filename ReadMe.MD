<p align="center">
    <img src="./Doc/Images/RE_Banner.png" alt="Rendering Engine">
</p>

# Rendering Engine

A cross-platform rendering engine designed to be portable and backend-agnostic.  
Currently supports Windows and Unix-like systems (Linux, FreeBSD).

### Features (Work in Progress)

- Support for 3D model loading and rendering (e.g., OBJ, FBX)
- Texture loading and binding (PNG, JPEG support)
- Customizable material and shader system (each model can use a unique shader)
- Text rendering (under development)
- Sprite rendering support (in progress)
- Unit tests and demo application included

## Dependencies

The following libraries are required:

- [Vulkan SDK](https://vulkan.lunarg.com/)
- [GLFW](https://www.glfw.org/)
- [GLM](https://github.com/g-truc/glm)
- [Boost::Filesystem](https://www.boost.org/)
- [GTest](https://github.com/google/googletest)
- [libjpeg-turbo](https://libjpeg-turbo.org/)
- [libpng](http://www.libpng.org/)
- [Assimp](https://github.com/assimp/assimp)
- [FreeType](https://freetype.org/)

---

## Windows Setup

1. Install [vcpkg](https://github.com/microsoft/vcpkg):

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install
```

2. Install required libraries:
```bash
.\vcpkg.exe install glfw3 glm boost-filesystem gtest libjpeg-turbo libpng assimp freetype
.\vcpkg.exe install glfw3:x64-windows glm:x64-windows boost-filesystem:x64-windows gtest:x64-windows libjpeg-turbo:x64-windows libpng:x64-windows assimp:x64-windows freetype:x64-windows
```

3. Set the VCPKG_ROOT environment variable to the folder where you cloned vcpkg:
Variable name: ```VCPKG_ROOT```
Variable value: ```Path\To\vcpkg```

4. Download and install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)

5. Set the glslc environment variable to point to the glslc.exe binary included in the SDK:
Variable name: ```glslc```
Variable value: ```Path\To\glslc.exe```

6. Build the project:
From the root directory, run:
```.\build_windows.bat```
This will generate a Build directory containing the Visual Studio solution and project files, compile shaders, and build the engine.


**********Linux installing.**********
Install needed dependencies. See "List of required dependencies"
Run script build_linux.sh
cd Build
./VulkanProject

# To run docker without sudo
sudo groupadd docker
sudo usermod -aG docker $USER

# Build container based on Dockerfile image
docker build -t re_env_cnr .

# Run container
docker run --name test_env_cnr -v /home/alexander/Development/rendering-engine:/rendering-engine -it re_env_cnr

# If container already has been run before
docker start -i re_env_cnr 

# Stop container
docker stop re_env_cnr

# Remove unnecessary image and containers
docker image prune -a
docker container prune