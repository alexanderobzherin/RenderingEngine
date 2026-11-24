#include "touch_backend_factory.hpp"
#include "xinput2_touch_backend.hpp"

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

#include <cstdint>
#include <memory>
#include <utility>

struct GLFWwindow;

namespace rendering_engine
{

std::unique_ptr<ITouchBackend> TouchBackendfactory::CreateTouchBackend(const TouchBackendContext& context, TouchEventCallback callback)
{
    auto* window = static_cast<GLFWwindow*>(context.nativeWindow);

    if (!window)
    {
        return nullptr;
    }

    const auto x11WindowId = static_cast<std::uint64_t>(glfwGetX11Window(window));
    return std::make_unique<XInput2TouchBackend>(x11WindowId, std::move(callback));
}

} // namespace rendering_engine
