cmake_minimum_required(VERSION 3.10)

#set the project name
project(RenderingEngineProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
#Set path to output binaries
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(PATH_TO_SOURCE_SHADER_PATH "${PROJECT_SOURCE_DIR}/Shaders")
set(PATH_TO_SOURCE_TEXTURES "${PROJECT_SOURCE_DIR}/Textures")
set(PATH_TO_SOURCE_MODELS "${PROJECT_SOURCE_DIR}/Models")
set(PATH_TO_SOURCE_FONTS "${PROJECT_SOURCE_DIR}/Fonts")
file(COPY ${PATH_TO_SOURCE_TEXTURES} DESTINATION "${CMAKE_BINARY_DIR}/Intermediate")
file(COPY ${PATH_TO_SOURCE_MODELS} DESTINATION "${CMAKE_BINARY_DIR}/Intermediate")
file(COPY ${PATH_TO_SOURCE_FONTS} DESTINATION "${CMAKE_BINARY_DIR}/Intermediate")
if(WIN32)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Intermediate/Shaders/BasicTexture3D)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Intermediate/Shaders/FlatColorFiltering)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Intermediate/Shaders/TestTriangle)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Intermediate/Shaders/TestSprite2D)

set(PATH_TO_COMPILED_SHADER_PATH "${CMAKE_BINARY_DIR}/Intermediate/Shaders")
set(SHADER_COMPILER $ENV{glslc})
endif()
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

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
add_subdirectory(Tests)
add_subdirectory(DemoApplication)