cmake_minimum_required(VERSION 3.10)

#set the project name
project(VulkanProject_gtests)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)
set(TESTES_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/Source/image_data.hpp
    ${CMAKE_SOURCE_DIR}/Source/image_data.cpp
)
include_directories(${GTEST_INCLUDE_DIRS}
)
#set source files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/image_data_tests.cpp
    ${TESTES_SOURCE_FILES}
)

#add the executable
add_executable(VulkanProject_gtests  ${SRC_FILES})

target_link_libraries(VulkanProject_gtests 
    PUBLIC
        ${Vulkan_LIBRARY}
        ${GTEST_LIBRARIES}
        glfw
        glm::glm
        Boost::filesystem
        Threads::Threads
        ${JPEG_LIBRARIES}
    PRIVATE 
        #GTest::gmock 
        #GTest::gtest 
        #GTest::gmock_main 
        #GTest::gtest_main
)

