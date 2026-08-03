#!/bin/sh

set -eu

PREFIX="${STATIC_DEPS_PREFIX:-/usr/local}"
WORK_DIR="${STATIC_DEPS_WORK_DIR:-${TMPDIR:-/tmp}/RenderingEngineThirdPartySources}"

mkdir -p "${WORK_DIR}"

case "$(uname -s)" in
    Linux)
        JOBS="$(nproc)"
        SUDO="sudo"
        ;;
    FreeBSD)
        JOBS="$(sysctl -n hw.ncpu)"
        SUDO="sudo"
        ;;
    *)
        echo "Unsupported platform: $(uname -s)"
        exit 1
        ;;
esac

export PKG_CONFIG_PATH="${PREFIX}/lib/pkgconfig:${PREFIX}/libdata/pkgconfig${PKG_CONFIG_PATH:+:${PKG_CONFIG_PATH}}"
export CMAKE_PREFIX_PATH="${PREFIX}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"

build_libjpeg_turbo()
{
    cd "${WORK_DIR}"

    if [ ! -d libjpeg-turbo ]; then
        git clone --branch 2.1.5 --depth 1 \
            https://github.com/libjpeg-turbo/libjpeg-turbo.git
    fi

    cd libjpeg-turbo
    git clean -xfd

    cmake \
        -S . \
        -B build-static-pic \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DBUILD_SHARED_LIBS=OFF \
        -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
        -DCMAKE_INSTALL_LIBDIR="${PREFIX}/lib" \
        -DENABLE_SHARED=OFF \
        -DENABLE_STATIC=ON \
        -DWITH_JPEG8=ON \
        -DWITH_TURBOJPEG=OFF

    cmake --build build-static-pic -j"${JOBS}"
    ${SUDO} cmake --install build-static-pic
}

build_libpng()
{
    cd "${WORK_DIR}"

    if [ ! -d libpng ]; then
        git clone --branch v1.6.58 --depth 1 \
            https://github.com/pnggroup/libpng.git
    fi

    cd libpng
    git clean -xfd

    cmake \
        -S . \
        -B build-static-pic \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DBUILD_SHARED_LIBS=OFF \
        -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
        -DCMAKE_INSTALL_LIBDIR=lib \
        -DPNG_SHARED=OFF \
        -DPNG_STATIC=ON \
        -DPNG_TESTS=OFF \
        -DPNG_TOOLS=OFF

    cmake --build build-static-pic -j"${JOBS}"
    ${SUDO} cmake --install build-static-pic
}

build_freetype()
{
    cd "${WORK_DIR}"

    if [ ! -d freetype ]; then
        git clone --branch VER-2-13-2 --depth 1 \
            https://github.com/freetype/freetype.git
    fi

    cd freetype
    git clean -xfd

    cmake \
        -S . \
        -B build-static-pic \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DBUILD_SHARED_LIBS=OFF \
        -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
        -DCMAKE_INSTALL_LIBDIR=lib \
        -DFT_REQUIRE_ZLIB=ON \
        -DFT_REQUIRE_PNG=ON \
        -DFT_DISABLE_HARFBUZZ=ON \
        -DFT_DISABLE_BROTLI=ON \
        -DFT_DISABLE_BZIP2=ON

    cmake --build build-static-pic -j"${JOBS}"
    ${SUDO} cmake --install build-static-pic
}

build_harfbuzz()
{
    cd "${WORK_DIR}"

    if [ ! -d harfbuzz ]; then
        git clone --branch 8.3.0 --depth 1 \
            https://github.com/harfbuzz/harfbuzz.git
    fi

    cd harfbuzz
    git clean -xfd

    PKG_CONFIG_PATH="${PKG_CONFIG_PATH}" \
    meson setup build-static-pic \
        --buildtype=release \
        --default-library=static \
        --prefix="${PREFIX}" \
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

    meson compile -C build-static-pic
    ${SUDO} meson install -C build-static-pic
}

build_assimp()
{
    cd "${WORK_DIR}"

    if [ ! -d assimp ]; then
        git clone --branch v5.4.3 --depth 1 \
            https://github.com/assimp/assimp.git
    fi

    cd assimp
    git clean -xfd

    cmake \
        -S . \
        -B build-static-pic \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DBUILD_SHARED_LIBS=OFF \
        -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
        -DCMAKE_INSTALL_LIBDIR=lib \
        -DASSIMP_BUILD_TESTS=OFF \
        -DASSIMP_BUILD_ASSIMP_TOOLS=OFF \
        -DASSIMP_BUILD_SAMPLES=OFF \
        -DASSIMP_BUILD_DOCS=OFF \
        -DASSIMP_BUILD_ZLIB=OFF \
        -DASSIMP_BUILD_MINIZIP=ON \
        -DASSIMP_WARNINGS_AS_ERRORS=OFF

    cmake --build build-static-pic -j"${JOBS}"
    ${SUDO} cmake --install build-static-pic
}

build_glfw()
{
    cd "${WORK_DIR}"

    if [ ! -d glfw ]; then
        git clone --branch 3.4 --depth 1 \
            https://github.com/glfw/glfw.git
    fi

    cd glfw
    git clean -xfd

    cmake \
        -S . \
        -B build-static-pic \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DBUILD_SHARED_LIBS=OFF \
        -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
        -DCMAKE_INSTALL_LIBDIR=lib \
        -DGLFW_BUILD_X11=ON \
        -DGLFW_BUILD_WAYLAND=ON \
        -DGLFW_BUILD_EXAMPLES=OFF \
        -DGLFW_BUILD_TESTS=OFF \
        -DGLFW_BUILD_DOCS=OFF \
        -DGLFW_INSTALL=ON

    cmake --build build-static-pic -j"${JOBS}"
    ${SUDO} cmake --install build-static-pic
}

build_libjpeg_turbo
build_libpng
build_freetype
build_harfbuzz
build_assimp
build_glfw
