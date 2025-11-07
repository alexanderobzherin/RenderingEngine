# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2025 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
cmake_minimum_required(VERSION 3.10)

#set the project name
project(RenderingLibrary VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} LANGUAGES CXX)

include(GNUInstallDirs)
set(CMAKE_INSTALL_LIBDIR "Library")
set(CMAKE_INSTALL_INCLUDEDIR "Include")
set(CMAKE_INSTALL_BINDIR "Binary")

#set header files
include_directories(${PROJECT_SOURCE_DIR}/Include/)


#include_directories(${PATH_TO_SOURCE_SHADER_PATH}/BuiltInAssets/Shaders)

set(HEADER_FILES
#[===[
	# Shaders
	${PROJECT_SOURCE_DIR}/BuiltInAssets/Shaders/BasicTexture3D/BasicTexture3D.vert
	${PROJECT_SOURCE_DIR}/BuiltInAssets/Shaders/BasicTexture3D/BasicTexture3D.frag
	${PROJECT_SOURCE_DIR}/BuiltInAssets/Shaders/FlatColorFiltering/FlatColorFiltering.vert
	${PROJECT_SOURCE_DIR}/BuiltInAssets/Shaders/FlatColorFiltering/FlatColorFiltering.frag
	${PROJECT_SOURCE_DIR}/BuiltInAssets/Shaders/Quad2D/Quad2D.vert
	${PROJECT_SOURCE_DIR}/BuiltInAssets/Shaders/Quad2D/Quad2D.frag
]===]

	# Windowing system level
	${PROJECT_SOURCE_DIR}/Include/i_window_system.hpp
	${PROJECT_SOURCE_DIR}/Include/standalone_window_system.hpp

	# Application level
	${PROJECT_SOURCE_DIR}/Include/i_application.hpp
	${PROJECT_SOURCE_DIR}/Include/core_application.hpp

	# Renderers
	${PROJECT_SOURCE_DIR}/Include/i_renderer.hpp
	${PROJECT_SOURCE_DIR}/Include/i_renderer_observer.hpp
	${PROJECT_SOURCE_DIR}/Include/vulkan_renderer.hpp
	
	# Render resources manegment
	${PROJECT_SOURCE_DIR}/Include/render_resource_context.hpp
	${PROJECT_SOURCE_DIR}/Include/i_render_resources.hpp
	${PROJECT_SOURCE_DIR}/Include/vulkan_render_resources.hpp
	
	# Material resources manegment
	${PROJECT_SOURCE_DIR}/Include/material_cache.hpp
	${PROJECT_SOURCE_DIR}/Include/material_types.hpp
	${PROJECT_SOURCE_DIR}/Include/material.hpp
	${PROJECT_SOURCE_DIR}/Include/i_material_render_resources.hpp
	${PROJECT_SOURCE_DIR}/Include/vulkan_material_resources.hpp
	
	# Texture resources manegment
	${PROJECT_SOURCE_DIR}/Include/texture_cache.hpp
	${PROJECT_SOURCE_DIR}/Include/i_texture_render_resources.hpp
	${PROJECT_SOURCE_DIR}/Include/vulkan_texture_resources.hpp
	${PROJECT_SOURCE_DIR}/Include/image_data.hpp
	${PROJECT_SOURCE_DIR}/Include/image_data_gpu.hpp
	${PROJECT_SOURCE_DIR}/Include/image_codec_jpeg.hpp
	${PROJECT_SOURCE_DIR}/Include/image_codec_png.hpp
	
	# Mesh resources manegment
	${PROJECT_SOURCE_DIR}/Include/model_cache.hpp
	${PROJECT_SOURCE_DIR}/Include/mesh_data_gpu.hpp
	${PROJECT_SOURCE_DIR}/Include/i_mesh_render_resources.hpp
	${PROJECT_SOURCE_DIR}/Include/vulkan_mesh_resources.hpp
	${PROJECT_SOURCE_DIR}/Include/vertex_declarations.hpp
	${PROJECT_SOURCE_DIR}/Include/mesh.hpp
	${PROJECT_SOURCE_DIR}/Include/model.hpp
	${PROJECT_SOURCE_DIR}/Include/model_material.hpp
	
	# Drawable entities
	${PROJECT_SOURCE_DIR}/Include/drawable_component.hpp
	${PROJECT_SOURCE_DIR}/Include/drawable_2d.hpp
	${PROJECT_SOURCE_DIR}/Include/drawable_3d.hpp
	${PROJECT_SOURCE_DIR}/Include/quad_2d.hpp
	${PROJECT_SOURCE_DIR}/Include/static_mesh.hpp
	
	# Scene organization
	${PROJECT_SOURCE_DIR}/Include/scene.hpp
	${PROJECT_SOURCE_DIR}/Include/scene_manager.hpp

	# Miscalenous
	${PROJECT_SOURCE_DIR}/Include/vector_definitions.hpp
	${PROJECT_SOURCE_DIR}/Include/camera.hpp
	${PROJECT_SOURCE_DIR}/Include/scene_component.hpp
	${PROJECT_SOURCE_DIR}/Include/scene_component_2d.hpp
	${PROJECT_SOURCE_DIR}/Include/camera_2d.hpp

	${PROJECT_SOURCE_DIR}/Include/utility.hpp
	${PROJECT_SOURCE_DIR}/Include/text_renderer.hpp
	${PROJECT_SOURCE_DIR}/Include/texture_atlas_maker.hpp
	${PROJECT_SOURCE_DIR}/Include/app_time.hpp
	${PROJECT_SOURCE_DIR}/Include/app_clock.hpp
	
	${PROJECT_SOURCE_DIR}/Include/actor.hpp
)

set(RenderingLibrary_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/Include/")
set(RenderingLibrary_LIBRARIES RenderingEngine)

#set source files
set(SRC_FILES
	# Windowing system level
	${PROJECT_SOURCE_DIR}/Source/standalone_window_system.cpp
	
	# Application level
	${PROJECT_SOURCE_DIR}/Source/core_application.cpp
	
	# Renderers
	${PROJECT_SOURCE_DIR}/Source/vulkan_renderer.cpp

	# Render resources manegment
	${PROJECT_SOURCE_DIR}/Source/vulkan_render_resources.cpp
	
	# Material resources manegment
	${PROJECT_SOURCE_DIR}/Source/material_cache.cpp
	${PROJECT_SOURCE_DIR}/Source/material.cpp
	${PROJECT_SOURCE_DIR}/Source/vulkan_material_resources.cpp
	
	# Texture resources manegment
	${PROJECT_SOURCE_DIR}/Source/texture_cache.cpp
	${PROJECT_SOURCE_DIR}/Source/vulkan_texture_resources.cpp
	${PROJECT_SOURCE_DIR}/Source/image_data.cpp
	${PROJECT_SOURCE_DIR}/Source/image_data_gpu.cpp
	
	# Mesh resources manegment
	${PROJECT_SOURCE_DIR}/Source/model_cache.cpp
	${PROJECT_SOURCE_DIR}/Source/mesh_data_gpu.cpp
	${PROJECT_SOURCE_DIR}/Source/vulkan_mesh_resources.cpp
	${PROJECT_SOURCE_DIR}/Source/mesh.cpp
	${PROJECT_SOURCE_DIR}/Source/model.cpp
	${PROJECT_SOURCE_DIR}/Source/model_material.cpp

	# Drawable entities
	${PROJECT_SOURCE_DIR}/Source/drawable_component.cpp
	${PROJECT_SOURCE_DIR}/Source/drawable_2d.cpp
	${PROJECT_SOURCE_DIR}/Source/drawable_3d.cpp
	${PROJECT_SOURCE_DIR}/Source/quad_2d.cpp
	${PROJECT_SOURCE_DIR}/Source/static_mesh.cpp

	# Scene organization
	${PROJECT_SOURCE_DIR}/Source/scene.cpp
	${PROJECT_SOURCE_DIR}/Source/scene_manager.cpp

	# Miscalenous
	${PROJECT_SOURCE_DIR}/Source/camera.cpp
	${PROJECT_SOURCE_DIR}/Source/scene_component.cpp
	${PROJECT_SOURCE_DIR}/Source/scene_component_2d.cpp
	${PROJECT_SOURCE_DIR}/Source/camera_2d.cpp

    ${PROJECT_SOURCE_DIR}/Source/utility.cpp
	${PROJECT_SOURCE_DIR}/Source/text_renderer.cpp
	${PROJECT_SOURCE_DIR}/Source/texture_atlas_maker.cpp
	${PROJECT_SOURCE_DIR}/Source/app_time.cpp
	${PROJECT_SOURCE_DIR}/Source/app_clock.cpp
	
	${PROJECT_SOURCE_DIR}/Source/actor.cpp
)


if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

#add shared library
add_library(RenderingEngine STATIC ${HEADER_FILES} ${SRC_FILES})

#specify the compiling properties
set_target_properties(RenderingEngine PROPERTIES
CXX_STANDARD 17
CXX_STANDARD_REQUIRED ON
)

if(NOT WIN32)
	set_target_properties(RenderingEngine PROPERTIES
		COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
else(
	set_target_properties(RenderingEngine PROPERTIES
		COMPILE_OPTIONS "-Wall;-Wextra"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
	)
)
endif()

target_link_libraries(RenderingEngine 
    PUBLIC
    ${Vulkan_LIBRARY}
    glfw
    glm::glm
	Boost::filesystem
	${JPEG_LIBRARIES}
	PNG::PNG
	assimp::assimp
	freetype
)


target_include_directories(RenderingEngine PUBLIC
${freetype2_INCLUDE_DIRS}
#${CMAKE_CURRENT_SOURCE_DIR}/Shaders
$<INSTALL_INTERFACE:Include>
)

#[===[
#Set path to shaders
set(basic_texture_3d_vert_source "${PATH_TO_SOURCE_SHADER_PATH}/BasicTexture3D/BasicTexture3D.vert")
set(basic_texture_3d_vert_spv "${PATH_TO_COMPILED_SHADER_PATH}/BasicTexture3D/BasicTexture3D_vert.spv")
set(basic_texture_3d_frag_source "${PATH_TO_SOURCE_SHADER_PATH}/BasicTexture3D/BasicTexture3D.frag")
set(basic_texture_3d_frag_spv "${PATH_TO_COMPILED_SHADER_PATH}/BasicTexture3D/BasicTexture3D_frag.spv")

set(flat_color_filtering_vert_source "${PATH_TO_SOURCE_SHADER_PATH}/FlatColorFiltering/FlatColorFiltering.vert")
set(flat_color_filtering_vert_spv "${PATH_TO_COMPILED_SHADER_PATH}/FlatColorFiltering/FlatColorFiltering_vert.spv")
set(flat_color_filtering_frag_source "${PATH_TO_SOURCE_SHADER_PATH}/FlatColorFiltering/FlatColorFiltering.frag")
set(flat_color_filtering_frag_spv "${PATH_TO_COMPILED_SHADER_PATH}/FlatColorFiltering/FlatColorFiltering_frag.spv")

set(test_triangle_vert_source "${PATH_TO_SOURCE_SHADER_PATH}/TestTriangle/TestTriangle.vert")
set(test_triangle_vert_spv "${PATH_TO_COMPILED_SHADER_PATH}/TestTriangle/TestTriangle_vert.spv")
set(test_triangle_frag_source "${PATH_TO_SOURCE_SHADER_PATH}/TestTriangle/TestTriangle.frag")
set(test_triangle_frag_spv "${PATH_TO_COMPILED_SHADER_PATH}/TestTriangle/TestTriangle_frag.spv")

set(quad2D_vert_source "${PATH_TO_SOURCE_SHADER_PATH}/Quad2D/Quad2D.vert")
set(quad2D_vert_spv "${PATH_TO_COMPILED_SHADER_PATH}/Quad2D/Quad2D_vert.spv")
set(quad2D_frag_source "${PATH_TO_SOURCE_SHADER_PATH}/Quad2D/Quad2D.frag")
set(quad2D_frag_spv "${PATH_TO_COMPILED_SHADER_PATH}/Quad2D/Quad2D_frag.spv")

if(WIN32)
add_custom_command(
	TARGET RenderingEngine 
	PRE_BUILD
	COMMAND cmd /c ${SHADER_COMPILER} ${basic_texture_3d_vert_source} -o ${basic_texture_3d_vert_spv}
	COMMAND cmd /c ${SHADER_COMPILER} ${basic_texture_3d_frag_source} -o ${basic_texture_3d_frag_spv}

	COMMAND cmd /c ${SHADER_COMPILER} ${flat_color_filtering_vert_source} -o ${flat_color_filtering_vert_spv}
	COMMAND cmd /c ${SHADER_COMPILER} ${flat_color_filtering_frag_source} -o ${flat_color_filtering_frag_spv}

	COMMAND cmd /c ${SHADER_COMPILER} ${test_triangle_vert_source} -o ${test_triangle_vert_spv}
	COMMAND cmd /c ${SHADER_COMPILER} ${test_triangle_frag_source} -o ${test_triangle_frag_spv}

	COMMAND cmd /c ${SHADER_COMPILER} ${quad2D_vert_source} -o ${quad2D_vert_spv}
	COMMAND cmd /c ${SHADER_COMPILER} ${quad2D_frag_source} -o ${quad2D_frag_spv}
)
endif()

]===]

#[===[
# --- Install the library and headers for SDK use ---
include(GNUInstallDirs)

install(TARGETS RenderingEngine
EXPORT RenderingEngineTargets
ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Public headers (the API)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Include/
DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RenderingEngine
FILES_MATCHING PATTERN "*.hpp"
)

# Generated version header
install(FILES ${CMAKE_BINARY_DIR}/Intermediate/Generated/version.h
DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/RenderingEngine
)

# Export CMake config
install(EXPORT RenderingEngineTargets
FILE RenderingEngineTargets.cmake
NAMESPACE RenderingEngine::
DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RenderingEngine
)

# Generate Config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/RenderingEngineConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
	)
	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/RenderingEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/RenderingEngineConfig.cmake
    @ONLY
	)
	
	install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/RenderingEngineConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/RenderingEngineConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RenderingEngine
)
# End of installation
]===]

### Installation 
# --------------------------
# Install headers
# --------------------------
install(DIRECTORY Include/
        DESTINATION Include
        FILES_MATCHING PATTERN "*.hpp")

# --------------------------
# Install the library
# --------------------------
install(TARGETS RenderingEngine
        EXPORT RenderingEngineTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# --------------------------
# Export the CMake package
# --------------------------
install(EXPORT RenderingEngineTargets
        FILE RenderingEngineTargets.cmake
        NAMESPACE RenderingEngine::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RenderingEngine)

# --------------------------
# Generate a Config file
# --------------------------
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/RenderingEngineConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/RenderingEngineConfig.cmake
    INSTALL_DESTINATION Library/cmake/RenderingEngine
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/RenderingEngineConfig.cmake
    DESTINATION Library/cmake/RenderingEngine
)
### / Installation 