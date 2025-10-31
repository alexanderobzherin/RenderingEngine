cmake_minimum_required(VERSION 3.10)

#set header files
include_directories(${PROJECT_SOURCE_DIR}/RenderingEngine/Include/)
include_directories(${PATH_TO_SOURCE_SHADER_PATH}/)

set(HEADER_FILES
	# Shaders
	${PROJECT_SOURCE_DIR}/Shaders/BasicTexture3D/BasicTexture3D.vert
	${PROJECT_SOURCE_DIR}/Shaders/BasicTexture3D/BasicTexture3D.frag
	${PROJECT_SOURCE_DIR}/Shaders/FlatColorFiltering/FlatColorFiltering.vert
	${PROJECT_SOURCE_DIR}/Shaders/FlatColorFiltering/FlatColorFiltering.frag
	${PROJECT_SOURCE_DIR}/Shaders/TestTriangle/TestTriangle.vert
	${PROJECT_SOURCE_DIR}/Shaders/TestTriangle/TestTriangle.frag
	${PROJECT_SOURCE_DIR}/Shaders/TestSprite2D/TestSprite2D.vert
	${PROJECT_SOURCE_DIR}/Shaders/TestSprite2D/TestSprite2D.frag

	# Windowing system level
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/test_triangle.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/test_sprite2D.hpp

	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/i_window_system.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/standalone_window_system.hpp

	# Application level
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/i_application.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/core_application.hpp

	# Renderers
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/i_renderer.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/i_renderer_observer.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/_vulkan_renderer.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/vulkan_renderer.hpp
	
	# Render resources manegment
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/render_resource_context.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/i_render_resources.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/vulkan_render_resources.hpp
	
	# Material resources manegment
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/material_cache.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/material_types.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/material.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/i_material_render_resources.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/vulkan_material_resources.hpp
	
	# Texture resources manegment
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/texture_cache.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/i_texture_render_resources.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/vulkan_texture_resources.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/image_data.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/image_data_gpu.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/image_codec_jpeg.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/image_codec_png.hpp
	
	# Mesh resources manegment
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/model_cache.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/mesh_data_gpu.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/i_mesh_render_resources.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/vulkan_mesh_resources.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/vertex_declarations.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/mesh.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/model.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/model_material.hpp
	
	# Drawable entities
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/drawable_component.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/_drawable_component.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/drawable_2d.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/drawable_3d.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/quad_2d.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/static_mesh.hpp
	
	# Scene organization
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/scene.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/scene_manager.hpp

	# Miscalenous
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/vector_definitions.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/camera.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/scene_component.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/scene_component_2d.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/camera_2d.hpp

	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/utility.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/text_renderer.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/texture_atlas_maker.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/app_time.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/app_clock.hpp
	
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/_renderer_base.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/_vulkan_image_data.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/_vulkan_drawable_component.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/actor.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/_vulkan_vertex_declarations.hpp
)

#set source files
set(SRC_FILES
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/test_triangle.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/test_sprite2D.cpp
	# Windowing system level
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/standalone_window_system.cpp
	
	# Application level
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/core_application.cpp
	
	# Renderers
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/_vulkan_renderer.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/vulkan_renderer.cpp

	# Render resources manegment
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/vulkan_render_resources.cpp
	
	# Material resources manegment
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/material_cache.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/material.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/vulkan_material_resources.cpp
	
	# Texture resources manegment
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/texture_cache.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/vulkan_texture_resources.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/image_data.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/image_data_gpu.cpp
	
	# Mesh resources manegment
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/model_cache.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/mesh_data_gpu.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/vulkan_mesh_resources.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/mesh.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/model.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/model_material.cpp

	# Drawable entities
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/drawable_component.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/_drawable_component.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/drawable_2d.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/drawable_3d.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/quad_2d.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/static_mesh.cpp

	# Scene organization
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/scene.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/scene_manager.cpp

	# Miscalenous
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/camera.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/scene_component.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/scene_component_2d.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/camera_2d.cpp

    ${PROJECT_SOURCE_DIR}/RenderingEngine/Source/utility.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/text_renderer.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/texture_atlas_maker.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/app_time.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/app_clock.cpp
	
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/_vulkan_image_data.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/_renderer_base.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/_vulkan_drawable_component.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/actor.cpp
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
${CMAKE_CURRENT_SOURCE_DIR}/Shaders
)

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

set(test_sprite2D_vert_source "${PATH_TO_SOURCE_SHADER_PATH}/TestSprite2D/TestSprite2D.vert")
set(test_sprite2D_vert_spv "${PATH_TO_COMPILED_SHADER_PATH}/TestSprite2D/TestSprite2D_vert.spv")
set(test_sprite2D_frag_source "${PATH_TO_SOURCE_SHADER_PATH}/TestSprite2D/TestSprite2D.frag")
set(test_sprite2D_frag_spv "${PATH_TO_COMPILED_SHADER_PATH}/TestSprite2D/TestSprite2D_frag.spv")

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

	COMMAND cmd /c ${SHADER_COMPILER} ${test_sprite2D_vert_source} -o ${test_sprite2D_vert_spv}
	COMMAND cmd /c ${SHADER_COMPILER} ${test_sprite2D_frag_source} -o ${test_sprite2D_frag_spv}
)
endif()
