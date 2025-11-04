// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <string>

namespace rendering_engine
{

class Material;

/**
 * @brief Interface for backend-specific material GPU resources.
 *
 * This abstract interface allows `Material` instances to delegate the creation and
 * destruction of rendering resources to the graphics backend.
 *
 * Concrete implementations should inherit from this and implement the resource 
 * lifecycle methods.
 */
class IMaterialRenderResources
{
public:
    virtual ~IMaterialRenderResources() = default;

   /**
    * @brief Initializes the GPU-side representation of a material.
    *
    * Called by the `Material` class after parameters and textures are set.
    *
    * @param material Pointer to the owning Material instance.
    */
    virtual void Initialize(Material* material) = 0;

   /**
    * @brief Releases all GPU resources associated with this material.
    *
    * Called during cleanup or when the renderer rebuilds resources.
    */
    virtual void Shutdown() = 0;
};

} // namespace rendering_engine