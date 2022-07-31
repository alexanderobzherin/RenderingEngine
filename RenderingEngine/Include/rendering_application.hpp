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
#include "vertex_declarations.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

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
    RenderingApplication(char const* title);
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
    void CleanupSwapChain();
    void RecreateSwapChain();
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const & availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(std::vector<VkPresentModeKHR>const & availablePresentModes);
    VkExtent2D ChooseSwapExtent( VkSurfaceCapabilitiesKHR const & capabilities );

    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, std::uint32_t mipmapLevels);
    void CreateImageViews();
    void CreateRenderPass();
    void CreateGraphicsPipeline();
    void CreateFramebuffers();
    void CreateDepthResources();
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat FindDepthFormat();
    bool HasStencilComponent(VkFormat format);

    VkShaderModule CreateShaderModule(std::vector<char>& code);

    void CreateCommandPool();
    void CreateVulkanImage(uint32_t width, uint32_t height, std::uint32_t mipmapLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                           VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    
    void CreateColorResources();
    void CreateTextureImage();
    void CreateTextureImageView();
    void CreateTextureSampler();
    void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    void CreateCommandBuffers();
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void LoadModel();

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    VkCommandBuffer BeginSingleTimeCommand();
    void EndSingleTimeCommand(VkCommandBuffer commandBuffer);
    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, std::uint32_t mipmapLevels);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSets();

    void CreateDescriptorSetLayout();

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void Draw();
    void UpdateUniformBuffer(uint32_t currentImage);
    void CreateSyncObjects();

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    VkSampleCountFlagBits GetMaxUsableSampleCount();

 protected:
    bool mIsFullScreen;
    unsigned int mWidth;
    unsigned int mHeight;
    char const* mTitle;

    VkSurfaceKHR mSurface;
    GLFWwindow* mWindow;
    VkInstance mInstance;

    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;

    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkDevice mLogicalDevice;

    std::vector<const char*> const mValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    VkDebugUtilsMessengerEXT mDebugMessenger;

    std::vector<const char*> const mDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkPhysicalDeviceFeatures mPhysDevSupportedFeatures;

    VkSwapchainKHR mSwapChain;
    std::vector<VkImage> mSwapChainImages;
    VkFormat mSwapChainImageFormat;
    VkExtent2D mSwapChainExtent;

    std::vector<VkImageView> mSwapChainImageViews;
    std::vector<VkFramebuffer> mSwapChainFramebuffers;

    VkRenderPass mRenderPass;
    VkPipelineLayout mPipelineLayout;
    VkPipeline mGraphicsPipeline;

    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;

    std::uint32_t mMipmapLevels; //Object specific
    VkImage mTextureImage;
    VkDeviceMemory mTextureImageMemory;
    VkImageView mTextureImageView;
    VkSampler mTextureSampler;

    VkImage mColorImage;
    VkDeviceMemory mColorImageMemory;
    VkImageView mColorImageView;

    VkImage mDepthImage;
    VkDeviceMemory mDepthImageMemory;
    VkImageView mDepthImageView;

    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFences;
    std::vector<VkFence> mImagesInFlight;
    size_t mCurrentFrame = 0;
    bool mFramebufferResized = false;

    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
    std::vector<VkBuffer> mUniformBuffers;
    std::vector<VkDeviceMemory> mUniformBuffersMemory;
    VkDescriptorPool mDescriptorPool;
    std::vector<VkDescriptorSet> mDescriptorSets;

    VkDescriptorSetLayout mDescriptorSetLayout;

    VkSampleCountFlagBits mMSAASamples;

    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;
};
}