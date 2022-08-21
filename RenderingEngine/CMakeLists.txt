cmake_minimum_required(VERSION 3.10)

#set header files
include_directories(${PROJECT_SOURCE_DIR}/RenderingEngine/Include/)
include_directories(${PATH_TO_SOURCE_SHADER_PATH}/)

set(HEADER_FILES
	${PROJECT_SOURCE_DIR}/Shaders/basic_shader.frag
	${PROJECT_SOURCE_DIR}/Shaders/basic_shader.vert
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/rendering_application.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/utility.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/vertex_declarations.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/image_data.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/image_codec_jpeg.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/image_codec_png.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/mesh.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/model.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/model_material.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/text_renderer.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/texture_atlas_maker.hpp
)

#set files used as engine API
set(API_HEADER_FILES
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/mesh.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/model.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/model_material.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/rendering_application.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/utility.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/vertex_declarations.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/image_data.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/text_renderer.hpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Include/texture_atlas_maker.hpp
)

#set source files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/RenderingEngine/Source/rendering_application.cpp
    ${PROJECT_SOURCE_DIR}/RenderingEngine/Source/utility.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/image_data.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/mesh.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/model.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/model_material.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/text_renderer.cpp
	${PROJECT_SOURCE_DIR}/RenderingEngine/Source/texture_atlas_maker.cpp
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
${CMAKE_CURRENT_SOURCE_DIR}/Shaders
)

#Set path to shaders
set(shader_vert_source "${PATH_TO_SOURCE_SHADER_PATH}/basic_shader.vert")
set(shader_vert_spv "${PATH_TO_COMPILED_SHADER_PATH}/basic_shader_vert.spv")
set(shader_frag_source "${PATH_TO_SOURCE_SHADER_PATH}/basic_shader.frag")
set(shader_frag_spv "${PATH_TO_COMPILED_SHADER_PATH}/basic_shader_frag.spv")

if(WIN32)
add_custom_command(
	TARGET RenderingEngine 
	PRE_BUILD
	COMMAND cmd /c ${SHADER_COMPILER} ${shader_vert_source} -o ${shader_vert_spv}
	COMMAND cmd /c ${SHADER_COMPILER} ${shader_frag_source} -o ${shader_frag_spv}
)
endif()
