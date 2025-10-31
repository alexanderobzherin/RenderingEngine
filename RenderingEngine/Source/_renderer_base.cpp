/*
#include "renderer_base.hpp"
#include "app_clock.hpp"
#include "app_time.hpp"
#include "camera.hpp"

namespace rendering_engine
{
    
RendererBase::RendererBase(char const* appName)
    :
    mIsFullScreen(true),
    mAppName(appName),
    mFramebufferResized(false),
    mWindow(nullptr),
    mAppTime(std::make_shared<AppTime>())
{
}

RendererBase::RendererBase(int const width, int const height, char const* appName)
    :
    mIsFullScreen(false),
    mWidth(width),
    mHeight(height),
    mAppName(appName),
    mFramebufferResized(false),
    mWindow(nullptr),
    mAppTime(std::make_shared<AppTime>())
{
}

void RendererBase::Run()
{
    InitializeWindow();
    InitializeRenderer();
    Initialize();

    for (auto& object : mDrawableObjects)
    {
        object->Initialize();
    }
    AppClock appClock;
    appClock.Reset();

    while( !glfwWindowShouldClose(mWindow) )
    {
        appClock.UpdateAppTime(*mAppTime);

        Update(mAppTime->ElapsedAppTimeMilliseconds());
        Draw();

        glfwPollEvents();
    }

    Shutdown();
}

void RendererBase::Shutdown()
{
    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void RendererBase::InitializeWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if( mIsFullScreen )
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        mWidth = mode->width;
        mHeight = mode->height;
        mWindow = glfwCreateWindow(mWidth, mHeight, mAppName, glfwGetPrimaryMonitor(), nullptr);
    }
    else
    {
        mWindow = glfwCreateWindow(mWidth, mHeight, mAppName, nullptr, nullptr);
    }

    glfwSetWindowUserPointer(mWindow, this);
    glfwSetFramebufferSizeCallback(mWindow, FramebufferResizeCallback);
}

void RendererBase::Initialize()
{
    mCamera = std::make_shared<Camera>(this);
    mCamera->Initialize();
}

void RendererBase::Update(float const delta)
{
    if( mCamera )
    {
        mCamera->Update(delta);
    }

    for( auto& object : mDrawableObjects )
    {
        if( object )
        {
            object->Update(delta);
        }
    }
}

std::shared_ptr<Camera> RendererBase::GetCamera()
{
    return mCamera;
}

std::shared_ptr<AppTime> RendererBase::GetAppTime()
{
    return mAppTime;
}

std::vector<std::shared_ptr<DrawableComponent>>& RendererBase::GetDrawableObjects()
{
    return mDrawableObjects;
}

void RendererBase::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<RendererBase*>(glfwGetWindowUserPointer(window));
    app->mFramebufferResized = true;
}

float RendererBase::GetAspectRation()
{
    return static_cast<float>(mWidth / mHeight);
}

} //rendering_engine

*/