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

#### 4. Building FreeType (Unix)

Rendering Engine statically links **FreeType** into `libRenderingEngine.so`.
A custom static PIC build is therefore required instead of the distribution package.

Clone the repository:

```bash
git clone https://gitlab.freedesktop.org/freetype/freetype.git
cd freetype
git checkout VER-2-13-2
```

Configure:

```bash
cmake -S . -B build-static-pic \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_INSTALL_LIBDIR=lib \
    -DFT_REQUIRE_ZLIB=ON \
    -DFT_REQUIRE_PNG=ON \
    -DFT_REQUIRE_HARFBUZZ=OFF \
    -DFT_DISABLE_BROTLI=ON \
    -DFT_DISABLE_BZIP2=ON
```

Build and install:

```bash
cmake --build build-static-pic -j$(nproc)
sudo cmake --install build-static-pic
```

Verify:

```bash
pkg-config --modversion freetype2
pkg-config --variable=libdir freetype2
pkg-config --libs --static freetype2
```

Expected library:

```text
/usr/local/lib/libfreetype.a
```

Expected headers:

```text
/usr/local/include/freetype2
```

#### 5. Building HarfBuzz (Unix)

Rendering Engine statically links **HarfBuzz** into `libRenderingEngine.so`.

Clone the repository:

```bash
git clone https://github.com/harfbuzz/harfbuzz.git
cd harfbuzz
git checkout 8.3.0
```

Configure:

```bash
meson setup build-static-pic \
    --buildtype=release \
    --default-library=static \
    --prefix=/usr/local \
    --libdir=lib \
    -Db_staticpic=true \
    -Dfreetype=enabled \
    -Dglib=disabled \
    -Dgobject=disabled \
    -Dgraphite2=disabled \
    -Dicu=disabled \
    -Dcairo=disabled \
    -Dintrospection=disabled \
    -Ddocs=disabled \
    -Ddoc_tests=false \
    -Dutilities=disabled \
    -Dtests=disabled \
    -Dbenchmark=disabled
```

Build and install:

```bash
meson compile -C build-static-pic
sudo meson install -C build-static-pic
```

Verify:

```bash
pkg-config --modversion harfbuzz
pkg-config --variable=libdir harfbuzz
pkg-config --libs --static harfbuzz
```

Expected library:

```text
/usr/local/lib/libharfbuzz.a
```

Expected headers:

```text
/usr/local/include/harfbuzz
```

#### 6. Building Assimp (Unix)
The Rendering Engine uses Assimp internally for loading 3D model formats.

On Unix-like systems, Assimp is built as a static position-independent library and linked into `libRenderingEngine.so`. This prevents the Rendering Engine SDK from requiring a separate `libassimp.so` runtime dependency.

The validated Assimp version is:

```text
5.4.3
```

Assimp 5.4.3 is used instead of 5.3.0 because the upstream 5.3.0 installation may omit the bundled utf8cpp headers required by installed Assimp headers.

Clone the repository:

```bash
mkdir -p ~/Development/ThirdPartySources
cd ~/Development/ThirdPartySources

git clone https://github.com/assimp/assimp.git
cd assimp
```

Checkout the validated version:

```
git checkout v5.4.3
git describe --tags --exact-match
```

Expected output:

```
v5.4.3
```

Configure Assimp:

```
cmake -S . -B build-static-pic \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_INSTALL_LIBDIR=lib \
    -DASSIMP_BUILD_TESTS=OFF \
    -DASSIMP_BUILD_ASSIMP_TOOLS=OFF \
    -DASSIMP_BUILD_SAMPLES=OFF \
    -DASSIMP_BUILD_DOCS=OFF \
    -DASSIMP_BUILD_ZLIB=OFF \
    -DASSIMP_WARNINGS_AS_ERRORS=OFF

```

Build and install:

```
cmake --build build-static-pic -j"$(nproc)"
sudo cmake --install build-static-pic
```

The expected static library is:

```
/usr/local/lib/libassimp.a
```

The expected public headers are installed under:

```
/usr/local/include/assimp
```

Verify the installation

Check the static archive:

```
ls -lh /usr/local/lib/libassimp.a
```

Check the installed Assimp version:

```
PKG_CONFIG_PATH=/usr/local/lib/pkgconfig \
    pkg-config --modversion assimp
```

Expected output:

```
5.4.3
```

Verify that the required utf8cpp headers were installed:

```
find /usr/local/include \
    \( -name utf8.h -o -path '*utf8cpp*' \) \
    -print
```

Verify that Assimp references external zlib symbols rather than embedding zlib:

```
nm /usr/local/lib/libassimp.a \
    | grep " U " \
    | grep -E 'inflate|deflate'
```

#### 7. Building GLFW (Unix)

The Rendering Engine statically links GLFW into `libRenderingEngine.so` to eliminate the runtime dependency on `libglfw.so`. This allows SDK users to deploy only the Rendering Engine library without requiring a matching GLFW installation.

The Rendering Engine has been validated with:

```text
- **GLFW 3.4**
```

Clone the repository:

```bash
git clone https://github.com/glfw/glfw.git
cd glfw
git checkout 3.4
```

Install the required development packages:

```bash
sudo apt update
sudo apt install \
    libwayland-dev \
    libxkbcommon-dev \
    xorg-dev
```

Configure a static Position Independent Code (PIC) build:

```bash
cmake -S . -B build-static-pic \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_INSTALL_LIBDIR=lib \
    -DGLFW_BUILD_X11=ON \
    -DGLFW_BUILD_WAYLAND=ON \
    -DGLFW_BUILD_EXAMPLES=OFF \
    -DGLFW_BUILD_TESTS=OFF \
    -DGLFW_BUILD_DOCS=OFF
```

Build and install:

```bash
cmake --build build-static-pic -j"$(nproc)"
sudo cmake --install build-static-pic
```

The installation should produce:

```
/usr/local/lib/libglfw3.a
```

Confirm that the expected version is installed:

```bash
PKG_CONFIG_PATH=/usr/local/lib/pkgconfig \
pkg-config --modversion glfw3
```

Expected output:

```
3.4.0
```

Confirm that the static library is present:

```bash
find /usr/local/lib \
    -maxdepth 1 \
    -name 'libglfw3.a'
```

Expected output:

```
/usr/local/lib/libglfw3.a
```

#### 8. Install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)
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
