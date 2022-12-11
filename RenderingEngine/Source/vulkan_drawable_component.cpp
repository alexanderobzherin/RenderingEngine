#include "vulkan_drawable_component.hpp"
#include "utility.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_image_data.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "app_time.hpp"
#include "scene_component.hpp"
#include "camera.hpp"

namespace rendering_engine
{
namespace vulkan_renderer
{
VulkanDrawableComponent::VulkanDrawableComponent(VulkanRenderer* renderer)
    :
    mRenderer(renderer)
{}

VulkanDrawableComponent::~VulkanDrawableComponent()
{
}

void VulkanDrawableComponent::Initialize()
{
    DrawableComponent::Initialize();

    mVulkanColorTexture = std::make_shared<VulkanImageData>(mRenderer, mColorTextureImageData);
    mVulkanColorTexture->Initialize();

    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateUniformBuffers();

    auto descriptorPool = mRenderer->GetDescriptorPool();
    auto descriptorSetLayout = mRenderer->GetDescriptorSetLayout();
    CreateDescriptorSet(descriptorPool, descriptorSetLayout);
}

void VulkanDrawableComponent::Shutdown()
{
    mVulkanColorTexture->Shutdown();

    auto logicalDevice = mRenderer->GetLogicalDevice();
    for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        vkDestroyBuffer(logicalDevice, mUniformBuffers[i], nullptr);
        vkFreeMemory(logicalDevice, mUniformBuffersMemory[i], nullptr);
    }

    vkDestroyBuffer(logicalDevice, mIndexBuffer, nullptr);
    vkFreeMemory(logicalDevice, mIndexBufferMemory, nullptr);

    vkDestroyBuffer(logicalDevice, mVertexBuffer, nullptr);
    vkFreeMemory(logicalDevice, mVertexBufferMemory, nullptr);
}

void VulkanDrawableComponent::Update(float delta)
{
    DrawableComponent::Update(delta);

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    mUbo.model = mSceneComponent->GetWorldMatrix();
    mUbo.view = mRenderer->GetCamera()->ViewMatrix();
    mUbo.proj = mRenderer->GetCamera()->ProjectionMatrix();
    mUbo.proj[1][1] *= -1;
}

void VulkanDrawableComponent::Draw()
{
    void* data;
    vkMapMemory(mRenderer->GetLogicalDevice(), mUniformBuffersMemory[mRenderer->GetCurrentFrame()], 0, sizeof(mUbo), 0, &data);
    memcpy(data, &mUbo, sizeof(mUbo));
    vkUnmapMemory(mRenderer->GetLogicalDevice(), mUniformBuffersMemory[mRenderer->GetCurrentFrame()]);

    auto commandBuffersRef = mRenderer->GetComandBuffers();
    auto currentFrame = mRenderer->GetCurrentFrame();

    vkCmdBindPipeline(commandBuffersRef[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, mRenderer->GetGraphicsPipeline( mMaterialName ).second);

    VkBuffer vertexBuffers[] = { mVertexBuffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(commandBuffersRef[currentFrame], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffersRef[currentFrame], mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffersRef[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, mRenderer->GetGraphicsPipeline(mMaterialName).first, 0, 1, &mDescriptorSets[currentFrame], 0, nullptr);
    vkCmdDrawIndexed(commandBuffersRef[currentFrame], static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
}

void VulkanDrawableComponent::CreateVertexBuffer()
{
    auto logicalDevice = mRenderer->GetLogicalDevice();

    VkDeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);

    memcpy(data, mVertices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);
    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mVertexBuffer, mVertexBufferMemory);

    mRenderer->CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void VulkanDrawableComponent::CreateIndexBuffer()
{
    auto logicalDevice = mRenderer->GetLogicalDevice();

    VkDeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, mIndices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);

    mRenderer->CopyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void VulkanDrawableComponent::CreateUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    mUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    mUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

    for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ ) {
        mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            , mUniformBuffers[i], mUniformBuffersMemory[i]);
    }
}

void VulkanDrawableComponent::UpdateUniformBuffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), mRenderer->GetAspectRation(), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    void* data;
    vkMapMemory(mRenderer->GetLogicalDevice(), mUniformBuffersMemory[mRenderer->GetCurrentFrame()], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(mRenderer->GetLogicalDevice(), mUniformBuffersMemory[mRenderer->GetCurrentFrame()]);
}

void VulkanDrawableComponent::CreateDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout)
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    mDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    VkResult result = vkAllocateDescriptorSets(mRenderer->GetLogicalDevice(), &allocInfo, mDescriptorSets.data());
    if( result != VK_SUCCESS )
    {
        throw std::runtime_error("failed to allocate descriptor sets! Error: " + std::to_string(result));
    }

    for( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = mUniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = mVulkanColorTexture->GetVulkanTextureImageView();
        imageInfo.sampler = mVulkanColorTexture->GetTextureSampler();

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = mDescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = mDescriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(mRenderer->GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

} // vulkan_renderer
} //rendering_engine