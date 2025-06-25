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


## Unix-like Setup

### Ubuntu

1. Install dependencies:

```bash
sudo apt update
sudo apt install cmake build-essential libglfw3-dev libglm-dev libboost-filesystem-dev libgtest-dev libjpeg-dev libpng-dev libassimp-dev libfreetype6-dev pkg-config
```

2. Install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)
Note: As of May 2025, LunarG has discontinued updating Vulkan SDK packages in the official Ubuntu repositories.
The following approaches are now officially recommended:

Option 1: Install from tarball (universal, for any Linux/Unix-like system)
See instructions here:
https://vulkan.lunarg.com/doc/sdk/1.4.313.0/linux/getting_started.html

Option 2: Install via LunarG APT repository (recommended for Ubuntu)
```bash
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-noble.list http://packages.lunarg.com/vulkan/lunarg-vulkan-noble.list
sudo apt update
sudo apt install vulkan-sdk
```

3. Build the project:
From the root directory, run:
```./build_unix.sh```

4. Run demo application:
```bash
cd Build/DemoApplication
./DemoApplication
```

### FreeBSD

1. Install dependencies:

```bash
sudo pkg install pkgconf glfw glm boost-libs libjpeg-turbo png googletest assimp freetype2 vulkan-loader vulkan-headers shaderc
```
Some of these packages may be built from ports, but this does not affect the build process or functionality, so you may use either pkg or make install clean under /usr/ports/...

2. Build the project:
From the root directory, run:
```./build_unix.sh```

3. Run demo application:
```bash
cd Build/DemoApplication
./DemoApplication
```

## Portability notes
This engine builds and runs successfully on:
- Windows 10
- Ubuntu 24.04
- FreeBSD 14.2

Note on FreeBSD:
Vulkan validation layers are currently disabled on FreeBSD due to the inability to install them via pkg or build them from ports. Until this is resolved upstream (in the FreeBSD Ports collection), validation layers will remain disabled in the FreeBSD build of the rendering engine.



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