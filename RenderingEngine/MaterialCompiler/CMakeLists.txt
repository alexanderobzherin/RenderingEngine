# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2025 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
cmake_minimum_required(VERSION 3.10)

#set the project name
project(MaterialCompiler)

#set header files
set(HEADER_FILES
	#${PROJECT_SOURCE_DIR}/Include/header_file.hpp
)

#set source files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/Source/main.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/Include/)
include_directories(${CMAKE_SOURCE_DIR}/RenderingEngine/RenderingLibrary/Include/)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Shaders/)

#add the executable
add_executable(MaterialCompiler ${HEADER_FILES} ${SRC_FILES})

#specify the compiling properties
set_target_properties(MaterialCompiler PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)
if(NOT WIN32)
set_target_properties(MaterialCompiler PROPERTIES
	COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)
else(
set_target_properties(MaterialCompiler PROPERTIES
	COMPILE_OPTIONS "-Wall;-Wextra"
	#RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
	#set_target_properties( VulkanProject PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}" )
)
)
endif()

target_link_libraries(MaterialCompiler 
	PRIVATE
		RenderingEngine
)





