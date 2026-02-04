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
 * @class IWindowSystem
 * @brief Abstract interface for platform-specific window management.
 *
 * Provides a unified API for creating, managing, and querying window state.
 * Platform-specific implementations (e.g., GLFW, SDL, Win32, X11) are expected
 * to implement this interface to integrate with the rendering engine.
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
    /** @brief Polls and processes OS-level window events (input, resize, close, etc.). */
    virtual void PollEvents() = 0;
    /** @brief Checks whether the user has requested to close the window. */
    virtual bool ShouldClose() const = 0;
    /**
     * @brief Returns a pointer to the underlying native window handle.
     * @return Platform-specific native handle (e.g., HWND, GLFWwindow*, X11 Window).
     */
    virtual void* GetNativeHandle() const = 0;
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