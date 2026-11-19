// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "i_application.hpp"
#include <string>

namespace rendering_engine
{
/**
 * @struct WindowResolution
 * @brief Represents the pixel resolution of a window or display.
 */
struct WindowResolution
{
    /** @brief Window or screen width in pixels. */
    unsigned int width;
    /** @brief Window or screen height in pixels. */
    unsigned int height;
};

/**
 * @brief Abstract interface for window management.
 *
 * Provides a platform-independent interface for creating windows,
 * processing events, and querying window and framebuffer state.
 *
 * @see IApplication
 */
class IWindowSystem
{
public:
    /**
     * @brief Creates the main application window.
     * @param width  Width of the window in pixels.
     * @param height Height of the window in pixels.
     * @param title  Title displayed in the window caption.
     */
    virtual void CreateAppWindow(unsigned int width, unsigned int height, const std::string& title) = 0;
    /**
     * @brief Polls and processes window system events.
     */
    virtual void PollEvents() = 0;
    /** @brief Checks whether the user has requested to close the window. */
    virtual bool ShouldClose() const = 0;
    /**
     * @brief Returns the underlying native window handle.
     *
     * @return Platform-specific window handle as an opaque pointer.
     */
    virtual void* GetNativeHandle() const = 0;
    /**
     * @brief Returns the current window dimensions in screen-coordinate units.
     *
     * These dimensions correspond to the coordinate system used by
     * mouse cursor positions. They may differ from framebuffer dimensions
     * on high-DPI displays.
     *
     * @return Current window width and height.
     */
    virtual WindowResolution GetWindowResolution() const = 0;
    /**
     * @brief Returns the current framebuffer dimensions in pixels.
     *
     * These dimensions are used by the rendering backend to determine
     * the actual drawable surface size.
     *
     * @return Current framebuffer width and height in pixels.
     */
    virtual WindowResolution GetFramebufferResolution() const = 0;
    /** @brief Checks if the framebuffer has been resized since the last frame. */
    virtual bool IsFramebufferResized() const = 0;
    /** @brief Resets the framebuffer resized flag after handling a resize event. */
    virtual void ResetFramebufferResizedFlag() = 0;
    /** @brief Performs cleanup and releases window-related resources. */
    virtual void Shutdown() = 0;
    /**
     * @brief Retrieves a reference to the owning application instance.
     * @return Reference to the associated IApplication object.
     */
    virtual const IApplication& GetApplication() = 0;
    /**
     * @brief Queries the resolution of the full-screen display mode.
     * @return The native screen resolution when running in full-screen mode.
     */
    virtual WindowResolution GetFullScreenResolution() const = 0;
    /** @brief Virtual destructor for safe polymorphic cleanup. */
    virtual ~IWindowSystem() = default;
};
} //rendering_engine