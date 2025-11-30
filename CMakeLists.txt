# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2025 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.

cmake_minimum_required(VERSION 3.16)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "3")

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

find_package(Boost REQUIRED COMPONENTS filesystem)

find_package(GTest REQUIRED)

set(JPEG_STATIC ON)
find_package(JPEG REQUIRED)

set(PNG_STATIC ON)
find_package(PNG REQUIRED)

find_package(assimp CONFIG REQUIRED)

find_package(nlohmann_json CONFIG QUIET)

if(WIN32)
find_package(freetype CONFIG REQUIRED)
else()
find_package(PkgConfig)
pkg_check_modules(freetype2 REQUIRED freetype2)
include_directories(${FREETYPE2_INCLUDE_DIRS})
message(STATUS "freetype2: found ${freetype2_FOUND}")
message(STATUS "freetype2 DIR:  ${freetype2_INCLUDE_DIRS}")
message(STATUS "freetype2 LIBRARIES:  ${freetype2_LIBRARIES}")
message(STATUS "freetype2 LINK LIBRARIES:  ${freetype2_LINK_LIBRARIES}")
endif()

add_subdirectory(RenderingEngine)
#add_subdirectory(ContentExamples)
if(NOT RE_BUILD_ENGINE_ONLY)
add_subdirectory(TestApplications)
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

find_path(BOOST_FS_INCLUDE_DIR
          NAMES boost/filesystem.hpp
          REQUIRED)

if(NOT BOOST_FS_INCLUDE_DIR)
    message(FATAL_ERROR "Boost filesystem headers not found on this system.")
endif()

message(STATUS "Boost include dir: ${BOOST_FS_INCLUDE_DIR}")
install(
    DIRECTORY ${BOOST_FS_INCLUDE_DIR}/boost
    DESTINATION RenderingEngine/External/boost
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
