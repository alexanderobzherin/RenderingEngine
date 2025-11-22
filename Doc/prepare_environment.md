# Prepare environment

Download the source code:

```bash
git clone git@bitbucket.org:AlexanderObzherin/rendering-engine.git
```

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
.\vcpkg install --triplet x64-windows-static-md glfw3 glm boost-filesystem libpng libjpeg-turbo freetype assimp nlohmann-json gtest
```

3. Set the VCPKG_ROOT environment variable to the folder where you cloned vcpkg:
Variable name: ```VCPKG_ROOT```
Variable value: ```Path\To\vcpkg```

4. Download and install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)

5. Set the glslc environment variable to point to the glslc.exe binary included in the SDK:
Variable name: ```glslc```
Variable value: ```Path\To\glslc.exe```

## Unix-like Setup

### Ubuntu

1. Install dependencies:

```bash
sudo apt update
sudo apt install cmake build-essential libglfw3-dev libglm-dev libboost-filesystem-dev libgtest-dev libjpeg-dev libpng-dev libassimp-dev libfreetype6-dev pkg-config nlohmann-json3-dev
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

### FreeBSD

1. Install dependencies:

```bash
sudo pkg install pkgconf glfw glm boost-libs libjpeg-turbo png googletest assimp freetype2 vulkan-loader vulkan-headers shaderc nlohmann-json
```
Some of these packages may be built from ports, but this does not affect the build process or functionality, so you may use either pkg or make install clean under /usr/ports/...

<- [Back to Developer Guide Page](developer_guide.md)
