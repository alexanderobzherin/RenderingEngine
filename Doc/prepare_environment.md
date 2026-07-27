# Prepare environment

Download the source code:

```bash
git clone git@bitbucket.org:AlexanderObzherin/rendering-engine.git
```

## Windows Setup

#### 1. Install [vcpkg](https://github.com/microsoft/vcpkg):

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe integrate install
```


#### 2. Install required libraries:

```bash
.\vcpkg install --triplet x64-windows-static-md glfw3 glm libpng libjpeg-turbo freetype harfbuzz assimp nlohmann-json gtest
```

#### 3. Set the VCPKG_ROOT environment variable to the folder where you cloned vcpkg:

Variable name: ```VCPKG_ROOT```  
Variable value: ```Path\To\vcpkg```  

#### 4. Download and install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)

#### 5. Set the glslc environment variable to point to the glslc.exe binary included in the SDK:
Variable name: ```glslc```
Variable value: ```Path\To\glslc.exe```

## Unix-like Setup

### Ubuntu

#### 1. Install build tools and dependencies:

```bash
sudo apt update
sudo apt install git cmake build-essential nasm pkg-config libglfw3-dev libglm-dev libgtest-dev libassimp-dev libfreetype6-dev libharfbuzz-dev pkg-config nlohmann-json3-dev
```

#### 2. Build and install the static PIC-enabled libjpeg-turbo library:

Rendering Engine is built as a shared library and statically incorporates
libjpeg-turbo on Unix-like systems. The static libjpeg archive provided by
Ubuntu is not built with position-independent code and therefore cannot be
linked into `libRenderingEngine.so`.

Build and install libjpeg-turbo 2.1.5 with static linkage and
position-independent code enabled:

```bash
mkdir -p ~/Development/ThirdPartySources
cd ~/Development/ThirdPartySources

git clone \
    --branch 2.1.5 \
    --depth 1 \
    https://github.com/libjpeg-turbo/libjpeg-turbo.git

cd libjpeg-turbo

cmake \
    -S . \
    -B build-static-pic \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_INSTALL_LIBDIR=/usr/local/lib \
    -DENABLE_SHARED=OFF \
    -DENABLE_STATIC=ON \
    -DWITH_JPEG8=ON \
    -DWITH_TURBOJPEG=OFF

cmake --build build-static-pic --parallel

ctest \
    --test-dir build-static-pic \
    --output-on-failure

sudo cmake --install build-static-pic
```

Verify that the static library and headers were installed:

```bash
ls -lh /usr/local/lib/libjpeg.a
ls -l /usr/local/include/jpeglib.h
```

The expected installation paths are:

```text
/usr/local/lib/libjpeg.a
/usr/local/include/jpeglib.h
```

The expected installation paths are:

```text
/usr/local/lib/libpng16.a
/usr/local/include/png.h
```

#### 3. Build and install the static PIC-enabled libpng library:

Rendering Engine is built as a shared library and statically incorporates
libpng on Unix-like systems. The static libpng archive provided by Ubuntu
is not built with position-independent code and therefore cannot be linked
into libRenderingEngine.so.

Build and install libpng 1.6.58 with static linkage and
position-independent code enabled:

```bash
mkdir -p ~/Development/ThirdPartySources
cd ~/Development/ThirdPartySources

git clone \
    https://github.com/pnggroup/libpng.git

cd libpng

git checkout v1.6.58

cmake \
    -S . \
    -B build-static-pic \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_INSTALL_LIBDIR=/usr/local/lib \
    -DPNG_SHARED=OFF \
    -DPNG_STATIC=ON \
    -DPNG_TESTS=ON \
    -DPNG_TOOLS=OFF

cmake --build build-static-pic --parallel

ctest \
    --test-dir build-static-pic \
    --output-on-failure

sudo cmake --install build-static-pic
```

Verify that the static library and headers were installed:

```
ls -lh /usr/local/lib/libpng16.a
ls -l /usr/local/include/png.h
```

#### 4. Install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)
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
sudo pkg install pkgconf glfw glm libjpeg-turbo png googletest assimp freetype2 harfbuzz vulkan-loader vulkan-headers shaderc nlohmann-json
```
Some of these packages may be built from ports, but this does not affect the build process or functionality, so you may use either pkg or make install clean under /usr/ports/...

<- [Back to Developer Guide Page](developer_guide.md)
