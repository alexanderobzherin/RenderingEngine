// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

namespace rendering_engine
{
class IRenderResources;
class ITextureRenderResources;
class IMaterialRenderResources;
class IMeshRenderResources;
class IRenderResources;
class IRendererObserver;

/**
 * @class IRenderer
 * @brief Defines an abstract interface for rendering backends.
 *
 * Provides a unified API for initializing, executing, and shutting down
 * the rendering system. Specific graphics backends implement this interface 
 * to handle platform-dependent operations.
 *
 * @see IRenderResources
 * @see IRendererObserver
 */
class IRenderer 
{
public:
    /** @brief Initializes all rendering subsystems and GPU resources. */
    virtual void InitializeRenderer() = 0;
    /** @brief Executes a full frame rendering cycle. */
    virtual void DrawFrame() = 0;
    /** @brief Begins frame rendering operations. @return True if the frame can proceed. */
    virtual bool BeginFrame() = 0;
    /** @brief Begins the active render pass for the current frame. */
    virtual void BeginRenderPass() = 0;
    /** @brief Ends the active render pass. */
    virtual void EndRenderPass() = 0;
    /** @brief Completes the current frame rendering and presents the result. */
    virtual void EndFrame() = 0;
    /** @brief Waits until the GPU has finished all pending rendering operations. */
    virtual void WaitIdle() = 0;
    /** @brief Destroys and cleans up all rendering resources. */
    virtual void ShutdownRenderer() = 0;
    /**
     * @brief Registers an observer for rendering events.
     * @param notifier Pointer to the observer to register.
     */
    virtual void RegisterObserver(IRendererObserver* notifier) = 0;
    /**
     * @brief Unregisters a previously registered observer.
     * @param notifier Pointer to the observer to remove.
     */
    virtual void UnregisterObserver(IRendererObserver* notifier) = 0;
    /**
     * @brief Provides access to the general rendering resource manager.
     * @return Pointer to the IRenderResources interface.
     */
    virtual IRenderResources* ProvideRenderResources() const = 0;
    /**
     * @brief Provides access to texture-related GPU resources.
     * @return Pointer to the ITextureRenderResources interface.
     */
    virtual ITextureRenderResources* ProvideTextureRenderResources() const = 0;
    /**
     * @brief Provides access to material-related GPU resources.
     * @return Pointer to the IMaterialRenderResources interface.
     */
    virtual IMaterialRenderResources* ProvideMaterialRenderResources() const = 0;
    /**
     * @brief Provides access to mesh-related GPU resources.
     * @return Pointer to the IMeshRenderResources interface.
     */
    virtual IMeshRenderResources* ProvideMeshRenderResources() const = 0;
    /** @brief Virtual destructor for safe polymorphic deletion. */
    virtual ~IRenderer() = default;
};

} //rendering_engine