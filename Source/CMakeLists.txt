cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

#set header files
set(HEADER_FILES
	${PROJECT_SOURCE_DIR}/Shaders/basic_shader.frag
	${PROJECT_SOURCE_DIR}/Shaders/basic_shader.vert
	${PROJECT_SOURCE_DIR}/Source/rendering_application.hpp
	${PROJECT_SOURCE_DIR}/Source/utility.hpp
	${PROJECT_SOURCE_DIR}/Source/vertex_declarations.hpp
)

#set source files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/Source/main.cpp
    ${PROJECT_SOURCE_DIR}/Source/rendering_application.cpp
    ${PROJECT_SOURCE_DIR}/Source/utility.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Shaders/)
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

#add the executable
add_executable(VulkanProject ${HEADER_FILES} ${SRC_FILES})

#specify the compiling properties
set_target_properties(VulkanProject PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)
if(NOT WIN32)
set_target_properties(VulkanProject PROPERTIES
	COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
else(
	COMPILE_OPTIONS "-Wall;-Wextra"
)
endif()

target_link_libraries(VulkanProject 
    PUBLIC
    ${Vulkan_LIBRARY}
    glfw
    glm::glm
	Boost::filesystem
)
target_include_directories(VulkanProject PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/Shaders
)

#Set path to shaders
set(shader_vert_source_origin "${PATH_TO_SOURCE_SHADER_PATH}/basic_shader.vert")
cmake_path(CONVERT ${shader_vert_source_origin} TO_NATIVE_PATH_LIST shader_vert_source NORMALIZE)

set(shader_vert_spv_origin "${PATH_TO_COMPILED_SHADER_PATH}/basic_shader_vert.spv")
cmake_path(CONVERT ${shader_vert_spv_origin} TO_NATIVE_PATH_LIST shader_vert_spv NORMALIZE)

set(shader_frag_source_origin "${PATH_TO_SOURCE_SHADER_PATH}/basic_shader.frag")
cmake_path(CONVERT ${shader_frag_source_origin} TO_NATIVE_PATH_LIST shader_frag_source NORMALIZE)

set(shader_frag_spv_origin "${PATH_TO_COMPILED_SHADER_PATH}/basic_shader_frag.spv")
cmake_path(CONVERT ${shader_frag_spv_origin} TO_NATIVE_PATH_LIST shader_frag_spv NORMALIZE)

if(WIN32)
add_custom_command(
	TARGET VulkanProject PRE_BUILD
	COMMAND cmd /c "${SHADER_COMPILER} ${shader_vert_source} -o ${shader_vert_spv}" 
	COMMAND cmd /c "${SHADER_COMPILER} ${shader_frag_source} -o ${shader_frag_spv}"
	COMMENT "Shaders compiling..."
)
endif()


