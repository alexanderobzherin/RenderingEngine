#pragma once

#include "rendering_engine_export.hpp"
#include "input_event.hpp"

#include <unordered_set>
#include <variant>
#include <vector>

struct GLFWwindow;

namespace rendering_engine
{

using InputEvent = std::variant<
    KeyEvent,
    MouseButtonEvent,
    MouseMoveEvent
>;

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
};

} // namespace rendering_engine
	