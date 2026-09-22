# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
cmake_minimum_required(VERSION 3.16)

project(RenderingEngine_gtests LANGUAGES CXX)

find_package(GTest REQUIRED)

set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/image_data_tests.cpp
    ${PROJECT_SOURCE_DIR}/model_tests.cpp
    ${PROJECT_SOURCE_DIR}/text_renderer_tests.cpp
    ${PROJECT_SOURCE_DIR}/texture_atlas_maker_tests.cpp
    ${PROJECT_SOURCE_DIR}/app_clock_tests.cpp
)

add_executable(RenderingEngine_gtests ${SRC_FILES})

target_link_libraries(RenderingEngine_gtests
    PRIVATE
        RenderingEngine
        GTest::gtest
)

if(WIN32)
    add_custom_command(
        TARGET RenderingEngine_gtests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:RenderingEngine>
            $<TARGET_FILE_DIR:RenderingEngine_gtests>
        COMMENT "Copying RenderingEngine runtime library for unit tests"
    )
endif()

include(GoogleTest)
gtest_discover_tests(RenderingEngine_gtests)
