#pragma once

#include <GLFW/glfw3.h>
#include <memory>

#include "drawable_component.hpp"

namespace rendering_engine
{
class AppTime;
class Camera;

class RendererBase
{
public:
	RendererBase(char const* appName);
    RendererBase(int const width, int const height, char const* appName);
	virtual ~RendererBase() = default;

    virtual void Run();

    virtual float GetAspectRation();
    std::shared_ptr<Camera> GetCamera();
    std::shared_ptr<AppTime> GetAppTime();

    std::vector<std::shared_ptr<DrawableComponent>>& GetDrawableObjects();

protected:
    virtual void Shutdown();
    virtual void InitializeWindow();
    virtual void InitializeRenderer() = 0;

    virtual void Initialize();
    virtual void Update(float const delta);
    virtual void Draw() = 0;

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

protected:
    bool mIsFullScreen;
    unsigned int mWidth;
    unsigned int mHeight;
    char const* mAppName;

    bool mFramebufferResized;

    GLFWwindow* mWindow;
    std::shared_ptr<AppTime> mAppTime;
    std::shared_ptr<Camera> mCamera;
    std::vector<std::shared_ptr<DrawableComponent>> mDrawableObjects;
};

} //rendering_engine