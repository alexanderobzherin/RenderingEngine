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

struct RenderResourceContext
{
    IRenderer* renderer;
    ModelCache* meshCache;
    TextureCache* textureCache;
    MaterialCache* materialCache;
};

} // namespace rendering_engine