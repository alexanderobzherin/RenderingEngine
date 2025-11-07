# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2025 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
cmake_minimum_required(VERSION 3.10)

#set the project name
project(HelloWinApp LANGUAGES CXX)

#set header files
set(HEADER_FILES
	${PROJECT_SOURCE_DIR}/Include/level_register.hpp
	${PROJECT_SOURCE_DIR}/Include/start_level.hpp
)

#set source files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/Source/main.cpp
	${PROJECT_SOURCE_DIR}/Source/start_level.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/Include/)
include_directories(${PROJECT_SOURCE_DIR}/Assets/Shaders/)

if(WIN32)
	include_directories(${CMAKE_SOURCE_DIR}/RenderingEngine/RenderingLibrary/Include/)
endif()

find_package(Boost 1.36.0 REQUIRED COMPONENTS filesystem)

if(UNIX)
set(RENDERING_ENGINE_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/../../Build/Installed/Library/cmake/RenderingEngine"
    CACHE PATH "Path to installed RenderingEngine SDK"
)
list(APPEND CMAKE_PREFIX_PATH "${RENDERING_ENGINE_ROOT}")
find_package(RenderingEngine REQUIRED)
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

#add the executable
add_executable(HelloWinApp ${HEADER_FILES} ${SRC_FILES})

target_include_directories(HelloWinApp PUBLIC
${PROJECT_SOURCE_DIR}/Assets/Shaders/
)
if(UNIX)
set_target_properties(HelloWinApp PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${PROJECT_BINARY_DIR}/Debug"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_BINARY_DIR}/Release"
)
target_link_libraries(HelloWinApp 
	PRIVATE
		RenderingEngine::RenderingEngine 
		Boost::filesystem
)
elseif(WIN32)
set_target_properties(HelloWinApp PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)
target_link_libraries(HelloWinApp 
	PRIVATE
		RenderingEngine
)
endif()

set(PATH_TO_COMPILED_SHADER_PATH "${PROJECT_BINARY_DIR}/Intermediate/Shaders")
set(SHADER_COMPILER $ENV{glslc})

# Shader compilation
set(BUILDIN_SOURCE_SHADER_PATH "${PROJECT_SOURCE_DIR}/Assets/Shaders")
set(PATH_TO_SOURCE_TEXTURES "${PROJECT_SOURCE_DIR}/Assets/Textures")
set(PATH_TO_SOURCE_MODELS "${PROJECT_SOURCE_DIR}/Assets/Models")
set(PATH_TO_SOURCE_FONTS "${PROJECT_SOURCE_DIR}/Assets/Fonts")
file(COPY ${PATH_TO_SOURCE_TEXTURES} DESTINATION "${PROJECT_BINARY_DIR}/Intermediate")
file(COPY ${PATH_TO_SOURCE_MODELS} DESTINATION "${PROJECT_BINARY_DIR}/Intermediate")
file(COPY ${PATH_TO_SOURCE_FONTS} DESTINATION "${PROJECT_BINARY_DIR}/Intermediate")
file(COPY ${BUILDIN_SOURCE_SHADER_PATH} DESTINATION "${PROJECT_BINARY_DIR}/Intermediate")
set(PATH_TO_SOURCE_SHADER "${PROJECT_BINARY_DIR}/Intermediate/Shaders")

#Set paths to buildin shaders
set(basic_texture_3d_vert_source "${PATH_TO_SOURCE_SHADER}/BasicTexture3D/BasicTexture3D.vert")
set(basic_texture_3d_vert_spv "${PATH_TO_COMPILED_SHADER_PATH}/BasicTexture3D/BasicTexture3D_vert.spv")
set(basic_texture_3d_frag_source "${PATH_TO_SOURCE_SHADER}/BasicTexture3D/BasicTexture3D.frag")
set(basic_texture_3d_frag_spv "${PATH_TO_COMPILED_SHADER_PATH}/BasicTexture3D/BasicTexture3D_frag.spv")

set(flat_color_filtering_vert_source "${PATH_TO_SOURCE_SHADER}/FlatColorFiltering/FlatColorFiltering.vert")
set(flat_color_filtering_vert_spv "${PATH_TO_COMPILED_SHADER_PATH}/FlatColorFiltering/FlatColorFiltering_vert.spv")
set(flat_color_filtering_frag_source "${PATH_TO_SOURCE_SHADER}/FlatColorFiltering/FlatColorFiltering.frag")
set(flat_color_filtering_frag_spv "${PATH_TO_COMPILED_SHADER_PATH}/FlatColorFiltering/FlatColorFiltering_frag.spv")

set(test_triangle_vert_source "${PATH_TO_SOURCE_SHADER}/TestTriangle/TestTriangle.vert")
set(test_triangle_vert_spv "${PATH_TO_COMPILED_SHADER_PATH}/TestTriangle/TestTriangle_vert.spv")
set(test_triangle_frag_source "${PATH_TO_SOURCE_SHADER}/TestTriangle/TestTriangle.frag")
set(test_triangle_frag_spv "${PATH_TO_COMPILED_SHADER_PATH}/TestTriangle/TestTriangle_frag.spv")

set(quad2D_vert_source "${PATH_TO_SOURCE_SHADER}/Quad2D/Quad2D.vert")
set(quad2D_vert_spv "${PATH_TO_COMPILED_SHADER_PATH}/Quad2D/Quad2D_vert.spv")
set(quad2D_frag_source "${PATH_TO_SOURCE_SHADER}/Quad2D/Quad2D.frag")
set(quad2D_frag_spv "${PATH_TO_COMPILED_SHADER_PATH}/Quad2D/Quad2D_frag.spv")

if(WIN32)
add_custom_command(
	TARGET HelloWinApp 
	PRE_BUILD
	# Compile buildin shaders
	COMMAND cmd /c ${SHADER_COMPILER} ${basic_texture_3d_vert_source} -o ${basic_texture_3d_vert_spv}
	COMMAND cmd /c ${SHADER_COMPILER} ${basic_texture_3d_frag_source} -o ${basic_texture_3d_frag_spv}

	COMMAND cmd /c ${SHADER_COMPILER} ${flat_color_filtering_vert_source} -o ${flat_color_filtering_vert_spv}
	COMMAND cmd /c ${SHADER_COMPILER} ${flat_color_filtering_frag_source} -o ${flat_color_filtering_frag_spv}

	COMMAND cmd /c ${SHADER_COMPILER} ${test_triangle_vert_source} -o ${test_triangle_vert_spv}
	COMMAND cmd /c ${SHADER_COMPILER} ${test_triangle_frag_source} -o ${test_triangle_frag_spv}

	COMMAND cmd /c ${SHADER_COMPILER} ${quad2D_vert_source} -o ${quad2D_vert_spv}
	COMMAND cmd /c ${SHADER_COMPILER} ${quad2D_frag_source} -o ${quad2D_frag_spv}

	# Compile custom user shader from json files via calling MaterialCompiler
	# TO DO
)
endif()