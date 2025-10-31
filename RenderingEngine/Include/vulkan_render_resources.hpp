// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "i_render_resources.hpp"
#include "i_renderer_observer.hpp"

#include <vector>
#include <vulkan/vulkan.h>

namespace rendering_engine
{
class VulkanRenderer;
class Material;
class MeshDataGpu;
class TextureCache;

/**
 * @brief Vulkan implementation of IRenderResources for the Rendering Engine.
 *
 * Aggregates and manages Vulkan GPU-side resources (buffers, descriptor sets, pipelines)
 * required to render a drawable object. Responds to renderer events (release/rebuild)
 * and can be safely reused with shared caches for materials, meshes, and textures.
 */
class VulkanRenderResources : public IRenderResources, public IRendererObserver
{
public:
    /**
     * @brief Constructor.
     * @param renderer Pointer to the owning VulkanRenderer instance.
     *
     * Registers itself as an observer for resource (re)creation events.
     */
    VulkanRenderResources(VulkanRenderer* renderer);

    /**
     * @brief Destructor.
     *
     * Unregisters from the renderer and calls Shutdown() to release all resources.
     */
    ~VulkanRenderResources();

    /**
     * @copydoc IRenderResources::Initialize
     */
    void Initialize(Material* material, MeshDataGpu* meshData, TextureCache* textureCache) override;

    /**
     * @copydoc IRenderResources::SubmitResources(Transformations2D&, const PackedMaterialData&)
     */
    void SubmitResources(Transformations2D& transformations, const PackedMaterialData& materialParameters) override;
    
    /**
     * @copydoc IRenderResources::SubmitResources(Transformations3D&, const PackedMaterialData&)
     */
    void SubmitResources(Transformations3D& transformations, const PackedMaterialData& materialParameters) override;

    /**
     * @copydoc IRenderResources::Shutdown
     */
    void Shutdown() override;

protected:
    /**
     * @copydoc IRendererObserver::OnRenderResourcesRelease
     */
    void OnRenderResourcesRelease() override;
    /**
     * @copydoc IRendererObserver::OnRenderResourcesRebuild
     */
    void OnRenderResourcesRebuild() override;

    void AcquireResources();
    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSet();

    void UpdateTransformations(Transformations2D& transformations);
    void UpdateTransformations(Transformations3D& transformations);
    void UpdateMaterialParameters(const PackedMaterialData& materialParameters);
    void DrawIndexed();

private:
    VulkanRenderer* mRenderer;

    Material* mMaterial;
    MeshDataGpu* mMeshData;
    TextureCache* mTextureCache;

    bool bHasCustomMaterialVariables;

    VkBuffer mVertexBuffer;
    VkBuffer mIndexBuffer;

    VkPipeline mGraphicsPipeline;
    VkPipelineLayout mPipelineLayout;

    std::vector<VkBuffer> mTransformationBuffers;
    std::vector<VkDeviceMemory> mTransformationBuffersMemory;

    std::vector<VkBuffer> mMaterialParametersBuffers;
    std::vector<VkDeviceMemory> mMaterialParametersMemory;

    VkDescriptorPool mDescriptorPool;
    std::vector<VkDescriptorSet> mDescriptorSets;
};

} // namespace rendering_engine


/*
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
*/