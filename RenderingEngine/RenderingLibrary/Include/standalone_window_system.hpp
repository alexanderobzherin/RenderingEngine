// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "i_window_system.hpp"

class GLFWwindow;

namespace rendering_engine
{
class IApplication;

/**
 * @class StandaloneDesktopWindow
 * @brief Desktop implementation of the IWindowSystem interface using GLFW.
 *
 * Provides window creation, event polling, and framebuffer resize handling
 * for standalone desktop environments. This class represents the concrete
 * platform layer for rendering contexts that rely on desktop windowing systems
 * (e.g., Windows, Linux, FreeBSD).
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