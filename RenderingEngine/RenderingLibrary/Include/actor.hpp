// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "drawable_component.hpp"

namespace rendering_engine
{
class Scene;
class Camera;

class Actor
{
public:
    /**
     * @brief Constructs the Actor with a resource context.
     * @param renderContext Rendering resource context (renderer, caches).
     */
    Actor(Scene& scene);

    /// Virtual destructor.
    virtual ~Actor() = default;

    /**
     * @brief Initializes render resource pointers (material, mesh, etc.).
     */
    virtual void Initialize();

    /**
     * @brief Updates logic (animation, movement, etc.) for this drawable.
     * @param deltaTime Time step (seconds).
     */
    virtual void Update(float deltaTime);

    virtual void Draw(const Camera& camera);

    RenderResourceContext GetRenderContext() const;

    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;

protected:
    RenderResourceContext mRenderContext;
    Scene& mScene;

};

} // namespace rendering_engine