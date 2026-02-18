#include "core_application.hpp"
#include "standalone_window_system.hpp"
#include "vulkan_renderer.hpp"
#include "app_clock.hpp"
#include "app_time.hpp"
#include "scene_manager.hpp"
#include "utility.hpp"
#include "logger.hpp"

#include <exception>
#include <thread>

namespace rendering_engine
{
CoreApplication::CoreApplication()
    :
    mAppTime(std::make_shared<AppTime>()),
    mWindowSystem{ nullptr },
    mRenderer{ nullptr }
{
    mAppConfig = Utility::ReadConfigFile();

    mAppName = mAppConfig.appName.c_str();
    bIsFullScreen = mAppConfig.isFullScreen;
    mWidth = static_cast<unsigned int>(mAppConfig.screenWidth);
    mHeight = static_cast<unsigned int>(mAppConfig.screenHeight);
}
CoreApplication::CoreApplication(char const* appName)
    :
    bIsFullScreen{true},
    mAppName(appName),
    mAppTime(std::make_shared<AppTime>()),
    mWindowSystem{nullptr},
    mRenderer{nullptr}
{
    mAppConfig = Utility::ReadConfigFile();
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
    mAppConfig = Utility::ReadConfigFile();
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
{
    mAppConfig = Utility::ReadConfigFile();
}

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
{
    mAppConfig = Utility::ReadConfigFile();
}

CoreApplication::~CoreApplication(){};

void CoreApplication::Initialize()
{
    Logger::Get().Initialize(mAppName);

    LOG_INFO("Initializing CoreApplication...");
    LOG_INFO(std::string("Application name: ") + mAppName);

    auto startTime = std::chrono::steady_clock::now();

    if (!mWindowSystem)
    {
        LOG_DEBUG("Creating StandaloneDesktopWindow...");
        mWindowSystem = std::make_shared<StandaloneDesktopWindow>(*this);
    }
    if (!mRenderer)
    {
        LOG_DEBUG("Creating VulkanRenderer...");
        mRenderer = std::make_shared<VulkanRenderer>(*mWindowSystem.get());
    }

    LOG_INFO("Creating application window...");
    mWindowSystem->CreateAppWindow(mWidth, mHeight, mAppName);
    if (bIsFullScreen)
    {
        mWidth = mWindowSystem->GetFullScreenResolution().width;
        mHeight = mWindowSystem->GetFullScreenResolution().height;

        LOG_INFO("Fullscreen mode enabled. Resolution set to: "
            + std::to_string(mWidth) + "x" + std::to_string(mHeight));
    }
    LOG_INFO("Initializing renderer...");
    mRenderer->InitializeRenderer();

    LOG_INFO("Initializing SceneManager...");
    mSceneManager = std::make_shared<SceneManager>(mRenderer.get(), this);
    mSceneManager->Initialize();

    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    LOG_INFO("CoreApplication initialized in " + std::to_string(duration) + " ms.");
}

void CoreApplication::Run()
{
    LOG_INFO("Entering main loop.");

    AppClock appClock;
    appClock.Reset();
    
    const float targetFrameTimeMs =
        (mAppConfig.targetFPS > 0) ? (1000.0f / static_cast<float>(mAppConfig.targetFPS)) : 0.0f;
    
    while (!mWindowSystem->ShouldClose())
    {
        const auto frameStart = std::chrono::steady_clock::now();

        appClock.UpdateAppTime(*mAppTime);
        mWindowSystem->PollEvents();

        const auto updateStart = std::chrono::steady_clock::now();
        Update(mAppTime->ElapsedAppTimeSeconds());
        const auto updateEnd = std::chrono::steady_clock::now();

        const auto drawStart = std::chrono::steady_clock::now();
        Draw();
        const auto drawEnd = std::chrono::steady_clock::now();

        const auto frameEnd = std::chrono::steady_clock::now();

        mFrameMetrics.updateTimeMs =
            std::chrono::duration<float, std::milli>(updateEnd - updateStart).count();
        mFrameMetrics.drawTimeMs =
            std::chrono::duration<float, std::milli>(drawEnd - drawStart).count();
        mFrameMetrics.frameDurationMs =
            std::chrono::duration<float, std::milli>(frameEnd - frameStart).count();

        if (mFrameMetrics.frameDurationMs > 0.0f)
        {
            mFrameMetrics.fpsRaw =
                1000.0f / mFrameMetrics.frameDurationMs;

            const float alpha = 0.1f;

            if (mFrameMetrics.fpsSmoothed == 0.0f)
                mFrameMetrics.fpsSmoothed = mFrameMetrics.fpsRaw;
            else
                mFrameMetrics.fpsSmoothed =
                alpha * mFrameMetrics.fpsRaw +
                (1.0f - alpha) * mFrameMetrics.fpsSmoothed;
        }

        // Frame pacing (sleep)
        if (mAppConfig.targetFPS > 0)
        {
            const auto afterMetrics = std::chrono::steady_clock::now();

            const float actualFrameTimeMs =
                std::chrono::duration<float, std::milli>(afterMetrics - frameStart).count();

            if (actualFrameTimeMs < targetFrameTimeMs)
            {
                const auto sleepDuration =
                    std::chrono::duration<float, std::milli>(
                        targetFrameTimeMs - actualFrameTimeMs);

                std::this_thread::sleep_for(sleepDuration);
            }
        }

    }

    LOG_INFO("Main loop exited.");

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
    LOG_INFO("Shutting down CoreApplication...");
    mRenderer->WaitIdle();
    LOG_DEBUG("Shutting down SceneManager...");
    mSceneManager->Shutdown();
    LOG_DEBUG("Shutting down Renderer...");
    mRenderer->ShutdownRenderer();
    LOG_DEBUG("Shutting down WindowSystem...");
    mWindowSystem->Shutdown();
    LOG_INFO("CoreApplication shutdown complete.");
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

FrameMetrics CoreApplication::GetFrameMetrics() const
{
    return mFrameMetrics;
}

} // rendering_engine