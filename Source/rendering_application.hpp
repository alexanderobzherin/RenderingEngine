#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <optional>
#include <fstream>

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
        std::optional<uint32_t> presentFamily;

        bool IsComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
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
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
                
        void PickPhysicalDevice();
        bool IsDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

        void CreateLogicalDevice();
        void CreateSurface();
        bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);


        void CreateSwapChain();
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const & availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(std::vector<VkPresentModeKHR>const & availablePresentModes);
        VkExtent2D ChooseSwapExtent( VkSurfaceCapabilitiesKHR const & capabilities );

        void CreateImageView();

        void CreateGraphicsPipeline();

        static std::vector<char> ReadFile( std::string const & filename )
        {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if (!file.is_open()) 
            {
                throw std::runtime_error("failed to open file!");
            }

            size_t fileSize = (size_t) file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();

            return buffer;
        }

        VkShaderModule CreateShaderModule(std::vector<char>& code);

        protected:
        int const mWidth;
        int const mHeight;
        char const* mTitle;

        VkSurfaceKHR mSurface;
        GLFWwindow* mWindow;
        VkInstance mInstance;

        VkQueue mPresentQueue;

        VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
        VkDevice mLogicalDevice;

        std::vector<const char*> const mValidationLayers = {"VK_LAYER_KHRONOS_validation"};
        VkDebugUtilsMessengerEXT mDebugMessenger;

        std::vector<const char*> const mDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkSwapchainKHR mSwapChain;
        std::vector<VkImage> mSwapChainImages;
        VkFormat mSwapChainImageFormat;
        VkExtent2D mSwapChainExtent;

        std::vector<VkImageView> mSwapChainImageViews;

        VkPipelineLayout mPipelineLayout;
    };  
}