// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "i_mesh_render_resources.hpp"
#include <vulkan/vulkan.h>
#include <string>

namespace rendering_engine
{
class VulkanRenderer;

/**
 * @brief Vulkan implementation of the mesh GPU resource interface.
 *
 * Manages Vulkan buffers and device memory for mesh vertex and index data.
 * Supports multiple vertex types and provides direct access to underlying VkBuffer handles.
 */
class VulkanMeshResources : public IMeshRenderResources
{
public:
    /**
     * @brief Construct Vulkan mesh resources.
     * @param renderer Pointer to the owning VulkanRenderer.
     */
    VulkanMeshResources(VulkanRenderer* renderer);

    /**
     * @brief Create a GPU vertex buffer from 2D vertex data.
     * @param vertices Vector of 2D vertex data.
     */
    void CreateVertexBuffer(std::vector<Vertex2D> vertices) override;

    /**
     * @brief Create a GPU vertex buffer from 3D unlit vertex data.
     * @param vertices Vector of VertexPositionColorTexture data.
     */
    void CreateVertexBuffer(std::vector<VertexPositionColorTexture> vertices) override;

    /**
     * @brief Create a GPU vertex buffer from 3D lit vertex data.
     * @param vertices Vector of VertexPositionColorTextureNormalTangent data.
     */
    void CreateVertexBuffer(std::vector<VertexPositionColorTextureNormalTangent> vertices) override;

    /**
     * @brief Create a GPU index buffer.
     * @param indices Vector of indices.
     */
    void CreateIndexBuffer(std::vector<uint32_t> indices) override;

    /**
     * @brief Query whether buffers are currently resident on the GPU.
     * @return True if buffers are allocated and uploaded, false otherwise.
     */
    inline bool IsOnGPU() const override;

    /**
     * @brief Release all GPU resources associated with this mesh.
     */
    void Shutdown() override;

    /**
     * @brief Get the Vulkan vertex buffer handle.
     * @return VkBuffer handle for the vertex buffer.
     */
    VkBuffer GetVertexBuffer() const;

    /**
     * @brief Get the Vulkan index buffer handle.
     * @return VkBuffer handle for the index buffer.
     */
    VkBuffer GetIndexBuffer() const;

    /**
     * @brief Get the size (in bytes) of the GPU vertex buffer.
     * @return Size in bytes of the vertex buffer.
     */
    inline size_t GetVertexBufferSize() const override;

    /**
     * @brief Get the size (in bytes) of the GPU index buffer.
     * @return Size in bytes of the index buffer.
     */
    inline size_t GetIndexBufferSize() const override;

private:
    VulkanRenderer* mRenderer;
    bool bIsOnGPU;

    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    size_t mVertexBufferSize;

    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
    size_t mIndexBufferSize;
};

} // namespace rendering_engine