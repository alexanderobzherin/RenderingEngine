# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
cmake_minimum_required(VERSION 3.16)

#set the project name
project(RenderingLibrary VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} LANGUAGES CXX)

#set header files
include_directories(${PROJECT_SOURCE_DIR}/Include/)

set(HEADER_FILES
	${PROJECT_SOURCE_DIR}/Include/rendering_engine_export.hpp
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
	${PROJECT_SOURCE_DIR}/Include/sprite_2d.hpp
	${PROJECT_SOURCE_DIR}/Include/static_mesh.hpp
	
	# Text rendering system
	${PROJECT_SOURCE_DIR}/Include/font_resources.hpp
	${PROJECT_SOURCE_DIR}/Include/text_renderer.hpp
	${PROJECT_SOURCE_DIR}/Include/text_block_2d.hpp
	${PROJECT_SOURCE_DIR}/Include/texture_atlas_maker.hpp

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
	${PROJECT_SOURCE_DIR}/Source/sprite_2d.cpp
	${PROJECT_SOURCE_DIR}/Source/static_mesh.cpp

	# Text rendering system
	${PROJECT_SOURCE_DIR}/Source/font_resources.cpp
	${PROJECT_SOURCE_DIR}/Source/text_renderer.cpp
	${PROJECT_SOURCE_DIR}/Source/text_block_2d.cpp
	${PROJECT_SOURCE_DIR}/Source/texture_atlas_maker.cpp

	# Scene organization
	${PROJECT_SOURCE_DIR}/Source/scene.cpp
	${PROJECT_SOURCE_DIR}/Source/scene_manager.cpp

	# Miscalenous
	${PROJECT_SOURCE_DIR}/Source/camera.cpp
	${PROJECT_SOURCE_DIR}/Source/scene_component.cpp
	${PROJECT_SOURCE_DIR}/Source/scene_component_2d.cpp
	${PROJECT_SOURCE_DIR}/Source/camera_2d.cpp

    ${PROJECT_SOURCE_DIR}/Source/utility.cpp
	${PROJECT_SOURCE_DIR}/Source/app_time.cpp
	${PROJECT_SOURCE_DIR}/Source/app_clock.cpp
	
	${PROJECT_SOURCE_DIR}/Source/actor.cpp
)


if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

#add shared library
add_library(RenderingEngine SHARED ${HEADER_FILES} ${SRC_FILES})
target_compile_definitions(RenderingEngine PRIVATE RENDERING_ENGINE_BUILD)

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
		RUNTIME_OUTPUT_DIRECTORY           "${CMAKE_BINARY_DIR}"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG     "${CMAKE_BINARY_DIR}"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE   "${CMAKE_BINARY_DIR}"
	)
)
endif()

# Engine public headers (visible to SDK user)
target_include_directories(RenderingEngine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>
        $<INSTALL_INTERFACE:RenderingEngine/RenderingLibrary/Include>
        $<INSTALL_INTERFACE:RenderingEngine/External/glm>
        $<INSTALL_INTERFACE:RenderingEngine/External/boost>
)

if(UNIX)
	target_include_directories(RenderingEngine PUBLIC
	${freetype2_INCLUDE_DIRS}
	)
endif()


target_link_libraries(RenderingEngine
    PRIVATE
		Vulkan::Vulkan
		glfw
		freetype
		harfbuzz::harfbuzz
		JPEG::JPEG
		PNG::PNG
		assimp::assimp
		glm::glm
		Boost::filesystem
		nlohmann_json::nlohmann_json
)

if(WIN32)
    target_link_libraries(RenderingEngine 
	PRIVATE 
	freetype
	harfbuzz::harfbuzz)
else()
    target_link_libraries(RenderingEngine PRIVATE ${FREETYPE2_LIBRARIES})
endif()

