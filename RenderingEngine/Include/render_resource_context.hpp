// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

namespace rendering_engine
{
class IRenderer;
class ModelCache;
class TextureCache;
class MaterialCache;
/**
 * @struct RenderResourceContext
 * @brief Aggregates pointers to global rendering resource managers.
 *
 * This structure provides access to core rendering subsystems required
 * for creating and managing GPU resources. It is typically passed to
 * drawable objects, materials, and scene components that need access to
 * shared renderer and caches.
 *
 * @see IRenderer, ModelCache, TextureCache, MaterialCache
 */
struct RenderResourceContext
{
    IRenderer* renderer;
    ModelCache* meshCache;
    TextureCache* textureCache;
    MaterialCache* materialCache;
};

} // namespace rendering_engine