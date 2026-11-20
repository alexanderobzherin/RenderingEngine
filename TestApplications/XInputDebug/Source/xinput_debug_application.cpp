#include "xinput_debug_application.hpp"

using namespace rendering_engine;

namespace application
{
    XInputDebugApplication::XInputDebugApplication()
    {
    }
    XInputDebugApplication::~XInputDebugApplication()
    {
    }
    void XInputDebugApplication::Initialize()
{
    CoreApplication::Initialize();

    mTouchListener =
        std::make_unique<XInput2DebugListener>();

    // Obtain the GLFW native handle through
    // the existing window system.

    // Initialize the XInput2 listener.
}

void XInputDebugApplication::Update(float deltaTime)
{
    // Poll native touch events.

    // Log touch information.

    CoreApplication::Update(deltaTime);
}

void XInputDebugApplication::Shutdown()
{
    // Destroy the listener before GLFW shuts down.
    mTouchListener.reset();

    CoreApplication::Shutdown();
}

}