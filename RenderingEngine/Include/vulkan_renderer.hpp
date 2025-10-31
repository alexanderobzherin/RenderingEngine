#pragma once

#include "i_renderer.hpp"
#include "i_window_system.hpp"
#include "vertex_declarations.hpp"
#include <optional>
#include <vector>
#include <unordered_map>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace rendering_engine
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

class Material;
class TextureCache;

class VulkanRenderer : public IRenderer
{
public:
    VulkanRenderer(IWindowSystem& windowSystem);

    // Next public functions just for test purposes
    VkRenderPass GetRenderPass() {return mRenderPass;}
    VkExtent2D GetSwapChainExtent() {return mSwapChainExtent;}
    VkSampleCountFlagBits GetMSAASamples() { return mMSAASamples; }
    // EndOf Next public functions just for test purposes

    void InitializeRenderer() override;
    void DrawFrame() override;

    bool BeginFrame() override;
    void BeginRenderPass() override;
    void EndRenderPass() override;
    void EndFrame() override;

    void WaitIdle() override;
    void ShutdownRenderer() override;

    void RegisterObserver(IRendererObserver* notifier) override;
    void UnregisterObserver(IRendererObserver* notifier) override;

    IRenderResources* ProvideRenderResources() const override;
    ITextureRenderResources* ProvideTextureRenderResources() const override;
    IMaterialRenderResources* ProvideMaterialRenderResources() const override;
    IMeshRenderResources* ProvideMeshRenderResources() const override;

    void CreateBuffer(VkDeviceSize size, 
                      VkBufferUsageFlags usage, 
                      VkMemoryPropertyFlags properties, 
                      VkBuffer& buffer, 
                      VkDeviceMemory& bufferMemory);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    VkDevice& GetLogicalDevice();
    void TransitionImageLayout(VkImage image, 
                               VkFormat format, 
                               VkImageLayout oldLayout, 
                               VkImageLayout newLayout, 
                               std::uint32_t mipmapLevels);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void GenerateMipmaps(VkImage image, 
                         VkFormat imageFormat, 
                         int32_t texWidth, 
                         int32_t texHeight, 
                         uint32_t mipLevels);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkPhysicalDevice& GetPhysicalDevice();
    VkPhysicalDeviceFeatures& GetPhysDevSupportedFeatures();

    /* To be removed 
    void CreateDescriptorSetLayout(); 
    */

    // By local convention of this engine, bindings will be numbered:
    // Bindings i = 0, i < n, there n - total number of bindings
    // Binding i - transformations matrices of types according to material domain (2D or 3D)
    // Binding i++ - cunstom parameter variable of material, if any. All variables will be serialized and packed in a single binding
    // Binding i++ - textures sampler, if any. One texture per binding.
    VkDescriptorSetLayout CreateDescriptorSetLayout(Material* material);

    inline size_t GetCurrentFrame() const
    {
        return mCurrentFrame;
    }

    std::vector<VkCommandBuffer> GetComandBuffers();

    /* To be removed
    void CreateBuildInGraphicsPipelines();
    void CreateGraphicsPipeline(std::string pipelineName,
        std::vector<char>& spvVertShaderCode,
        std::vector<char>& spvFragShaderCode);
    void CreateGraphicsPipeline(Material* material,
        std::vector<char>& spvVertShaderCode,
        std::vector<char>& spvFragShaderCode);
    */
    std::pair<VkPipelineLayout, VkPipeline> CreateGraphicsPipeline(Material* material,
                                                                   VkDescriptorSetLayout& descriptorSetLayout,
                                                                   std::vector<char>& spvVertShaderCode,
                                                                   std::vector<char>& spvFragShaderCode);

    template <typename T>
    static VkVertexInputBindingDescription GetBindingDescription();

    template <typename T>
    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    // Explicit declarations
    template <>
    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<Vertex2D>();

    template <>
    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexPositionColorTexture>();

    template <>
    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexPositionColorTextureNormalTangent>();


private:
    // Vulkan Renderer initialization
    void CreateInstance();
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
    void SetupDebugMessenger();
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                                          VkDebugUtilsMessengerCreateInfoEXT const* pCreateInfo, 
                                          VkAllocationCallbacks const* pAllocator, 
                                          VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
                                       VkDebugUtilsMessengerEXT debugMessenger, 
                                       const VkAllocationCallbacks* pAllocator);
    void CreateSurface();

    void PickPhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    VkSampleCountFlagBits CheckMaxUsableSampleCount();

    void CreateLogicalDevice();

    void CreateSwapChain();
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(std::vector<VkPresentModeKHR>const& availablePresentModes);
    VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR const& capabilities);
    void CleanupSwapChain();
    void RecreateSwapChain();

    void CreateImageViews();
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, std::uint32_t mipmapLevels);
    
    void CreateRenderPass();
    VkFormat FindDepthFormat();
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    

    void CreateCommandPool();

    void CreateColorResources();

    void CreateVulkanImage(uint32_t width, uint32_t height, std::uint32_t mipmapLevels,
        VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

    void CreateDepthResources();

    void CreateFramebuffers();

    void CreateCommandBuffers();

    void CreateSyncObjects();

    VkShaderModule CreateShaderModule(std::vector<char>& code);

    /*To be removed to per material
    void CleanGraphicsPipeline();
    */

    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    VkCommandBuffer BeginSingleTimeCommand();
    bool HasStencilComponent(VkFormat format);
    void EndSingleTimeCommand(VkCommandBuffer commandBuffer);

private:
    IWindowSystem& mWindowSystem;
    //IApplication& mApp;
    size_t mCurrentFrame;

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

    /* Move out to be per material
    VkDescriptorSetLayout mDescriptorSetLayout;
    */

    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;
    uint32_t mImageIndex = 0;

    VkImage mColorImage;
    VkDeviceMemory mColorImageMemory;
    VkImageView mColorImageView;

    VkImage mDepthImage;
    VkDeviceMemory mDepthImageMemory;
    VkImageView mDepthImageView;

/* Move out to be per drawable component
    VkDescriptorPool mDescriptorPool;
*/

    std::vector<VkSemaphore> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderFinishedSemaphores;
    std::vector<VkFence> mInFlightFences;
    std::vector<VkFence> mImagesInFlight;

/* Move out to be per material
    std::unordered_map<std::string, std::pair<VkPipelineLayout, VkPipeline>> mGraphicsPipelines;
*/
    std::vector<IRendererObserver*> mObservers;

};


template<typename T>
VkVertexInputBindingDescription VulkanRenderer::GetBindingDescription()
{
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(T);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return binding;
}

template<>
std::vector<VkVertexInputAttributeDescription> VulkanRenderer::GetAttributeDescriptions<Vertex2D>()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    attributeDescriptions.resize(3);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex2D, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex2D, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex2D, textureCoordinates);

    return attributeDescriptions;
}

template<>
std::vector<VkVertexInputAttributeDescription> VulkanRenderer::GetAttributeDescriptions<VertexPositionColorTexture>()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    attributeDescriptions.resize(3);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(VertexPositionColorTexture, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(VertexPositionColorTexture, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(VertexPositionColorTexture, textureCoordinates);

    return attributeDescriptions;
}

template<>
std::vector<VkVertexInputAttributeDescription> VulkanRenderer::GetAttributeDescriptions<VertexPositionColorTextureNormalTangent>()
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
    attributeDescriptions.resize(5);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(VertexPositionColorTextureNormalTangent, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(VertexPositionColorTextureNormalTangent, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(VertexPositionColorTextureNormalTangent, textureCoordinates);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(VertexPositionColorTextureNormalTangent, normal);

    attributeDescriptions[4].binding = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[4].offset = offsetof(VertexPositionColorTextureNormalTangent, tangent);

    return attributeDescriptions;
}


} //rendering_engine