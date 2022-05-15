cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

#set source files
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/Tests/main.cpp
)

#add the executable
add_executable(VulkanProject_gtests  ${SRC_FILES})
#specify the compiling properties

target_link_libraries(VulkanProject_gtests 
    PUBLIC
        ${Vulkan_LIBRARY}
        glfw
        glm::glm
        Boost::filesystem
    PRIVATE 
        GTest::gmock 
        GTest::gtest 
        GTest::gmock_main 
        GTest::gtest_main
)

