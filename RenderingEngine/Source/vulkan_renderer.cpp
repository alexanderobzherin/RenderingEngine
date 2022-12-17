#include "vulkan_renderer.hpp"
#include <set>
#include <algorithm>
#include "vulkan_drawable_component.hpp"
#include "utility.hpp"
#include "scene_component.hpp"

namespace rendering_engine
{
namespace vulkan_renderer
{
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

VulkanRenderer::VulkanRenderer(char const* title)
    :
    RendererBase::RendererBase(title),
    mCurrentFrame(0)
{
}

VulkanRenderer::VulkanRenderer(int const width, int const height, char const* title)
    :
    RendererBase::RendererBase(width, height, title),
    mCurrentFrame(0)
{
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::Shutdown()
{
    vkDeviceWaitIdle(mLogicalDevice);

    CleanupSwapChain();

    for( auto& object : mDrawableObjects )
    {
        object->Shutdown();
    }

    vkDestroyDescriptorPool(mLogicalDevice, mDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mLogicalDevice, mDescriptorSetLayout, nullptr);

    for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        vkDestroySemaphore(mLogicalDevice, mRenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(mLogicalDevice, mImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(mLogicalDevice, mInFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(mLogicalDevice, mCommandPool, nullptr);

    vkDestroyDevice(mLogicalDevice, nullptr);
    if( enableValidationLayers )
    {
        DestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
    }
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    vkDestroyInstance(mInstance, nullptr);

    RendererBase::Shutdown();
}

void VulkanRenderer::InitializeRenderer()
{
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();

    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateDescriptorSetLayout();

    CreateCommandPool();

    CreateColorResources();
    CreateDepthResources();

    CreateFramebuffers();

    unsigned int const numberOfDrawableComponent = 3;
    CreateDescriptorPool(numberOfDrawableComponent);
    CreateCommandBuffers();
    CreateSyncObjects();

    CreateBuildInGraphicsPipelines();

    mDrawableObjects.push_back(std::make_shared<VulkanDrawableComponent>(this));
    mDrawableObjects.back()->SetColorTexture("../Intermediate/Models/Dice/D4/D4FontTextureSegoeScript.png");
    mDrawableObjects.back()->SetModelMesh("../Intermediate/Models/Dice/D4/D4.fbx");
    mDrawableObjects.back()->SetMaterial("FlatColorFiltering");
    mDrawableObjects.back()->Initialize();
    mDrawableObjects.back()->GetSceneComponent()->SetPosition(glm::vec3(4.0f, 0.0f, 0.0f));

    mDrawableObjects.push_back(std::make_shared<VulkanDrawableComponent>(this));
    mDrawableObjects.back()->Initialize();
    mDrawableObjects.back()->SetMaterial("BasicTexture3D");
    mDrawableObjects.back()->GetSceneComponent()->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    mDrawableObjects.push_back(std::make_shared<VulkanDrawableComponent>(this));
    mDrawableObjects.back()->SetColorTexture("../Intermediate/Models/Dice/D8/D8FontTextureSegoeScript.png");
    mDrawableObjects.back()->SetModelMesh("../Intermediate/Models/Dice/D8/D8.fbx");
    mDrawableObjects.back()->SetMaterial("FlatColorFiltering");
    mDrawableObjects.back()->Initialize();
    mDrawableObjects.back()->GetSceneComponent()->SetPosition(glm::vec3(-4.0f, 0.0f, 0.0f));
}

void VulkanRenderer::Update(float const delta)
{
    RendererBase::Update(delta);
}

void VulkanRenderer::Draw()
{
    vkWaitForFences(mLogicalDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(mLogicalDevice, mSwapChain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &imageIndex);

    if( result == VK_ERROR_OUT_OF_DATE_KHR )
    {
        RecreateSwapChain();
        return;
    }
    else
    {
        if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR )
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
    }

    vkResetFences(mLogicalDevice, 1, &mInFlightFences[mCurrentFrame]);

    vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    RecordCommandBuffer(mCommandBuffers[mCurrentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];

    VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if( vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { mSwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(mPresentQueue, &presentInfo);

    if( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized )
    {
        mFramebufferResized = false;
        RecreateSwapChain();
    }
    else
    {
        if( result != VK_SUCCESS )
        {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

    mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

float VulkanRenderer::GetAspectRation()
{
    return (float)(GetSwapChainExtent().width) / (float)(GetSwapChainExtent()).height;
}

void VulkanRenderer::CreateGraphicsPipeline(std::string pipelineName, std::vector<char>& const spvVertShaderCode, std::vector<char>& const spvFragShaderCode)
{
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    VkShaderModule vertShaderModule = CreateShaderModule(spvVertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(spvFragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = GetBindingDescription();
    auto attributeDescriptions = GetAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());

    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)mSwapChainExtent.width;
    viewport.height = (float)mSwapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = mSwapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = mMSAASamples;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;

    if( vkCreatePipelineLayout(mLogicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    //depthStencil.minDepthBounds = 0.0f; // Optional
    //depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    //depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = mRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if( vkCreateGraphicsPipelines(mLogicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    mGraphicsPipelines.emplace( pipelineName, std::pair<VkPipelineLayout, VkPipeline>( pipelineLayout, pipeline ) );
    //Destroy shader modules
    vkDestroyShaderModule(mLogicalDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(mLogicalDevice, vertShaderModule, nullptr);
}

std::pair<VkPipelineLayout, VkPipeline>& VulkanRenderer::GetGraphicsPipeline(std::string graphicsPipelineName)
{
    if( mGraphicsPipelines.find(graphicsPipelineName) != mGraphicsPipelines.end() )
    {
        return mGraphicsPipelines.find(graphicsPipelineName)->second;
    }
    else
    {
        throw std::runtime_error("There is no graphics pipeline " + graphicsPipelineName);
    }
}

void VulkanRenderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;

    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if( vkCreateBuffer(mLogicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(mLogicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if( vkAllocateMemory(mLogicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(mLogicalDevice, buffer, bufferMemory, 0);
}

VkPhysicalDevice& VulkanRenderer::GetPhysicalDevice()
{
    return mPhysicalDevice;
}

VkPhysicalDeviceFeatures& VulkanRenderer::GetPhysDevSupportedFeatures()
{
    return mPhysDevSupportedFeatures;
}

VkDevice& VulkanRenderer::GetLogicalDevice()
{
    return mLogicalDevice;
}

std::vector<VkCommandBuffer>& VulkanRenderer::GetComandBuffers()
{
    return mCommandBuffers;
}

VkExtent2D VulkanRenderer::GetSwapChainExtent()
{
    return mSwapChainExtent;
}

VkShaderModule VulkanRenderer::CreateShaderModule(std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if( vkCreateShaderModule(mLogicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

VkCommandBuffer VulkanRenderer::BeginSingleTimeCommand()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(mLogicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanRenderer::EndSingleTimeCommand(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(mGraphicsQueue);

    vkFreeCommandBuffers(mLogicalDevice, mCommandPool, 1, &commandBuffer);
}

bool VulkanRenderer::HasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanRenderer::CreateDescriptorPool(unsigned int const numberOfDrawableComponents)
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    
    for( unsigned int i = 0; i < numberOfDrawableComponents; ++i )
    {
        std::array<VkDescriptorPoolSize, 2> poolSizesComponent{};
        poolSizesComponent[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizesComponent[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizesComponent[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizesComponent[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        std::copy(poolSizesComponent.begin(), poolSizesComponent.end(), std::back_inserter(poolSizes));
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * numberOfDrawableComponents);

    if( vkCreateDescriptorPool(mLogicalDevice, &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void VulkanRenderer::CreateCommandBuffers()
{
    mCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.size();

    if( vkAllocateCommandBuffers(mLogicalDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void VulkanRenderer::CreateSyncObjects()
{
    mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    mImagesInFlight.resize(mSwapChainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        if( vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(mLogicalDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(mLogicalDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void VulkanRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //beginInfo.flags = 0; // Optional
    //beginInfo.pInheritanceInfo = nullptr; // Optional

    if( vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mRenderPass;
    renderPassInfo.framebuffer = mSwapChainFramebuffers[imageIndex];

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = mSwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    for( auto object : mDrawableObjects )
    {
        object->Draw();
    }

    vkCmdEndRenderPass(commandBuffer);

    if( vkEndCommandBuffer(commandBuffer) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void VulkanRenderer::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, imageFormat, &formatProperties);

    if( !(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) )
    {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = BeginSingleTimeCommand();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for( uint32_t i = 1; i < mipLevels; i++ )
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if( mipWidth > 1 ) mipWidth /= 2;
        if( mipHeight > 1 ) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    EndSingleTimeCommand(commandBuffer);
}

size_t VulkanRenderer::GetCurrentFrame() const
{
    return mCurrentFrame;
}

VkDescriptorPool& VulkanRenderer::GetDescriptorPool()
{
    return mDescriptorPool;
}

VkDescriptorSetLayout& VulkanRenderer::GetDescriptorSetLayout()
{
    return mDescriptorSetLayout;
}

void VulkanRenderer::CreateInstance()
{
    //Check validation layers.
    if( enableValidationLayers && !CheckValidationLayerSupport() )
    {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = mAppName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensionsNames = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionsNames.size());
    createInfo.ppEnabledExtensionNames = extensionsNames.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if( enableValidationLayers )
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
        createInfo.ppEnabledLayerNames = mValidationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if( vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create instance!");
    }
    std::cout << "Vulkan instance created" << std::endl;
}

bool VulkanRenderer::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for( const char* layerName : mValidationLayers )
    {
        bool layerFound = false;

        for( const auto& layerProperties : availableLayers )
        {
            if( strcmp(layerName, layerProperties.layerName) == 0 )
            {
                layerFound = true;
                break;
            }
        }

        if( !layerFound )
        {
            return false;
        }
    }

    return true;
}

std::vector<const char*> VulkanRenderer::GetRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if( enableValidationLayers )
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void VulkanRenderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void VulkanRenderer::SetupDebugMessenger()
{
    if( !enableValidationLayers )
    {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    PopulateDebugMessengerCreateInfo(createInfo);

    if( CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VkResult VulkanRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT const* pCreateInfo, VkAllocationCallbacks const* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if( func != nullptr )
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanRenderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if( func != nullptr )
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void VulkanRenderer::CreateSurface()
{
    if( glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

void VulkanRenderer::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

    if( deviceCount == 0 )
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

    for( const auto& device : devices )
    {
        if( IsDeviceSuitable(device) ) {
            mPhysicalDevice = device;
            vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mPhysDevSupportedFeatures);
            vkGetPhysicalDeviceProperties(mPhysicalDevice, &mPhysicalDeviceProperties);
            mMSAASamples = CheckMaxUsableSampleCount();
            break;
        }
    }

    if( mPhysicalDevice == VK_NULL_HANDLE )
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool VulkanRenderer::IsDeviceSuitable(VkPhysicalDevice device)
{
    // Next features could be used for rating and checkink proper device

    //VkPhysicalDeviceProperties deviceProperties;
    //vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // VkPhysicalDeviceFeatures supportedFeatures;
    // vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    //return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    //       deviceFeatures.geometryShader;

    QueueFamilyIndices indices = FindQueueFamilies(device);

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if( extensionsSupported )
    {
        auto const swapChainSupport = QuerySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    return indices.IsComplete() && extensionsSupported && swapChainAdequate;
}

QueueFamilyIndices VulkanRenderer::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    // Logic to find queue family indices to populate struct with
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for( const auto& queueFamily : queueFamilies )
    {
        if( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
        {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);
        if( presentSupport )
        {
            indices.presentFamily = i;
        }
        if( indices.IsComplete() )
        {
            break;
        }

        i++;
    }
    return indices;
}

bool VulkanRenderer::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(mDeviceExtensions.begin(), mDeviceExtensions.end());

    for( const auto& extension : availableExtensions )
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanRenderer::QuerySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails scsDetails;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &scsDetails.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

    if( formatCount != 0 )
    {
        scsDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, scsDetails.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

    if( presentModeCount != 0 )
    {
        scsDetails.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, scsDetails.presentModes.data());
    }

    return scsDetails;
}

VkSampleCountFlagBits VulkanRenderer::CheckMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(mPhysicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if( counts & VK_SAMPLE_COUNT_64_BIT ) { return VK_SAMPLE_COUNT_64_BIT; }
    if( counts & VK_SAMPLE_COUNT_32_BIT ) { return VK_SAMPLE_COUNT_32_BIT; }
    if( counts & VK_SAMPLE_COUNT_16_BIT ) { return VK_SAMPLE_COUNT_16_BIT; }
    if( counts & VK_SAMPLE_COUNT_8_BIT ) { return VK_SAMPLE_COUNT_8_BIT; }
    if( counts & VK_SAMPLE_COUNT_4_BIT ) { return VK_SAMPLE_COUNT_4_BIT; }
    if( counts & VK_SAMPLE_COUNT_2_BIT ) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

void VulkanRenderer::CreateLogicalDevice()
{
    QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for( uint32_t queueFamily : uniqueQueueFamilies )
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};
    if( mPhysDevSupportedFeatures.samplerAnisotropy )
    {
        deviceFeatures.samplerAnisotropy = VK_TRUE;
    }
    else
    {
        deviceFeatures.samplerAnisotropy = VK_FALSE;
    }

    if( mPhysDevSupportedFeatures.sampleRateShading )
    {
        deviceFeatures.sampleRateShading = VK_TRUE;
    }
    else
    {
        deviceFeatures.sampleRateShading = VK_FALSE;
    }

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();

    if( enableValidationLayers )
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
        createInfo.ppEnabledLayerNames = mValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if( vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(mLogicalDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
    vkGetDeviceQueue(mLogicalDevice, indices.presentFamily.value(), 0, &mPresentQueue);
}

void VulkanRenderer::CreateBuildInGraphicsPipelines()
{
    auto basicText3DvertShader = Utility::ReadShaderBinaryFile("../Intermediate/Shaders/BasicTexture3D/basic_texture_3d_vert.spv");
    auto basicText3DfragShader = Utility::ReadShaderBinaryFile("../Intermediate/Shaders/BasicTexture3D/basic_texture_3d_frag.spv");

    CreateGraphicsPipeline("BasicTexture3D", basicText3DvertShader, basicText3DfragShader);

    auto flatColorFilteringvertShader = Utility::ReadShaderBinaryFile("../Intermediate/Shaders/FlatColorFiltering/flat_color_filtering_vert.spv");
    auto flatColorFilteringfragShader = Utility::ReadShaderBinaryFile("../Intermediate/Shaders/FlatColorFiltering/flat_color_filtering_frag.spv");

    CreateGraphicsPipeline("FlatColorFiltering", flatColorFilteringvertShader, flatColorFilteringfragShader);
}

void VulkanRenderer::CleanGraphicsPipeline()
{
    for( auto& graphicsPipeline : mGraphicsPipelines )
    {
        vkDestroyPipeline(mLogicalDevice, graphicsPipeline.second.second, nullptr);
        vkDestroyPipelineLayout(mLogicalDevice, graphicsPipeline.second.first, nullptr);
    }
}

void VulkanRenderer::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(mPhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if( swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount )
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = mSurface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if( indices.graphicsFamily != indices.presentFamily ) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if( vkCreateSwapchainKHR(mLogicalDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(mLogicalDevice, mSwapChain, &imageCount, nullptr);
    mSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(mLogicalDevice, mSwapChain, &imageCount, mSwapChainImages.data());

    mSwapChainImageFormat = surfaceFormat.format;
    mSwapChainExtent = extent;
}

void VulkanRenderer::RecreateSwapChain()
{
    int width = 0;
    int height = 0;

    while( width == 0 || height == 0 )
    {
        glfwGetFramebufferSize(mWindow, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(mLogicalDevice);

    CleanupSwapChain();
    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();

    CreateBuildInGraphicsPipelines();

    CreateColorResources();
    CreateDepthResources();
    CreateFramebuffers();
    CreateCommandBuffers();
}

void VulkanRenderer::CleanupSwapChain()
{
    vkDestroyImageView(mLogicalDevice, mColorImageView, nullptr);
    vkDestroyImage(mLogicalDevice, mColorImage, nullptr);
    vkFreeMemory(mLogicalDevice, mColorImageMemory, nullptr);

    vkDestroyImageView(mLogicalDevice, mDepthImageView, nullptr);
    vkDestroyImage(mLogicalDevice, mDepthImage, nullptr);
    vkFreeMemory(mLogicalDevice, mDepthImageMemory, nullptr);
    for( auto framebuffer : mSwapChainFramebuffers )
    {
        vkDestroyFramebuffer(mLogicalDevice, framebuffer, nullptr);
    }

    vkFreeCommandBuffers(mLogicalDevice, mCommandPool, static_cast<uint32_t>(mCommandBuffers.size()), mCommandBuffers.data());

    CleanGraphicsPipeline();

    vkDestroyRenderPass(mLogicalDevice, mRenderPass, nullptr);
    for( auto imageView : mSwapChainImageViews )
    {
        vkDestroyImageView(mLogicalDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(mLogicalDevice, mSwapChain, nullptr);
}

VkSurfaceFormatKHR VulkanRenderer::ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& availableFormats)
{
    for( const auto& availableFormat : availableFormats )
    {
        if( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanRenderer::ChooseSwapPresentMode(std::vector<VkPresentModeKHR> const& availablePresentModes)
{
    for( const auto& availablePresentMode : availablePresentModes )
    {
        if( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR )
        {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderer::ChooseSwapExtent(VkSurfaceCapabilitiesKHR const& capabilities)
{
    if( capabilities.currentExtent.width != UINT32_MAX )
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(mWindow, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void VulkanRenderer::CreateImageViews()
{
    mSwapChainImageViews.resize(mSwapChainImages.size());

    for( uint32_t i = 0; i < mSwapChainImages.size(); i++ )
    {
        mSwapChainImageViews[i] = CreateImageView(mSwapChainImages[i], mSwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}

VkImageView VulkanRenderer::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, std::uint32_t mipmapLevels)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipmapLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if( vkCreateImageView(mLogicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void VulkanRenderer::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = mSwapChainImageFormat;
    colorAttachment.samples = mMSAASamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindDepthFormat();
    depthAttachment.samples = mMSAASamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = mSwapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if( vkCreateRenderPass(mLogicalDevice, &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

VkFormat VulkanRenderer::FindDepthFormat()
{
    return FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

VkFormat VulkanRenderer::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for( VkFormat format : candidates )
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &props);

        if( tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features )
        {
            return format;
        }
        else if( tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features )
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}

void VulkanRenderer::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;

    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if( vkCreateDescriptorSetLayout(mLogicalDevice, &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void VulkanRenderer::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(mPhysicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if( vkCreateCommandPool(mLogicalDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create command pool!");
    }
}

void VulkanRenderer::CreateColorResources()
{
    VkFormat colorFormat = mSwapChainImageFormat;

    CreateVulkanImage(mSwapChainExtent.width, mSwapChainExtent.height, 1, mMSAASamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mColorImage, mColorImageMemory);
    mColorImageView = CreateImageView(mColorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void VulkanRenderer::CreateVulkanImage(uint32_t width, uint32_t height, std::uint32_t mipmapLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipmapLevels;
    imageInfo.arrayLayers = 1;

    imageInfo.format = format;
    imageInfo.tiling = tiling;

    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    imageInfo.usage = usage;

    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    imageInfo.samples = numSamples;
    imageInfo.flags = 0; // Optional

    if( vkCreateImage(mLogicalDevice, &imageInfo, nullptr, &image) != VK_SUCCESS )
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(mLogicalDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if( vkAllocateMemory(mLogicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS ) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(mLogicalDevice, image, imageMemory, 0);
}

uint32_t VulkanRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

    for( uint32_t i = 0; i < memProperties.memoryTypeCount; i++ )
    {
        if( typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties )
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanRenderer::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, std::uint32_t mipmapLevels)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommand();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipmapLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if( newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL )
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if( HasStencilComponent(format) )
        {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    EndSingleTimeCommand(commandBuffer);
}

void VulkanRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    auto commandBuffer = BeginSingleTimeCommand();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    EndSingleTimeCommand(commandBuffer);
}

void VulkanRenderer::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommand();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    EndSingleTimeCommand(commandBuffer);
}

void VulkanRenderer::CreateDepthResources()
{
    VkFormat depthFormat = FindDepthFormat();

    CreateVulkanImage(mSwapChainExtent.width, mSwapChainExtent.height, 1, mMSAASamples, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mDepthImage, mDepthImageMemory);
    mDepthImageView = CreateImageView(mDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    //TransitionImageLayout(mDepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, mMipmapLevels);
}

void VulkanRenderer::CreateFramebuffers()
{
    mSwapChainFramebuffers.resize(mSwapChainImageViews.size());

    for( size_t i = 0; i < mSwapChainImageViews.size(); i++ )
    {
        std::array<VkImageView, 3> attachments = { mColorImageView, mDepthImageView, mSwapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mRenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = mSwapChainExtent.width;
        framebufferInfo.height = mSwapChainExtent.height;
        framebufferInfo.layers = 1;

        if( vkCreateFramebuffer(mLogicalDevice, &framebufferInfo, nullptr, &mSwapChainFramebuffers[i]) != VK_SUCCESS )
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

} //vulkan_renderer

} //rendering_engine