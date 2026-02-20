// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "i_renderer.hpp"
#include "i_window_system.hpp"
#include "vertex_declarations.hpp"
#include <optional>
#include <vector>
#include <unordered_map>
#include <deque>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace rendering_engine
{
/** @brief Number of frames that can be processed simultaneously (double buffering). */
const int MAX_FRAMES_IN_FLIGHT = 2;
/**
 * @struct QueueFamilyIndices
 * @brief Holds indices of Vulkan queue families supporting required operations.
 */
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    /** @brief Checks if both required queue families are available. */
    bool IsComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};
/**
 * @struct SwapChainSupportDetails
 * @brief Describes capabilities and available configurations for a physical device's swap chain.
 */
struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

enum class DeferredType
{
    Buffer,
    Memory,
    DescriptorPool,
    Image,
    ImageView,
    Sampler,
    Pipeline,
    PipelineLayout,
};

struct DeferredItem
{
DeferredType type;
union {
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDescriptorPool descriptorPool;
    VkImage image;
    VkImageView imageView;
    VkSampler sampler;
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;
};
std::uint64_t retireFrame = 0;
DeferredItem() { buffer = VK_NULL_HANDLE; }
};

class Material;
class TextureCache;

/**
 * @class VulkanRenderer
 * @brief Vulkan-based implementation of the IRenderer interface.
 *
 * This class encapsulates the initialization and management of Vulkan rendering resources,
 * including device selection, swap chain management, render pass setup, frame synchronization,
 * and command buffer submission. It serves as the core rendering backend for all graphics
 * operations performed by the engine.
 */
class VulkanRenderer : public IRenderer
{
public:
    /**
     * @brief Constructs a VulkanRenderer bound to a specific window system.
     * @param windowSystem Reference to the window system used for surface creation and event handling.
     */
    VulkanRenderer(IWindowSystem& windowSystem);
    /** @copydoc IRenderer::InitializeRenderer */
    void InitializeRenderer() override;
    /** @copydoc IRenderer::DrawFrame */
    void DrawFrame() override;
    /** @copydoc IRenderer::BeginFrame */
    bool BeginFrame() override;
    /** @copydoc IRenderer::BeginRenderPass */
    void BeginRenderPass() override;
    /** @copydoc IRenderer::EndRenderPass */
    void EndRenderPass() override;
    /** @copydoc IRenderer::EndFrame */
    void EndFrame() override;
    /** @copydoc IRenderer::WaitIdle */
    void WaitIdle() override;
    /** @copydoc IRenderer::ShutdownRenderer */
    void ShutdownRenderer() override;
    /** @copydoc IRenderer::RegisterObserver */
    void RegisterObserver(IRendererObserver* notifier) override;
    /** @copydoc IRenderer::UnregisterObserver */
    void UnregisterObserver(IRendererObserver* notifier) override;

    /** @copydoc IRenderer::ProvideRenderResources */
    IRenderResources* ProvideRenderResources() const override;
    /** @copydoc IRenderer::ProvideTextureRenderResources */
    ITextureRenderResources* ProvideTextureRenderResources() const override;
    /** @copydoc IRenderer::ProvideMaterialRenderResources */
    IMaterialRenderResources* ProvideMaterialRenderResources() const override;
    /** @copydoc IRenderer::ProvideMeshRenderResources */
    IMeshRenderResources* ProvideMeshRenderResources() const override;
    /** @copydoc IRenderer::SetDefaultColor */
    void SetDefaultColor(float r, float g, float b) override;

    /**
     * @brief Creates a new Vulkan buffer with the specified usage and memory properties.
     * @param size        Size of the buffer in bytes.
     * @param usage       Bitmask specifying intended buffer usage.
     * @param properties  Memory property flags defining allocation type.
     * @param buffer      Output handle to the created buffer.
     * @param bufferMemory Output handle to the allocated buffer memory.
     */
    void CreateBuffer(VkDeviceSize size, 
                      VkBufferUsageFlags usage, 
                      VkMemoryPropertyFlags properties, 
                      VkBuffer& buffer, 
                      VkDeviceMemory& bufferMemory);
    /**
     * @brief Copies data from one buffer to another using a temporary command buffer.
     * @param srcBuffer Source buffer.
     * @param dstBuffer Destination buffer.
     * @param size      Size of data to copy in bytes.
     */
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    /** @brief Returns reference to the logical Vulkan device. */
    VkDevice& GetLogicalDevice();
    /**
     * @brief Transitions the image layout for a given Vulkan image.
     * @param image         Image handle.
     * @param format        Image format.
     * @param oldLayout     Current image layout.
     * @param newLayout     Desired image layout.
     * @param mipmapLevels  Number of mipmap levels to transition.
     */
    void TransitionImageLayout(VkImage image, 
                               VkFormat format, 
                               VkImageLayout oldLayout, 
                               VkImageLayout newLayout, 
                               std::uint32_t mipmapLevels);
    /**
     * @brief Copies buffer data into an image (used for uploading texture data).
     * @param buffer Source buffer containing pixel data.
     * @param image  Destination image.
     * @param width  Image width in pixels.
     * @param height Image height in pixels.
     */
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    /**
     * @brief Generates mipmaps for a given image resource.
     * @param image       Vulkan image handle.
     * @param imageFormat Image format (must support linear blitting).
     * @param texWidth    Texture width in pixels.
     * @param texHeight   Texture height in pixels.
     * @param mipLevels   Number of mipmap levels to generate.
     */
    void GenerateMipmaps(VkImage image, 
                         VkFormat imageFormat, 
                         int32_t texWidth, 
                         int32_t texHeight, 
                         uint32_t mipLevels);
    /**
     * @brief Finds a suitable memory type for a given allocation.
     * @param typeFilter  Bitmask of compatible memory types.
     * @param properties  Desired memory property flags.
     * @return Index of the selected memory type.
     */
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    /** @brief Returns reference to the physical device. */
    VkPhysicalDevice& GetPhysicalDevice();
    /** @brief Returns reference to the physical device�s supported features. */
    VkPhysicalDeviceFeatures& GetPhysDevSupportedFeatures();

    /**
     * @brief Creates a descriptor set layout corresponding to a given material.
     *
     * The layout defines how shader resources (uniform buffers, custom parameters, textures)
     * are bound to the pipeline. By the local convention of this engine:
     * - **Binding 0** � Transformation matrices (model, view, projection) depending on material domain (2D or 3D).
     * - **Binding 1** � Custom parameter variable block (if any). All variables are serialized and packed into a single binding.
     * - **Bindings 2..n** � Texture samplers, one texture per binding.
     *
     * This convention ensures consistent descriptor bindings across all shaders and materials.
     *
     * @param material Pointer to the material defining binding structure.
     * @return Handle to the created descriptor set layout.
     */
    VkDescriptorSetLayout CreateDescriptorSetLayout(Material* material);
    /** @brief Returns the index of the currently active frame in flight. */
    inline size_t GetCurrentFrame() const
    {
        return mCurrentFrame;
    }
    /** @brief Returns the collection of command buffers used for rendering. */
    std::vector<VkCommandBuffer> GetComandBuffers();
    /**
     * @brief Creates a Vulkan graphics pipeline based on material and shader inputs.
     * @param material            Pointer to the material configuration.
     * @param descriptorSetLayout Descriptor set layout used in the pipeline.
     * @param spvVertShaderCode   Compiled SPIR-V vertex shader bytecode.
     * @param spvFragShaderCode   Compiled SPIR-V fragment shader bytecode.
     * @return A pair containing pipeline layout and pipeline handle.
     */
    std::pair<VkPipelineLayout, VkPipeline> CreateGraphicsPipeline(Material* material,
                                                                   VkDescriptorSetLayout& descriptorSetLayout,
                                                                   std::vector<char>& spvVertShaderCode,
                                                                   std::vector<char>& spvFragShaderCode);

    /**
     * @brief Returns a vertex input binding description for a specific vertex type.
     * @tparam T Vertex type (e.g., Vertex2D, VertexPositionColorTexture, etc.).
     */
    template <typename T>
    static VkVertexInputBindingDescription GetBindingDescription();
    /**
     * @brief Returns vertex attribute descriptions for a specific vertex type.
     * @tparam T Vertex type (e.g., Vertex2D, VertexPositionColorTexture, etc.).
     */
    template <typename T>
    static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

    void AddDeferredDestroy(DeferredItem deferredItem);

private:
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

    void CreateFrameSyncObjects();
    void CreateSwapchainSyncObjects();

    VkShaderModule CreateShaderModule(std::vector<char>& code);

    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    VkCommandBuffer BeginSingleTimeCommand();
    bool HasStencilComponent(VkFormat format);
    void EndSingleTimeCommand(VkCommandBuffer commandBuffer);

    void ProcessDeferredDestruction();

private:
    IWindowSystem& mWindowSystem;
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

    VkCommandPool mCommandPool;
    std::vector<VkCommandBuffer> mCommandBuffers;
    uint32_t mImageIndex = 0;

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
    std::vector<IRendererObserver*> mObservers;

    std::uint64_t mFrameSerial = 0;
    std::deque<DeferredItem> mDeferredQueue;

    glm::vec3 mDefaultColor = {0.0f, 0.0f, 0.0f};
};

} // rendering_engine