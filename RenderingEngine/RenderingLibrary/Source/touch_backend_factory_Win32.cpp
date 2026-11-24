#include "touch_backend_factory.hpp"

struct GLFWwindow;

namespace rendering_engine
{

std::unique_ptr<ITouchBackend> TouchBackendfactory::CreateTouchBackend(const TouchBackendContext& context, TouchEventCallback callback)
{
    return nullptr;
}

} // namespace rendering_engine
