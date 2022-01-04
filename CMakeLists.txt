cmake_minimum_required(VERSION 3.10)

#set the project name
project(VulkanProject)

set(CMAKE_BUILD_TYPE Debug)

#set header files
set(HEADER_FILES
	Source/rendering_application.hpp
	Source/utility.hpp
)

#set source files
set(SRC_FILES
Source/main.cpp
Source/rendering_application.cpp
Source/utility.cpp
)

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

if(WIN32)
add_custom_command(
	TARGET VulkanProject PRE_BUILD
	COMMAND cmd /c ${CMAKE_CURRENT_SOURCE_DIR}/windows_compile_shaders.bat
)
endif()