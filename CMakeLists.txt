# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2025 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.

cmake_minimum_required(VERSION 3.10)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")

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

if(UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -g0 -s")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

option(RE_BUILD_ENGINE_ONLY "" OFF)

# Use FindVulkan module added with CMAKE 3.7
if (NOT CMAKE_VERSION VERSION_LESS 3.7.0)
	message(STATUS "Using module to find Vulkan")
	find_package(Vulkan)
endif()

IF(WIN32)
	IF (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS ${CMAKE_SOURCE_DIR}/libs/vulkan)
		IF (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			MESSAGE("Using bundled Vulkan library version")
		ENDIF()
	ENDIF()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
ELSEIF(LINUX)
	IF (NOT Vulkan_FOUND)
		find_library(Vulkan_LIBRARY NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib" "${CMAKE_SOURCE_DIR}/libs/vulkan" REQUIRED)
		IF (Vulkan_LIBRARY)
			set(Vulkan_FOUND ON)
			MESSAGE("Using bundled Vulkan library version")
		ENDIF()
	ENDIF()
ENDIF()

IF (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
ELSE()
	message(STATUS ${Vulkan_LIBRARY})
ENDIF()

find_package(glfw3 CONFIG REQUIRED)

find_package(glm CONFIG REQUIRED)

find_package(Boost 1.36.0 REQUIRED COMPONENTS filesystem)

find_package(GTest REQUIRED)

find_package(JPEG REQUIRED)

find_package(PNG REQUIRED)

find_package(assimp CONFIG REQUIRED)

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
