cmake_minimum_required(VERSION 3.10)

#set the project name
project(DemoApplication)

#set header files
set(HEADER_FILES
)

#set source files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/Source/main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/RenderingEngine/Include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Shaders/)
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

#add the executable
add_executable(DemoApplication ${HEADER_FILES} ${SRC_FILES})

#specify the compiling properties
set_target_properties(DemoApplication PROPERTIES
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
	set_target_properties( VulkanProject PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}" )
)
)
endif()

target_link_libraries(DemoApplication 
	PUBLIC
		RenderingEngine
	PRIVATE
		${Vulkan_LIBRARY}
		${GTEST_LIBRARIES}
		glfw
		glm::glm
		Boost::filesystem
		Threads::Threads
		${JPEG_LIBRARIES}
		PNG::PNG
		assimp::assimp
)
target_include_directories(DemoApplication PUBLIC
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


