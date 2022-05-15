cmake_minimum_required(VERSION 3.10)

#set the project name
project(VulkanProject_gtests)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
#set source files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/main.cpp
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
    PRIVATE 
        #GTest::gmock 
        #GTest::gtest 
        #GTest::gmock_main 
        #GTest::gtest_main
)

