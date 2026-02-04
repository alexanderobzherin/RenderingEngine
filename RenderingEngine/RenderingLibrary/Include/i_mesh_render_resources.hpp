// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "vertex_declarations.hpp"

#include <string>
#include <vector>

namespace rendering_engine
{

/**
 * @brief Interface for GPU mesh resource management.
 *
 * This interface abstracts the creation, management, and destruction of mesh-related GPU resources.
 * It supports multiple vertex types, index buffers, and provides utilities for buffer state and size queries.
 */
class IMeshRenderResources
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~IMeshRenderResources() = default;

    /**
     * @brief Create a GPU vertex buffer from 2D vertex data.
     * @param vertices Vector of 2D vertex data.
     */
    virtual void CreateVertexBuffer(std::vector<Vertex2D> vertices) = 0;

    /**
     * @brief Create a GPU vertex buffer from 3D unlit vertex data.
     * @param vertices Vector of VertexPositionTextureColor data.
     */
    virtual void CreateVertexBuffer(std::vector<VertexPositionColorTexture> vertices) = 0;

    /**
     * @brief Create a GPU vertex buffer from 3D lit vertex data.
     * @param vertices Vector of VertexPositionTextureColorNormalTangent data.
     */
    virtual void CreateVertexBuffer(std::vector<VertexPositionColorTextureNormalTangent> vertices) = 0;

    /**
     * @brief Create a GPU index buffer.
     * @param indices Vector of indices.
     */
    virtual void CreateIndexBuffer(std::vector<uint32_t> indices) = 0;

    /**
     * @brief Check if the buffers are currently resident on the GPU.
     * @return True if the buffers are allocated on the GPU, false otherwise.
     */
    virtual bool IsOnGPU() const = 0;


    /**
     * @brief Release all GPU resources associated with this mesh.
     */
    virtual void Shutdown() = 0;

    /**
     * @brief Get the size (in bytes) of the GPU vertex buffer.
     * @return Size in bytes of the vertex buffer.
     */
    virtual size_t GetVertexBufferSize() const = 0;

    /**
     * @brief Get the size (in bytes) of the GPU index buffer.
     * @return Size in bytes of the index buffer.
     */
    virtual size_t GetIndexBufferSize() const = 0;
};

} // namespace rendering_engine