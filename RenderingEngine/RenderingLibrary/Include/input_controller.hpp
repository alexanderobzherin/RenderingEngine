#pragma once

#include "rendering_engine_export.hpp"
#include "input_event.hpp"

#include <unordered_set>
#include <variant>
#include <vector>

struct GLFWwindow;

namespace rendering_engine
{

class RE_API InputController
{
public:
    explicit InputController(void* nativeWindow);
    ~InputController();

    InputController(const InputController&) = delete;
    InputController& operator=(const InputController&) = delete;
    InputController(InputController&&) = delete;
    InputController& operator=(InputController&&) = delete;

    void BeginFrame();

    void OnKeyEvent(const KeyEvent& event);
    void OnMouseButtonEvent(const MouseButtonEvent& event);
    void OnMouseMoveEvent(const MouseMoveEvent& event);

    // Check if the key is down during current polling frame
    bool IsKeyDown(Key key) const;
    bool IsKeyPressed(Key key) const;
    bool IsKeyReleased(Key key) const;

    bool IsMouseButtonDown(MouseButton button) const;

    void OnMouseScrollEvent(const MouseScrollEvent& event);

    bool IsMouseButtonPressed(MouseButton button) const;
    bool IsMouseButtonReleased(MouseButton button) const;

    double GetMouseScrollX() const;
    double GetMouseScrollY() const;

    double GetMouseX() const;
    double GetMouseY() const;

    const std::vector<InputEvent>& GetEvents() const;

private:
    static void KeyCallback(
        GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods);

    static void MouseButtonCallback(
        GLFWwindow* window,
        int button,
        int action,
        int mods);

    static void CursorPositionCallback(
        GLFWwindow* window,
        double x,
        double y);

    static void ScrollCallback(
        GLFWwindow* window,
        double xOffset,
        double yOffset);

private:
    // Persistent state
    std::unordered_set<Key> mPressedKeys;

    // Transitions that occurred during this frame
    std::unordered_set<Key> mJustPressedKeys;
    std::unordered_set<Key> mJustReleasedKeys;

    std::unordered_set<MouseButton> mPressedMouseButtons;

    double mMouseX = 0.0;
    double mMouseY = 0.0;

    std::vector<InputEvent> mEvents;

    GLFWwindow* mWindow = nullptr;

    std::unordered_set<MouseButton> mJustPressedMouseButtons;
    std::unordered_set<MouseButton> mJustReleasedMouseButtons;

    double mMouseScrollX = 0.0;
    double mMouseScrollY = 0.0;
};

} // namespace rendering_engine
	