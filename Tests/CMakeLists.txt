cmake_minimum_required(VERSION 3.10)

#set the project name
project(RenderingEngine_gtests)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/RenderingEngine/Include/)

include_directories(${GTEST_INCLUDE_DIRS}
)
#set source files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/image_data_tests.cpp
)

#add the executable
add_executable(RenderingEngine_gtests  ${SRC_FILES})

target_link_libraries(RenderingEngine_gtests 
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
        freetype
        #GTest::gmock 
        #GTest::gtest 
        #GTest::gmock_main 
        #GTest::gtest_main
)

