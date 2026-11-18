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

    glfwSetMouseButtonCallback(mWindow, &InputController::MouseButtonCallback);

    glfwSetCursorPosCallback(mWindow, &InputController::CursorPositionCallback);

    glfwSetScrollCallback(mWindow, &InputController::ScrollCallback);

    glfwGetCursorPos(mWindow, &mMouseX, &mMouseY);
}

InputController::~InputController()
{
    if (mWindow)
    {
        glfwSetKeyCallback(mWindow, nullptr);
        glfwSetMouseButtonCallback(mWindow, nullptr);
        glfwSetCursorPosCallback(mWindow, nullptr);
        glfwSetScrollCallback(mWindow, nullptr);

        gInputControllers.erase(mWindow);
    }
}

void InputController::BeginFrame()
{
    mEvents.clear();
    mJustPressedKeys.clear();
    mJustReleasedKeys.clear();

    mJustPressedMouseButtons.clear();
    mJustReleasedMouseButtons.clear();

    mMouseScrollX = 0.0;
    mMouseScrollY = 0.0;
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
        if (mPressedMouseButtons.insert(event.button).second)
        {
            mJustPressedMouseButtons.insert(event.button);
        }
        break;

    case InputAction::Release:
        if (mPressedMouseButtons.erase(event.button) > 0)
        {
            mJustReleasedMouseButtons.insert(event.button);
        }
        break;

    case InputAction::Repeat:
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

void InputController::OnMouseScrollEvent(const MouseScrollEvent& event)
{
    mMouseScrollX += event.xOffset;
    mMouseScrollY += event.yOffset;

    mEvents.emplace_back(event);
}

bool InputController::IsMouseButtonPressed(MouseButton button) const
{
    return mJustPressedMouseButtons.find(button)
        != mJustPressedMouseButtons.end();
}

bool InputController::IsMouseButtonReleased(MouseButton button) const
{
    return mJustReleasedMouseButtons.find(button)
        != mJustReleasedMouseButtons.end();
}

double InputController::GetMouseScrollX() const
{
    return mMouseScrollX;
}

double InputController::GetMouseScrollY() const
{
    return mMouseScrollY;
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

void InputController::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    (void)mods;

    const auto it = gInputControllers.find(window);

    if (it == gInputControllers.end())
    {
        return;
    }

    MouseButton mouseButton;

    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        mouseButton = MouseButton::Left;
        break;

    case GLFW_MOUSE_BUTTON_RIGHT:
        mouseButton = MouseButton::Right;
        break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
        mouseButton = MouseButton::Middle;
        break;

    default:
        return;
    }

    if (action != GLFW_PRESS && action != GLFW_RELEASE)
    {
        return;
    }

    double x = 0.0;
    double y = 0.0;

    glfwGetCursorPos(window, &x, &y);

    it->second->OnMouseButtonEvent({
        mouseButton,
        TranslateAction(action),
        x,
        y
        });
}

void InputController::CursorPositionCallback(GLFWwindow* window, double x, double y)
{
    const auto it = gInputControllers.find(window);

    if (it == gInputControllers.end())
    {
        return;
    }

    it->second->OnMouseMoveEvent({ x, y });
}

void InputController::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    const auto it = gInputControllers.find(window);

    if (it == gInputControllers.end())
    {
        return;
    }

    it->second->OnMouseScrollEvent({
        xOffset,
        yOffset
        });
}



} // namespace rendering_engine
