
#include "xinput_debug_application.hpp"
#include "xinput2_debug_listener.hpp"

#include "logger.hpp"
#include "standalone_window_system.hpp"

namespace application
{

XInputDebugApplication::XInputDebugApplication() = default;

XInputDebugApplication::~XInputDebugApplication() = default;

void XInputDebugApplication::Initialize()
{
    CoreApplication::Initialize();

    auto* desktopWindow = dynamic_cast<StandaloneDesktopWindow*>(mWindowSystem.get());

    if (!desktopWindow)
    {
        throw std::runtime_error(
            "XInputDebug requires StandaloneDesktopWindow"
        );
    }

    const auto xWindow = desktopWindow->GetX11WindowId();
    
    mTouchListener = std::make_unique<XInput2DebugListener>(xWindow);
    LOG_INFO("XInputDebugApplication initialized");
}

void XInputDebugApplication::Update(float deltaTime)
{
    if (mTouchListener)
    {
        mTouchListener->PollEvents();
    }

    CoreApplication::Update(deltaTime);
}

void XInputDebugApplication::Shutdown()
{
    mTouchListener.reset();

    CoreApplication::Shutdown();
}

} // namespace application
