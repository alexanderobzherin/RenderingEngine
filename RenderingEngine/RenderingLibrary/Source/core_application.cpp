#include "core_application.hpp"
#include "standalone_window_system.hpp"
#include "vulkan_renderer.hpp"
#include "app_clock.hpp"
#include "app_time.hpp"
#include "scene_manager.hpp"
#include "utility.hpp"
#include "logger.hpp"

#include <exception>

namespace rendering_engine
{
CoreApplication::CoreApplication()
    :
    mAppTime(std::make_shared<AppTime>()),
    mWindowSystem{ nullptr },
    mRenderer{ nullptr }
{
    AppConfig appConfig = Utility::ReadConfigFile();

    mAppName = appConfig.appName.c_str();
    bIsFullScreen = appConfig.isFullScreen;
    mWidth = static_cast<unsigned int>(appConfig.screenWidth);
    mHeight = static_cast<unsigned int>(appConfig.screenHeight);
}
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

CoreApplication::~CoreApplication(){};

void CoreApplication::Initialize()
{
    Logger::Get().Initialize(mAppName);

    LOG_DEBUG("This is debug level message");
    LOG_INFO("This is info level message");
    LOG_WARNING("This is warning level message");
    LOG_ERROR("This is error level message");

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

    Shutdown();
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
        mSceneManager->Draw();
        mRenderer->EndRenderPass();
        mRenderer->EndFrame();
    }
}

void CoreApplication::Shutdown()
{
    mRenderer->WaitIdle();
    mSceneManager->Shutdown();
    mRenderer->ShutdownRenderer();
    mWindowSystem->Shutdown();

    Logger::Get().Shutdown();
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