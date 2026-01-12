#include "vulkan_render_resources.hpp"
#include "vulkan_renderer.hpp"
#include "vulkan_material_resources.hpp"
#include "vulkan_texture_resources.hpp"
#include "vulkan_mesh_resources.hpp"
#include "image_data_gpu.hpp"
#include "material.hpp"
#include "mesh_data_gpu.hpp"
#include "texture_cache.hpp"

#include <stdexcept>
#include <iostream>

namespace rendering_engine
{

VulkanRenderResources::VulkanRenderResources(VulkanRenderer* renderer)
    :
    mRenderer(renderer),
    bHasCustomMaterialVariables(false)
{
    mRenderer->RegisterObserver(this);
}

VulkanRenderResources::~VulkanRenderResources()
{
    mRenderer->UnregisterObserver(this);
}

void VulkanRenderResources::Initialize(Material* material, MeshDataGpu* meshData, TextureCache* textureCache)
{
    mMaterial = material;
    mMeshData = meshData;
    mTextureCache = textureCache;

    AcquireResources();
}

void VulkanRenderResources::SubmitResources(Transformations2D& transformations, const PackedMaterialData& materialParameters)
{
    UpdateTransformations(transformations);
    UpdateMaterialParameters(materialParameters);
    DrawIndexed();
}

void VulkanRenderResources::SubmitResources(Transformations3D& transformations, const PackedMaterialData& materialParameters)
{
    UpdateTransformations(transformations);
    UpdateMaterialParameters(materialParameters);
    DrawIndexed();
}

void VulkanRenderResources::Shutdown()
{
    DeferredItem descriptorPool;
    descriptorPool.type = DeferredType::DescriptorPool;
    descriptorPool.descriptorPool = mDescriptorPool;
    mRenderer->AddDeferredDestroy(descriptorPool);
    mDescriptorPool = VK_NULL_HANDLE;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (!mTransformationBuffers.empty())
        {   
            DeferredItem tBuffer;
            tBuffer.type = DeferredType::Buffer;
            tBuffer.buffer = mTransformationBuffers[i];
            mRenderer->AddDeferredDestroy(tBuffer);

            DeferredItem tMem;
            tMem.type = DeferredType::Memory;
            tMem.memory = mTransformationBuffersMemory[i];
            mRenderer->AddDeferredDestroy(tMem);

            mTransformationBuffers[i] = VK_NULL_HANDLE;
            mTransformationBuffersMemory[i] = VK_NULL_HANDLE;
        }

        if (!mMaterialParametersBuffers.empty())
        {
            DeferredItem matBuffer;
            matBuffer.type = DeferredType::Buffer;
            matBuffer.buffer = mMaterialParametersBuffers[i];
            mRenderer->AddDeferredDestroy(matBuffer);

            DeferredItem matMem;
            matMem.type = DeferredType::Memory;
            matMem.memory = mMaterialParametersMemory[i];
            mRenderer->AddDeferredDestroy(matMem);

            mMaterialParametersBuffers[i] = VK_NULL_HANDLE;
            mMaterialParametersMemory[i] = VK_NULL_HANDLE;
        }
    }

    mTransformationBuffers.clear();
    mTransformationBuffersMemory.clear();
    mMaterialParametersBuffers.clear();
    mMaterialParametersMemory.clear();
}

void VulkanRenderResources::OnRenderResourcesRelease()
{
    Shutdown();
}

void VulkanRenderResources::OnRenderResourcesRebuild()
{
    AcquireResources();
}

void VulkanRenderResources::AcquireResources()
{
    if (!mMaterial)
    {
        // Log error message, as without initialized material rendering is not possible.
        return;
    }
    VulkanMaterialResources* materialResources = static_cast<VulkanMaterialResources*>(mMaterial->GetMaterialRenderResources());
    mGraphicsPipeline = materialResources->GetPipeline();
    mPipelineLayout = materialResources->GetPipelineLayout();

    VulkanMeshResources* meshResources = static_cast<VulkanMeshResources*>(mMeshData->GetMeshRenderResources());
    mVertexBuffer = meshResources->GetVertexBuffer();
    mIndexBuffer = meshResources->GetIndexBuffer();

    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSet();
}

void VulkanRenderResources::CreateUniformBuffers()
{
    VkDeviceSize transformationBufferSize;
    if (mMaterial->GetMaterialSettings().materialDomain == MaterialDomain::Sprite2D)
    {
        transformationBufferSize = sizeof(Transformations2D);
    }
    if (mMaterial->GetMaterialSettings().materialDomain == MaterialDomain::Surface3D)
    {
        transformationBufferSize = sizeof(Transformations3D);
    }

    mTransformationBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    mTransformationBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        mRenderer->CreateBuffer(transformationBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            mTransformationBuffers[i], mTransformationBuffersMemory[i]);
    }

    auto materialParameters = mMaterial->PackMaterialParameters();
    if (materialParameters.buffer.empty())
    {
        return;
    }

    VkDeviceSize matVarBufferSize = materialParameters.buffer.size();
    mMaterialParametersBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    mMaterialParametersMemory.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        mRenderer->CreateBuffer(matVarBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            mMaterialParametersBuffers[i], mMaterialParametersMemory[i]);
    }
    std::cout << "Mat UBO buf[0]=" << (void*)mMaterialParametersBuffers[0] << "\n";
}

void VulkanRenderResources::CreateDescriptorPool()
{
    const uint32_t maxFramesInFlight = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    std::vector<VkDescriptorPoolSize> poolSizes;

    VkDescriptorPoolSize tranformationPool;
    tranformationPool.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    tranformationPool.descriptorCount = maxFramesInFlight;

    poolSizes.push_back(tranformationPool);

    auto materialParameters = mMaterial->PackMaterialParameters();
    if (!materialParameters.buffer.empty())
    {
        VkDescriptorPoolSize materialParameterPool;
        materialParameterPool.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        materialParameterPool.descriptorCount = maxFramesInFlight;

        poolSizes.push_back(materialParameterPool);
    }

    for (const auto& texture : mMaterial->GetTextures())
    {
        (void)texture;
        VkDescriptorPoolSize textureSamplerPool;
        textureSamplerPool.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureSamplerPool.descriptorCount = maxFramesInFlight;

        poolSizes.push_back(textureSamplerPool);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxFramesInFlight;

    if (vkCreateDescriptorPool(mRenderer->GetLogicalDevice(), &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void VulkanRenderResources::CreateDescriptorSet()
{
    // By local convention of this engine, bindings will be numbered:
    // Bindings i = 0, i < n, there n - total number of bindings
    // Binding i - transformations matrices of types according to material domain (2D or 3D)
    // Binding i++ - cunstom parameter variable of material, if any. All variables will be 
    //               serialized and packed in a single binding
    // Binding i++ - textures sampler, if any. One texture per binding.

    const uint32_t maxFramesInFlight = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VulkanMaterialResources* materialResources = static_cast<VulkanMaterialResources*>(mMaterial->GetMaterialRenderResources());
    auto descriptorSetLayout = materialResources->GetDescriptorSetLayout();
    std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool;
    allocInfo.descriptorSetCount = maxFramesInFlight;
    allocInfo.pSetLayouts = layouts.data();

    mDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    VkResult result = vkAllocateDescriptorSets(mRenderer->GetLogicalDevice(), &allocInfo, mDescriptorSets.data());
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets! Error: " + std::to_string(result));
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        unsigned int dstBinding = 0;

        VkDescriptorBufferInfo matVarBufferInfo{};
        VkWriteDescriptorSet matVarDescSet;

        std::vector<VkWriteDescriptorSet> descriptorWrites;

        // Transfotmations descriptor set
        VkDescriptorBufferInfo transformationBufferInfo{};
        transformationBufferInfo.buffer = mTransformationBuffers[i];
        transformationBufferInfo.offset = 0;
        if (mMaterial->GetMaterialSettings().materialDomain == MaterialDomain::Sprite2D)
        {
            transformationBufferInfo.range = sizeof(Transformations2D);
        }
        if (mMaterial->GetMaterialSettings().materialDomain == MaterialDomain::Surface3D)
        {
            transformationBufferInfo.range = sizeof(Transformations3D);
        }
        VkWriteDescriptorSet transformationsDescSet;
        transformationsDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        transformationsDescSet.pNext = nullptr;
        transformationsDescSet.dstSet = mDescriptorSets[i];
        transformationsDescSet.dstBinding = dstBinding;
        transformationsDescSet.dstArrayElement = 0;
        transformationsDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        transformationsDescSet.descriptorCount = 1;
        transformationsDescSet.pBufferInfo = &transformationBufferInfo;
        descriptorWrites.push_back(transformationsDescSet);

        // Custom material parameters descriptor set
        auto materialParameters = mMaterial->PackMaterialParameters();
        if (!materialParameters.buffer.empty())
        {
            ++dstBinding;

            matVarBufferInfo.buffer = mMaterialParametersBuffers[i];
            matVarBufferInfo.offset = 0;
            matVarBufferInfo.range = materialParameters.buffer.size();

            matVarDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            matVarDescSet.pNext = nullptr;
            matVarDescSet.dstSet = mDescriptorSets[i];
            matVarDescSet.dstBinding = dstBinding;
            matVarDescSet.dstArrayElement = 0;
            matVarDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            matVarDescSet.descriptorCount = 1;
            matVarDescSet.pBufferInfo = &matVarBufferInfo;
            descriptorWrites.push_back(matVarDescSet);
        }

        // Custom texture image data descriptor set
        for (const auto& texture : mMaterial->GetTextures())
        {
            ++dstBinding;
            auto imageData = mTextureCache->GetTextureResources(texture);
            VulkanTextureResources* textureResources = static_cast<VulkanTextureResources*>(imageData->GetTextureRenderResources());
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = textureResources->GetVkImageView();
            imageInfo.sampler = textureResources->GetVkSampler();

            VkWriteDescriptorSet imageDescSet;
            imageDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            imageDescSet.pNext = nullptr;
            imageDescSet.dstSet = mDescriptorSets[i];
            imageDescSet.dstBinding = dstBinding;
            imageDescSet.dstArrayElement = 0;
            imageDescSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            imageDescSet.descriptorCount = 1;
            imageDescSet.pImageInfo = &imageInfo;

            descriptorWrites.push_back(imageDescSet);
        }

        vkUpdateDescriptorSets(mRenderer->GetLogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void VulkanRenderResources::UpdateTransformations(Transformations2D& transformations)
{
    auto device = mRenderer->GetLogicalDevice();
    auto frame = mRenderer->GetCurrentFrame();

    // Update transformations
    void* dataTransform;
    vkMapMemory(device, mTransformationBuffersMemory[frame], 0, sizeof(Transformations2D), 0, &dataTransform);
    memcpy(dataTransform, &transformations, sizeof(transformations));
    vkUnmapMemory(device, mTransformationBuffersMemory[frame]);
}

void VulkanRenderResources::UpdateTransformations(Transformations3D& transformations)
{
    auto device = mRenderer->GetLogicalDevice();
    auto frame = mRenderer->GetCurrentFrame();

    // Flip Y-axis for Vulkan clip space
    transformations.proj[1][1] *= -1;
    // Update transformations
    void* dataTransform;
    vkMapMemory(device, mTransformationBuffersMemory[frame], 0, sizeof(Transformations3D), 0, &dataTransform);
    memcpy(dataTransform, &transformations, sizeof(transformations));
    vkUnmapMemory(device, mTransformationBuffersMemory[frame]);
}

void VulkanRenderResources::UpdateMaterialParameters(const PackedMaterialData& materialParameters)
{
    auto device = mRenderer->GetLogicalDevice();
    auto frame = mRenderer->GetCurrentFrame();

    // Update custom variables
    if (!materialParameters.buffer.empty())
    {
        void* dataCustom;
        vkMapMemory(device, mMaterialParametersMemory[frame], 0, materialParameters.buffer.size(), 0, &dataCustom);
        memcpy(dataCustom, materialParameters.buffer.data(), materialParameters.buffer.size());
        vkUnmapMemory(device, mMaterialParametersMemory[frame]);
    }
}

void VulkanRenderResources::DrawIndexed()
{
    auto commandBuffers = mRenderer->GetComandBuffers();
    auto frame = mRenderer->GetCurrentFrame();

    VkBuffer vertexBuffer[] = { mVertexBuffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindPipeline(commandBuffers[frame], VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);
    
    /*If dynamicState in pipeline is used then vkCmdSetViewport and vkCmdSetScissor per draw call*/
    //VkViewport viewport{};
    //viewport.x = 0.0f;
    //viewport.y = 0.0f;
    //viewport.width = static_cast<float>(mRenderer->GetSwapChainExtent().width);
    //viewport.height = static_cast<float>(mRenderer->GetSwapChainExtent().height);
    //viewport.minDepth = 0.0f;
    //viewport.maxDepth = 1.0f;
    //vkCmdSetViewport(commandBuffers[frame], 0, 1, &viewport);

    //VkRect2D scissor{};
    //scissor.offset = { 0, 0 };
    //scissor.extent = mRenderer->GetSwapChainExtent();
    //vkCmdSetScissor(commandBuffers[frame], 0, 1, &scissor);
    
    vkCmdBindVertexBuffers(commandBuffers[frame], 0, 1, vertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffers[frame], mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffers[frame], VK_PIPELINE_BIND_POINT_GRAPHICS,
        mPipelineLayout, 0, 1, &mDescriptorSets[frame], 0, nullptr);
    vkCmdDrawIndexed(commandBuffers[frame], static_cast<uint32_t>(mMeshData->GetIndices().size()), 1, 0, 0, 0);
}



} // namespace rendering_engine
