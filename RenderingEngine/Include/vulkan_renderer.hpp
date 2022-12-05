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
#include "app_time.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer_base.hpp"

namespace rendering_engine
{
namespace vulkan_renderer
{

const int MAX_FRAMES_IN_FLIGHT = 2;

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

class VulkanDrawableComponent;

class VulkanRenderer : public RendererBase
{
public:
    VulkanRenderer(char const* title);
    VulkanRenderer(int const width, int const height, char const* title);
    ~VulkanRenderer() override;

    void Shutdown() override;
    void InitializeRenderer() override;

    void Update(float const delta) override;
    void Draw() override;

    float GetAspectRation() override;

    void CreateGraphicsPipeline(std::vector<char>& const spvVertShaderCode, std::vector<char>& const spvFragShaderCode);

    VkPipelineLayout GetPipelineLayout();
    VkPipeline GetGraphicsPipeline();

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    VkPhysicalDevice& GetPhysicalDevice();
    VkPhysicalDeviceFeatures& GetPhysDevSupportedFeatures();
    VkDevice& GetLogicalDevice();
    std::vector<VkCommandBuffer>& GetComandBuffers();
    VkExtent2D GetSwapChainExtent();

    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, std::uint32_t mipmapLevels);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    size_t GetCurrentFrame() const;

    VkDescriptorPool& GetDescriptorPool();
    VkDescriptorSetLayout& GetDescriptorSetLayout();

protected:
    void CreateInstance();
    bool CheckValidationLayerSupport();

    std::vector<const char*> GetRequiredExtensions();

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
    void SetupDebugMessenger();
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT const* pCreateInfo,
        VkAllocationCallbacks const* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    void CreateSurface();

    void PickPhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkSampleCountFlagBits CheckMaxUsableSampleCount();

    void CreateLogicalDevice();

    void CreateSwapChain();
    void RecreateSwapChain();
    void CleanupSwapChain();

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(std::vector<VkPresentModeKHR>const& availablePresentModes);
    VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR const& capabilities);

    void CreateImageViews();
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, std::uint32_t mipmapLevels);

    void CreateRenderPass();
    VkFormat FindDepthFormat();
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    void CreateDescriptorSetLayout();

    void CreateCommandPool();

    void CreateColorResources();

    void CreateVulkanImage(uint32_t width, uint32_t height, std::uint32_t mipmapLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    void CreateDepthResources();

    void CreateFramebuffers();

    VkShaderModule CreateShaderModule(std::vector<char>& code);

    VkCommandBuffer BeginSingleTimeCommand();
    void EndSingleTimeCommand(VkCommandBuffer commandBuffer);
    bool HasStencilComponent(VkFormat format);

    void CreateDescriptorPool(unsigned int const numberOfDrawableComponents);
    void CreateCommandBuffers();

    void CreateSyncObjects();

    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);   

protected:
    VkInstance mInstance;

    VkSurfaceKHR mSurface;

    std::vector<const char*> const mValidationLayers = { "VK_LAYER_KHRONOS_validation" };

    VkDebugUtilsMessengerEXT mDebugMessenger;
    std::vector<const char*> const mDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties mPhysicalDeviceProperties;
    VkPhysicalDeviceFeatures mPhysDevSupportedFeatures;

    VkSampleCountFlagBits mMSAASamples;

    VkDevice mLogicalDevice;
    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;

    VkSwapchainKHR mSwapChain;
    std::vector<VkImage> mSwapChainImages;
    VkFormat mSwapChainImageFormat;
    VkExtent2D mSwapChainExtent;

    std::vector<VkImageView> mSwapChainImageViews;
    std::vector<VkFramebuffer> mSwapChainFramebuffers;

    VkRenderPass mRenderPass;

    VkDescriptorSetLayout mDescriptorSetLayout;

    VkPipelineLayout mPipelineLayout; //Object specific
    VkPipeline mGraphicsPipeline; //Object specific

    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;

    VkImage mColorImage;
    VkDeviceMemory mColorImageMemory;
    VkImageView mColorImageView;

    VkImage mDepthImage;
    VkDeviceMemory mDepthImageMemory;
    VkImageView mDepthImageView;

    VkDescriptorPool mDescriptorPool;

    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFences;
    std::vector<VkFence> mImagesInFlight;

    size_t mCurrentFrame = 0;

};

} // vulkan_renderer
} //rendering_engine