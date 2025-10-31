#include "standalone_window_system.hpp"
#include <GLFW/glfw3.h>
#include "i_application.hpp"

namespace rendering_engine
{
    StandaloneDesktopWindow::StandaloneDesktopWindow(IApplication& app)
	:
	mApp(app),
	mWindow{nullptr},
    mFramebufferResized(false)
{
}

void StandaloneDesktopWindow::CreateAppWindow(unsigned int width, unsigned int height, const std::string& title)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if (mApp.GetScreenSettings().isFullScreen)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        mFullScreenRes.width = mode->width;
        mFullScreenRes.height = mode->height;
        mWindow = glfwCreateWindow(mode->width,
                                   mode->height,
                                   mApp.GetScreenSettings().name.c_str(), 
                                   glfwGetPrimaryMonitor(), nullptr);
    }
    else
    {
        mWindow = glfwCreateWindow(mApp.GetScreenSettings().width,
                                   mApp.GetScreenSettings().height,
                                   mApp.GetScreenSettings().name.c_str(),
                                   nullptr, nullptr);
    }

    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, FramebufferResizeCallback);
}

void StandaloneDesktopWindow::PollEvents()
{
    glfwPollEvents();
}

bool StandaloneDesktopWindow::ShouldClose() const
{
    return glfwWindowShouldClose(mWindow);
}

void* StandaloneDesktopWindow::GetNativeHandle() const
{
    return static_cast<void*>(mWindow);
}

void StandaloneDesktopWindow::Shutdown()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

const IApplication& StandaloneDesktopWindow::GetApplication()
{
    return mApp;
}

void StandaloneDesktopWindow::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<StandaloneDesktopWindow*>(glfwGetWindowUserPointer(window));
    app->mFramebufferResized = true;
}

WindowResolution StandaloneDesktopWindow::GetFullScreenResolution() const
{
    return mFullScreenRes;
}


}