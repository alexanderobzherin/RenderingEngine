// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <memory>
#include <string>
#include "vertex_declarations.hpp"
#include "render_resource_context.hpp"
#include "material_types.hpp"

namespace rendering_engine
{
class IRenderer;
class IRenderResources;
class Material;
class ModelCache;
class TextureCache;
class MaterialCache;
class MeshDataGpu;

/**
 * @class DrawableComponent
 * @brief Abstract base for all drawable (renderable) objects in the engine.
 *
 * Provides core functionality for accessing render resources (meshes, materials, textures)
 * and defines the initialization, update, and draw lifecycle for renderable entities.
 *
 * Derived classes (such as Drawable2D and Drawable3D) implement type-specific behavior for
 * spatial transforms, camera use, and render submission.
 *
 * @note Not copyable or assignable.
 * @see Drawable2D, Drawable3D
 */
class DrawableComponent
{
public:
    /**
     * @brief Constructs the DrawableComponent with a resource context.
     * @param renderContext Rendering resource context (renderer, caches).
     */
    DrawableComponent(RenderResourceContext renderContext);

    /// Virtual destructor.
    virtual ~DrawableComponent() = default;

    /**
     * @brief Initializes render resource pointers (material, mesh, etc.).
     *        Must be called after setting material and mesh names.
     */
    virtual void Initialize();

    /**
     * @brief Updates logic (animation, movement, etc.) for this drawable.
     * @param deltaTime Time step (seconds).
     */
    virtual void Update(float deltaTime) = 0;

    virtual void Shutdown();

    DrawableComponent(const DrawableComponent&) = delete;
    DrawableComponent& operator=(const DrawableComponent&) = delete;

protected:
    /**
     * @brief Sets the material to use (by name).
     * @param materialName Name of the material.
     */
    void SetMaterialName(const std::string& materialName);

    /**
     * @brief Sets the mesh to use (by name).
     * @param meshName Name of the mesh.
     */
    void SetMeshName(const std::string& meshName);

protected:
    RenderResourceContext mRenderContext;
    std::unique_ptr<IRenderResources> mRenderResources;

    std::string mMaterialName;
    std::string mMeshName;

    Material* mMaterial;
    PackedMaterialData mMaterialParameters;
    MeshDataGpu* mMeshData;
};

}

