#pragma once

#include "i_window_system.hpp"

class GLFWwindow;

namespace rendering_engine
{
class IApplication;

class StandaloneDesktopWindow : public IWindowSystem
{
public:
    StandaloneDesktopWindow(IApplication& app);

    void CreateAppWindow(unsigned int width, unsigned int height, const std::string& title) override;
    void PollEvents() override;
    bool ShouldClose() const override;
    void* GetNativeHandle() const override;

    void Shutdown() override;
    const IApplication& GetApplication() override;

    bool IsFramebufferResized() const override { return mFramebufferResized; }
    void ResetFramebufferResizedFlag() override { mFramebufferResized = false; }

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    WindowResolution GetFullScreenResolution() const override;

private:
    IApplication& mApp;
    GLFWwindow* mWindow;
    bool mFramebufferResized;
    WindowResolution mFullScreenRes;
};
} //rendering_engine