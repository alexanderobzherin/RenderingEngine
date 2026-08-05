# Prepare Environment

Rendering Engine supports two application development workflows and a separate
runtime environment.

## Dev Mode

Dev Mode uses the Rendering Engine source tree directly. It is intended for both
engine development and application development when full access to the engine
source code is useful.

In this mode, developers can:

- modify and rebuild the Rendering Engine;
- build TestApplications and ContentExamples from the same source tree;
- create applications that evolve together with engine features;
- debug through both application and engine code.

Dev Mode requires the complete Rendering Engine build environment, including
the engine's third-party implementation dependencies.

## SDK User Mode

SDK User Mode uses the precompiled Rendering Engine SDK.

It is intended for applications that consume the engine as a binary dependency
and do not need to modify or debug the engine implementation.

SDK users require a C++ toolchain, CMake, Python, the Vulkan SDK, and the
precompiled Rendering Engine SDK. The engine's implementation dependencies are
already included in the precompiled Rendering Engine library.

---

## Windows

### Dev Mode

Requirements
Visual Studio, CMake, Python

1. Install [vcpkg](https://github.com/microsoft/vcpkg):
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install
```

2. Set the VCPKG_ROOT environment variable to the folder where you cloned vcpkg:

Variable name: ```VCPKG_ROOT```  
Variable value: ```Path\To\vcpkg```  

3. Install required libraries:

```bash
.\vcpkg install glm nlohmann-json gtest
```

4. Download and install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)

5. Set the glslc environment variable to point to the glslc.exe binary included in the SDK:
Variable name: ```glslc```
Variable value: ```Path\To\glslc.exe```

6. Download the source code:

```bash
git clone git@github.com:alexanderobzherin/RenderingEngine.git
cd RenderingEngine
```

7. Prepare static dependencies

```bash
.\vcpkg install --triplet x64-windows-static-md glfw3 libpng libjpeg-turbo freetype harfbuzz assimp
```

8. Build engine

Run

```bash
.\build_engine.bat
```

### SDK User Mode

Requirements
Visual Studio, CMake, Python

1. Download latest release from 

[Rendering Engine SDK Releases](https://github.com/alexanderobzherin/RenderingEngine/releases)

2. Here you can build content examples such as
```bash
ContentExamples\MultilingualTextDemo\build_project.bat
```
and/or create new projects. See: [Project Creation Guide](project_creation_guide.md)

---

## Ubuntu

### Dev Mode

1. Install build tools and libraries:

```bash
sudo apt update
sudo apt install git cmake build-essential nasm pkg-config libglm-dev libgtest-dev nlohmann-json3-dev
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

3. Download the source code:

```bash
git clone git@github.com:alexanderobzherin/RenderingEngine.git
cd RenderingEngine
```

4. Prepare static dependencies

The Rendering Engine provides a helper script that downloads, builds and
installs all required implementation libraries automatically.

Run

```bash
./RenderingEngine/Scripts/build_static_dependencies_unix.sh
```

5. Build engine

Run

```bash
./build_engine.sh --engine-only
```

### SDK User Mode

1. Install build tools and libraries:

```bash
sudo apt update
sudo apt install git cmake build-essential nasm pkg-config
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

3. Download latest release from 

[Rendering Engine SDK Releases](https://github.com/alexanderobzherin/RenderingEngine/releases)

Here you can build content examples such as
```bash
ContentExamples/MultilingualTextDemo/build_project.sh
```
and/or create new projects. See: [Project Creation Guide](project_creation_guide.md)

---

## FreeBSD

### Dev Mode

1. Install build tools and libraries:

```bash
sudo pkg install pkgconf glm googletest vulkan-loader vulkan-headers shaderc nlohmann-json
```

2. Download the source code:

```bash
git clone git@github.com:alexanderobzherin/RenderingEngine.git
cd RenderingEngine
```

3. Prepare static dependencies

The Rendering Engine provides a helper script that downloads, builds and
installs all required implementation libraries automatically.

Run

```bash
./RenderingEngine/Scripts/build_static_dependencies_unix.sh
```

4. Build engine

Run

```bash
./build_engine.sh --engine-only
```

### SDK User Mode

1. Install build tools and libraries:

```bash
sudo pkg install pkgconf glm vulkan-loader shaderc 
```

2. Download latest release from 

[Rendering Engine SDK Releases](https://github.com/alexanderobzherin/RenderingEngine/releases)

Here you can build content examples such as
```bash
ContentExamples/MultilingualTextDemo/build_project.sh
```
and/or create new projects. See: [Project Creation Guide](project_creation_guide.md)

---

## Application Deployment

Applications built with the Rendering Engine are intended to have a minimal
runtime footprint.

By default, user applications link dynamically against the Rendering Engine shared library.

Typical runtime requirements are:

- RenderingEngine.dll / libRenderingEngine.so
- Vulkan loader and compatible graphics driver
- zlib (Linux and FreeBSD)
- Standard operating-system runtime libraries

For more details see:

- [Project Packaging Guide](project_packaging_guide.md)
- [Target HMI Hardware Reference Platform](HMI/reference_platform.md)




