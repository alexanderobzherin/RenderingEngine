#include "core_application.hpp"
#include "standalone_window_system.hpp"
#include "vulkan_renderer.hpp"
#include "app_clock.hpp"
#include "app_time.hpp"
#include "scene_manager.hpp"
#include "test_triangle.hpp"
#include "test_sprite2D.hpp"

#include <exception>

namespace rendering_engine
{
CoreApplication::CoreApplication(char const* appName)
    :
    bIsFullScreen{true},
    mAppName(appName),
    mAppTime(std::make_shared<AppTime>()),
    mWindowSystem{nullptr},
    mRenderer{nullptr}
{
}

CoreApplication::CoreApplication(unsigned int width, unsigned int height, char const* appName)
    :
    bIsFullScreen{false},
    mWidth{width},
    mHeight{height},
    mAppName(appName),
    mAppTime(std::make_shared<AppTime>()),
    mWindowSystem{ nullptr },
    mRenderer{ nullptr }
{
}

CoreApplication::CoreApplication(unsigned int width,
                                 unsigned int height,
                                 char const* appName,
                                 std::shared_ptr<IWindowSystem> windowSystem,
                                 std::shared_ptr<IRenderer> renderer)
    :
    bIsFullScreen{ false },
    mWidth{ width },
    mHeight{ height },
    mAppName(appName),
    mAppTime(std::make_shared<AppTime>()),
    mWindowSystem{ windowSystem },
    mRenderer{ renderer }
{}

CoreApplication::CoreApplication(char const* appName,
                                 std::shared_ptr<IWindowSystem> windowSystem,
                                 std::shared_ptr<IRenderer> renderer)
    :
    bIsFullScreen{ true },
    mWidth{ 0 },
    mHeight{ 0 },
    mAppName(appName),
    mAppTime(std::make_shared<AppTime>()),
    mWindowSystem{ windowSystem },
    mRenderer{ renderer }
{}

void CoreApplication::Initialize()
{
    if (!mWindowSystem)
    {
        mWindowSystem = std::make_shared<StandaloneDesktopWindow>(*this);
    }
    if (!mRenderer)
    {
        mRenderer = std::make_shared<VulkanRenderer>(*mWindowSystem.get());
    }

    mWindowSystem->CreateAppWindow(mWidth, mHeight, mAppName);
    if (bIsFullScreen)
    {
        mWidth = mWindowSystem->GetFullScreenResolution().width;
        mHeight = mWindowSystem->GetFullScreenResolution().height;
    }
    mRenderer->InitializeRenderer();

    //mTestTriangle = std::make_shared<TestTriangle>();
    //VulkanRenderer* vulkanRenderer = dynamic_cast<VulkanRenderer*>(mRenderer.get());
    //mTestTriangle->Initialize(vulkanRenderer);

    //mTestSprite2D = std::make_shared<TestSprite2D>();
    //VulkanRenderer* vulkanRenderer = dynamic_cast<VulkanRenderer*>(mRenderer.get());
    //mTestSprite2D->Initialize(vulkanRenderer);

    mSceneManager = std::make_shared<SceneManager>(mRenderer.get(), this);
    mSceneManager->Initialize();
}

void CoreApplication::Run()
{
    AppClock appClock;
    appClock.Reset();

    while (!mWindowSystem->ShouldClose())
    {
        appClock.UpdateAppTime(*mAppTime);
        mWindowSystem->PollEvents();
        Update(mAppTime->ElapsedAppTimeMilliseconds());
        Draw();
    }
}

void CoreApplication::Update(float deltaTime)
{
    if (mSceneManager)
    {
        mSceneManager->Update(deltaTime);
    }
}

void CoreApplication::Draw()
{
    if (mRenderer->BeginFrame())
    {
        mRenderer->BeginRenderPass();
        //mTestTriangle->Draw();
        //mTestSprite2D->Draw();
        mSceneManager->Draw();
        mRenderer->EndRenderPass();
        mRenderer->EndFrame();
    }
}

void CoreApplication::Shutdown()
{
    mRenderer->WaitIdle();
    //mTestTriangle->Shutdown();
    //mTestSprite2D->Shutdown();
    mSceneManager->Shutdown();
    mRenderer->ShutdownRenderer();
    mWindowSystem->Shutdown();
}

ScreenSettings CoreApplication::GetScreenSettings() const
{
    ScreenSettings ss;
    ss.name = mAppName;
    ss.isFullScreen = bIsFullScreen;
    ss.height = mHeight;
    ss.width = mWidth;
    
    return ss;
}

} // rendering_engine