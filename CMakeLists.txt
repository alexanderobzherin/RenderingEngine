# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.

cmake_minimum_required(VERSION 3.16)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "2")
set(VERSION_PATCH "11")

#set the project name
project(RenderingEngineProject VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} LANGUAGES CXX)

# Get date in YYYY-MM-DD format
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

# Path to template and output
set(VERSION_TEMPLATE "${CMAKE_SOURCE_DIR}/RenderingEngine/RenderingLibrary/Include/version.h.in")
set(VERSION_OUTPUT   "${CMAKE_BINARY_DIR}/Intermediate/Generated/version.h")

configure_file(${VERSION_TEMPLATE} ${VERSION_OUTPUT} @ONLY)

# Make generated header visible to all targets
include_directories(${CMAKE_BINARY_DIR}/Intermediate/Generated)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build flags
if(UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -g0 -s")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

option(RE_BUILD_ENGINE_ONLY "" OFF)
option(BUILD_SDK "Build the Rendering Engine as SDK" OFF)

if (NOT CMAKE_VERSION VERSION_LESS 3.7.0)
    find_package(Vulkan REQUIRED)
else()
    message(FATAL_ERROR "CMake >= 3.7 required for FindVulkan.")
endif()

# Extra fallback for manual SDK path if needed
if (NOT Vulkan_FOUND)
    if (WIN32)
        find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan
                     PATHS "${CMAKE_SOURCE_DIR}/libs/vulkan")
    elseif (UNIX)
        find_library(Vulkan_LIBRARY NAMES vulkan
                     HINTS "$ENV{VULKAN_SDK}/lib" "${CMAKE_SOURCE_DIR}/libs/vulkan")
    endif()

    if (Vulkan_LIBRARY)
        set(Vulkan_FOUND ON)
        add_library(Vulkan::Vulkan UNKNOWN IMPORTED)
        set_target_properties(Vulkan::Vulkan PROPERTIES
            IMPORTED_LOCATION "${Vulkan_LIBRARY}")
        message(STATUS "Using bundled Vulkan library: ${Vulkan_LIBRARY}")
    else()
        message(FATAL_ERROR "Could not find Vulkan library!")
    endif()
endif()

if (WIN32)
    add_compile_definitions(VK_USE_PLATFORM_WIN32_KHR)
endif()

find_package(glfw3 CONFIG REQUIRED)

find_package(glm CONFIG REQUIRED)

find_package(GTest REQUIRED)

set(JPEG_STATIC ON)
find_package(JPEG REQUIRED)

set(PNG_STATIC ON)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
if(UNIX AND NOT WIN32)
    find_library(
        RE_JPEG_STATIC_LIBRARY
        NAMES libjpeg.a
        PATHS
            /usr/local/lib
            /usr/local/lib64
        NO_DEFAULT_PATH
        REQUIRED
    )
    message(STATUS
        "Rendering Engine static JPEG library: ${RE_JPEG_STATIC_LIBRARY}"
    )

    find_library(
        RE_PNG_STATIC_LIBRARY
        NAMES
            libpng16.a
            libpng.a
        PATHS
            /usr/local/lib
            /usr/lib
            /usr/lib/x86_64-linux-gnu
        NO_DEFAULT_PATH
        REQUIRED
    )

    message(
        STATUS
        "Rendering Engine static PNG library: "
        "${RE_PNG_STATIC_LIBRARY}"
    )

    find_package(Threads REQUIRED)

    find_library(
        RE_HARFBUZZ_STATIC_LIBRARY
        NAMES libharfbuzz.a
        PATHS
            /usr/local/lib
            /usr/local/lib64
        NO_DEFAULT_PATH
        REQUIRED
    )

    find_path(
        RE_HARFBUZZ_INCLUDE_DIR
        NAMES hb.h
        PATHS
            /usr/local/include/harfbuzz
        NO_DEFAULT_PATH
        REQUIRED
    )

    message(
        STATUS
        "Rendering Engine static HarfBuzz library: "
        "${RE_HARFBUZZ_STATIC_LIBRARY}"
    )

    message(
        STATUS
        "Rendering Engine HarfBuzz include directory: "
        "${RE_HARFBUZZ_INCLUDE_DIR}"
    )

    find_library(
        RE_FREETYPE_STATIC_LIBRARY
        NAMES libfreetype.a
        PATHS
            /usr/local/lib
            /usr/local/lib64
        NO_DEFAULT_PATH
        REQUIRED
    )

    find_path(
        RE_FREETYPE_INCLUDE_DIR
        NAMES ft2build.h
        PATHS
            /usr/local/include/freetype2
        NO_DEFAULT_PATH
        REQUIRED
    )

    message(
        STATUS
        "Rendering Engine static FreeType library: "
        "${RE_FREETYPE_STATIC_LIBRARY}"
    )

    message(
        STATUS
        "Rendering Engine FreeType include directory: "
        "${RE_FREETYPE_INCLUDE_DIR}"
    )
        find_library(
        RE_ASSIMP_STATIC_LIBRARY
        NAMES libassimp.a
        PATHS
            /usr/local/lib
            /usr/local/lib64
        NO_DEFAULT_PATH
        REQUIRED
    )

    find_path(
        RE_ASSIMP_INCLUDE_DIR
        NAMES assimp/Importer.hpp
        PATHS
            /usr/local/include
        NO_DEFAULT_PATH
        REQUIRED
    )

    message(
        STATUS
        "Rendering Engine static Assimp library: "
        "${RE_ASSIMP_STATIC_LIBRARY}"
    )

    message(
        STATUS
        "Rendering Engine Assimp include directory: "
        "${RE_ASSIMP_INCLUDE_DIR}"
    )

    find_package(Threads REQUIRED)

    find_file(
        RE_GLFW_STATIC_LIBRARY
        NAMES glfw3 glfw libglfw3.a
        PATHS
            /usr/local/lib
            /usr/local/lib64
        NO_DEFAULT_PATH
        REQUIRED
    )

    find_path(
        RE_GLFW_INCLUDE_DIR
        NAMES GLFW/glfw3.h
        PATHS
            /usr/local/include
        NO_DEFAULT_PATH
        REQUIRED
    )

    add_library(RE_GLFW_STATIC STATIC IMPORTED GLOBAL)

    set_target_properties(
        RE_GLFW_STATIC
        PROPERTIES
            IMPORTED_LOCATION "${RE_GLFW_STATIC_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${RE_GLFW_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES
                "Threads::Threads;${CMAKE_DL_LIBS};m;rt"
    )

    set(RE_GLFW_LINK_TARGET RE_GLFW_STATIC)

    message(STATUS "GLFW static library: ${RE_GLFW_STATIC_LIBRARY}")
    message(STATUS "GLFW include directory: ${RE_GLFW_INCLUDE_DIR}")
endif()

if(WIN32)
    find_package(harfbuzz CONFIG REQUIRED)
    find_package(freetype CONFIG REQUIRED)
    find_package(assimp CONFIG REQUIRED)
    find_package(glfw3 CONFIG REQUIRED)
    set(RE_GLFW_LINK_TARGET glfw)
endif()

find_package(nlohmann_json CONFIG QUIET)

add_subdirectory(RenderingEngine)
if(NOT RE_BUILD_ENGINE_ONLY)
add_subdirectory(ContentExamples)
add_subdirectory(TestApplications)
add_subdirectory(UserApplications)
endif()

# ======================================================================
# SDK INSTALLATION LAYOUT
# ======================================================================

set(CMAKE_INSTALL_INCLUDEDIR "RenderingEngine/RenderingLibrary/Include")
set(CMAKE_INSTALL_LIBDIR     "RenderingEngine/RenderingLibrary/Library")
set(CMAKE_INSTALL_BINDIR     "RenderingEngine/RenderingLibrary/Library")

# ======================================================================
# Install Public Engine Headers
# ======================================================================
install(
    DIRECTORY RenderingEngine/RenderingLibrary/Include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# ======================================================================
# Install Vendored Headers
# ======================================================================
find_path(GLM_INCLUDE_DIR
          NAMES glm/glm.hpp
          REQUIRED)

if(NOT GLM_INCLUDE_DIR)
    message(FATAL_ERROR "GLM headers not found on this system.")
endif()

message(STATUS "GLM include dir: ${GLM_INCLUDE_DIR}")
install(
    DIRECTORY ${GLM_INCLUDE_DIR}/glm
    DESTINATION RenderingEngine/External/glm
)

# ======================================================================
# Install Engine Library (dll/lib/so)
# ======================================================================
install(
    TARGETS RenderingEngine
    EXPORT  RenderingEngineTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# ======================================================================
# Install MaterialCompiler
# ======================================================================
install(
    TARGETS MaterialCompiler
    RUNTIME DESTINATION RenderingEngine/MaterialCompiler
    LIBRARY DESTINATION RenderingEngine/MaterialCompiler
)

if(WIN32)
    install(
        FILES $<TARGET_FILE:RenderingEngine>
        DESTINATION RenderingEngine/MaterialCompiler
    )
endif()

# ======================================================================
# Install CMake package
# ======================================================================

# Export Engine Targets
install(
    EXPORT RenderingEngineTargets
    FILE RenderingEngineTargets.cmake
    NAMESPACE RenderingEngine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RenderingEngine
)

# Configure Package Config
include(CMakePackageConfigHelpers)

configure_package_config_file(
    RenderingEngine/RenderingLibrary/RenderingEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/RenderingEngineConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RenderingEngine
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/RenderingEngineConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RenderingEngine
)
