#pragma once

#include <iostream>
#include <cstdint>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace rendering_engine
{
    class RenderingApplication
    {
        public:
        //Public constructor
        RenderingApplication( int const width, int const height, char const* title );
        virtual ~RenderingApplication();

        //Interface to run rendering application
        virtual void Run();

        protected:
        void Shutdown();
        void InitializeWindow();
        void InitializeVulkan();
        void CreateInstance();

        protected:
        int const mWidth;
        int const mHeight;
        char const* mTitle;

        GLFWwindow* mWindow;
        VkInstance mInstance;
    };  
}