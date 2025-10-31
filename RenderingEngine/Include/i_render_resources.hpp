// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

namespace rendering_engine
{

struct Transformations3D;
struct Transformations2D;
struct PackedMaterialData;
class Material;
class MeshDataGpu;
class TextureCache;

/**
 * @brief Interface for rendering backend resource aggregation and submission.
 *
 * Implementations of this interface aggregate all GPU-side objects and logic
 * required to render a drawable entity.
 * The interface is backend-agnostic and can be implemented for any graphics API.
 *
 * The interface is designed to work with shared resource caches (material, mesh, texture),
 * typically owned elsewhere and passed to Initialize().
 *
 * @note Lifetime management of Material, MeshDataGpu, and TextureCache must be handled externally.
 */
class IRenderResources
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~IRenderResources() = default;

    /**
     * @brief Initializes GPU-side resources using provided material, mesh, and texture cache.
     *
     * @param material Pointer to the Material describing the shader and its parameters.
     * @param meshData Pointer to MeshDataGpu containing vertex and index buffers.
     * @param textureCache Pointer to the TextureCache containing all loaded textures.
     *
     * @note Ownership of parameters is not transferred; caller must ensure they remain valid.
     */
    virtual void Initialize(Material* material, MeshDataGpu* meshData, TextureCache* textureCache) = 0;

    /**
     * @brief Updates GPU resources and issues a draw call for a 2D object.
     *
     * @param transformations Transformation data (2D).
     * @param materialParameters Packed structure with material parameters for the draw call.
     */
    virtual void SubmitResources(Transformations2D& transformations, const PackedMaterialData& materialParameters) = 0;

    /**
     * @brief Updates GPU resources and issues a draw call for a 3D object.
     *
     * @param transformations Transformation data (3D).
     * @param materialParameters Packed structure with material parameters for the draw call.
     */
    virtual void SubmitResources(Transformations3D& transformations, const PackedMaterialData& materialParameters) = 0;
    
    /**
     * @brief Releases all allocated GPU resources for this object.
     */
    virtual void Shutdown() = 0;
};

} // namespace rendering_engine