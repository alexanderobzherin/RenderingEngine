#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace rendering_engine
{
    struct QueueFamilyIndices 
    {
        std::optional<uint32_t> graphicsFamily;

        bool IsComplete()
        {
            return graphicsFamily.has_value();
        }
    };

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
        bool CheckValidationLayerSupport();

        std::vector<const char*> GetRequiredExtensions();
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void* pUserData);
        void SetupDebugMessenger();
        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT const* pCreateInfo, 
                                              VkAllocationCallbacks const* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        void PickPhysicalDevice();
        bool IsDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

        void CreateLogicalDevice();

        protected:
        int const mWidth;
        int const mHeight;
        char const* mTitle;

        GLFWwindow* mWindow;
        VkInstance mInstance;

        VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
        VkDevice mLogicalDevice;

        std::vector<const char*> const mValidationLayers = {"VK_LAYER_KHRONOS_validation"};
        VkDebugUtilsMessengerEXT mDebugMessenger;
    };  
}