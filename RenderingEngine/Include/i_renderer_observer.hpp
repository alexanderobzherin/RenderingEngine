// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

namespace rendering_engine
{

/*
 * @brief Interface for observing renderer resource lifecycle events.
 *
 * Classes implementing this interface can be registered as observers with the renderer.
 * The renderer will call these methods when GPU resources must be released (e.g., device loss, swapchain recreation)
 * and when resources must be rebuilt (e.g., after device reset or window resize).
 *
 * This enables resource containers (such as caches or per-object resources) to correctly manage
 * device-dependent memory and handles, ensuring a robust and backend-agnostic rendering workflow.
 */
class IRendererObserver
{
public:
    /**
     * @brief Renderer callback: release all GPU resources (used during device loss/reset).
     *
     * This method will be called before any device or swapchain is destroyed,
     * allowing the observer to safely release all handles
     * and deallocate any GPU memory.
     */
    virtual void OnRenderResourcesRelease() = 0;

    /**
     * @brief Renderer callback: re-upload or recreate all GPU resources (used after device reset/rebuild).
     *
     * This method will be called after the device or swapchain is recreated,
     * allowing the observer to re-upload or recreate all necessary resources for rendering.
     */
    virtual void OnRenderResourcesRebuild() = 0;

    /**
     * @brief Virtual destructor.
     */
    virtual ~IRendererObserver() = default;
};

} // namespace rendering_engine