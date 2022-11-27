#pragma once

#include <GLFW/glfw3.h>
#include <memory>

#include "drawable_component.hpp"

namespace rendering_engine
{
class AppTime;

class RendererBase
{
public:
	RendererBase(char const* appName);
    RendererBase(int const width, int const height, char const* appName);
	virtual ~RendererBase() = default;

    virtual void Run();

protected:
    virtual void Shutdown();
    virtual void InitializeWindow();
    virtual void InitializeRenderer() = 0;

    virtual void Initialize();
    virtual void Update(float const delta);
    virtual void Draw();

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

protected:
    bool mIsFullScreen;
    unsigned int mWidth;
    unsigned int mHeight;
    char const* mAppName;

    bool mFramebufferResized;

    GLFWwindow* mWindow;
    std::shared_ptr<AppTime> mAppTime;
    std::vector<std::shared_ptr<DrawableComponent>> mDrawableObjects;
};

} //rendering_engine