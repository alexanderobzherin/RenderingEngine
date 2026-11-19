// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "i_window_system.hpp"

struct GLFWwindow;

namespace rendering_engine
{
class IApplication;

/**
 * @brief GLFW-based desktop implementation of IWindowSystem.
 *
 * Manages application windows, event polling, and framebuffer
 * resize notifications on supported desktop platforms.
 *
 * Provides access to the underlying GLFW window handle
 * for subsystems that require native window integration.
 *
 * @see IWindowSystem
 * @see IApplication
 */
class StandaloneDesktopWindow : public IWindowSystem
{
public:
    /**
     * @brief Constructs a desktop window system instance.
     * @param app Reference to the associated application.
     */
    StandaloneDesktopWindow(IApplication& app);
    /** @copydoc IWindowSystem::CreateAppWindow */
    void CreateAppWindow(unsigned int width, unsigned int height, const std::string& title) override;
    /** @copydoc IWindowSystem::PollEvents */
    void PollEvents() override;
    /** @copydoc IWindowSystem::ShouldClose */
    bool ShouldClose() const override;
    /** @copydoc IWindowSystem::GetNativeHandle */
    void* GetNativeHandle() const override;
    /** @copydoc IWindowSystem::GetWindowResolution */
    WindowResolution GetWindowResolution() const override;
    /** @copydoc IWindowSystem::GetFramebufferResolution */
    WindowResolution GetFramebufferResolution() const override;
    /** @copydoc IWindowSystem::Shutdown */
    void Shutdown() override;
    /** @copydoc IWindowSystem::GetApplication */
    const IApplication& GetApplication() override;
    /** @copydoc IWindowSystem::IsFramebufferResized */
    bool IsFramebufferResized() const override { return mFramebufferResized; }
    /** @copydoc IWindowSystem::ResetFramebufferResizedFlag */
    void ResetFramebufferResizedFlag() override { mFramebufferResized = false; }
    /**
     * @brief GLFW framebuffer resize callback.
     *
     * This static function is registered with GLFW to detect when the framebuffer
     * size changes. It sets the internal resize flag for synchronization with
     * the rendering system.
     *
     * @param window Pointer to the GLFW window instance.
     * @param width  New framebuffer width.
     * @param height New framebuffer height.
     */
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    /** @copydoc IWindowSystem::GetFullScreenResolution */
    WindowResolution GetFullScreenResolution() const override;

private:
    IApplication& mApp;
    GLFWwindow* mWindow;
    bool mFramebufferResized;
    WindowResolution mFullScreenRes;
};
} //rendering_engine