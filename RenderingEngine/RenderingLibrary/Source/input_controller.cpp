#include "input_controller.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <variant>
#include <unordered_map>
#include "input_key_glfw.hpp"

namespace
{
    std::unordered_map<GLFWwindow*, rendering_engine::InputController*>
        gInputControllers;
}

namespace rendering_engine
{
InputController::InputController(void* nativeWindow)
{
    mWindow = static_cast<GLFWwindow*>(nativeWindow);

    if (!mWindow)
    {
        throw std::invalid_argument("Invalid GLFW window.");
    }

    gInputControllers[mWindow] = this;

    glfwSetKeyCallback(mWindow, &InputController::KeyCallback);
}

InputController::~InputController()
{
    if (mWindow)
    {
        glfwSetKeyCallback(mWindow, nullptr);
        gInputControllers.erase(mWindow);
    }
}

void InputController::BeginFrame()
{
    mEvents.clear();
    mJustPressedKeys.clear();
    mJustReleasedKeys.clear();
}


void InputController::OnKeyEvent(const KeyEvent& event)
{
    switch (event.action)
    {
    case InputAction::Press:
        if (mPressedKeys.insert(event.key).second)
        {
            mJustPressedKeys.insert(event.key);
        }
        break;

    case InputAction::Release:
        if (mPressedKeys.erase(event.key) > 0)
        {
            mJustReleasedKeys.insert(event.key);
        }
        break;

    case InputAction::Repeat:
        // Key remains down; repeat is not a new press.
        break;
    }

    mEvents.emplace_back(event);
}

void InputController::OnMouseButtonEvent(const MouseButtonEvent& event)
{
    switch (event.action)
    {
    case InputAction::Press:
        mPressedMouseButtons.insert(event.button);
        break;

    case InputAction::Release:
        mPressedMouseButtons.erase(event.button);
        break;

    case InputAction::Repeat:
        // Not normally generated for mouse buttons.
        break;
    }

    mEvents.emplace_back(event);
}

void InputController::OnMouseMoveEvent(const MouseMoveEvent& event)
{
    mMouseX = event.x;
    mMouseY = event.y;

    mEvents.emplace_back(event);
}

bool InputController::IsKeyDown(Key key) const
{
    return mPressedKeys.find(key) != mPressedKeys.end();
}

bool InputController::IsKeyPressed(Key key) const
{
    return mJustPressedKeys.find(key) != mJustPressedKeys.end();
}

bool InputController::IsKeyReleased(Key key) const
{
    return mJustReleasedKeys.find(key) != mJustReleasedKeys.end();
}

bool InputController::IsMouseButtonDown(MouseButton button) const
{
    return mPressedMouseButtons.find(button)
        != mPressedMouseButtons.end();
}

double InputController::GetMouseX() const
{
    return mMouseX;
}

double InputController::GetMouseY() const
{
    return mMouseY;
}

const std::vector<InputEvent>& InputController::GetEvents() const
{
    return mEvents;
}

using namespace glfw_backend;

InputAction TranslateAction(int glfwAction)
{
    using rendering_engine::InputAction;

    switch (glfwAction)
    {
    case GLFW_PRESS: return InputAction::Press;
    case GLFW_RELEASE: return InputAction::Release;
    case GLFW_REPEAT: return InputAction::Repeat;
    default: return InputAction::Release;
    }
}

void InputController::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    const auto it = gInputControllers.find(window);

    if (it == gInputControllers.end())
    {
        return;
    }

    if (key == GLFW_KEY_UNKNOWN)
    {
        return;
    }

    const Key translatedKey = glfw_backend::TranslateKey(key);

    if (translatedKey == Key::Unknown)
    {
        return;
    }

    it->second->OnKeyEvent({
        translatedKey,
        TranslateAction(action)
        });
}



} // namespace rendering_engine
