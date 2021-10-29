#pragma once

#include <iostream>
#include <cstdint>

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
        RenderingApplication( int const width, int const height );
        virtual ~RenderingApplication();

        //Interface to run rendering application
        virtual void Run();

        protected:
        virtual void InitializeWindow();

        protected:
        int const mWidth;
        int const mHeight;

        GLFWwindow* mWindow;
    };  
}