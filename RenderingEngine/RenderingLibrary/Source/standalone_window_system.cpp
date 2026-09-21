#include "standalone_window_system.hpp"
#include <GLFW/glfw3.h>
#include <limits>
#include <stdexcept>
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
        mFullScreenRes.width = static_cast<unsigned int>(mode->width);
        mFullScreenRes.height = static_cast<unsigned int>(mode->height);
        mWindow = glfwCreateWindow(mode->width,
                                   mode->height,
                                   title.c_str(),
                                   glfwGetPrimaryMonitor(), nullptr);
    }
    else
    {
        if (width > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
            height > static_cast<unsigned int>(std::numeric_limits<int>::max()))
        {
            throw std::out_of_range("Window dimensions exceed GLFW supported range.");
        }

        mWindow = glfwCreateWindow(
            static_cast<int>(width),
            static_cast<int>(height),
            title.c_str(),
            nullptr,
            nullptr);
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
    static_cast<void>(width);
    static_cast<void>(height);

    auto app = reinterpret_cast<StandaloneDesktopWindow*>(glfwGetWindowUserPointer(window));
    app->mFramebufferResized = true;
}

WindowResolution StandaloneDesktopWindow::GetFullScreenResolution() const
{
    return mFullScreenRes;
}


}