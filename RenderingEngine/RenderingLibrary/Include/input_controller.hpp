#pragma once

#include "rendering_engine_export.hpp"
#include "input_event.hpp"

#include <unordered_set>
#include <variant>
#include <vector>

struct GLFWwindow;

namespace rendering_engine
{
/**
 * @brief Provides access to input device states and events.
 *
 * Supports querying persistent input states, detecting
 * frame-specific transitions, and processing input events.
 *
 * Input states and events are updated during the application's
 * frame processing cycle.
 */
class RE_API InputController
{
public:
    /**
     * @brief Creates an input controller for a native window.
     * @param nativeWindow Native window handle used for input handling.
     */
    explicit InputController(void* nativeWindow);
    /**
     * @brief Destroys the input controller.
     */
    ~InputController();

    InputController(const InputController&) = delete;
    InputController& operator=(const InputController&) = delete;
    InputController(InputController&&) = delete;
    InputController& operator=(InputController&&) = delete;

    /**
     * @brief Begins a new input processing frame.
     *
     * Clears frame-specific input transitions, accumulated
     * scroll offsets, and the event collection while preserving
     * the current key and mouse button states.
     */
    void BeginFrame();
    /**
     * @brief Processes a keyboard input event.
     * @param event Keyboard event to process.
     */
    void OnKeyEvent(const KeyEvent& event);
    /**
     * @brief Processes a mouse button event.
     * @param event Mouse button event to process.
     */
    void OnMouseButtonEvent(const MouseButtonEvent& event);
    /**
     * @brief Processes a mouse movement event.
     * @param event Mouse movement event to process.
     */
    void OnMouseMoveEvent(const MouseMoveEvent& event);
    /**
     * @brief Processes a mouse scrolling event.
     * @param event Mouse scrolling event to process.
     */
    void OnMouseScrollEvent(const MouseScrollEvent& event);

    /**
     * @brief Checks whether a key is currently held down.
     * @param key Key to query.
     * @return True if the key is down.
     */
    bool IsKeyDown(Key key) const;
    /**
     * @brief Checks whether a key was pressed during the current frame.
     * @param key Key to query.
     * @return True if a press transition occurred this frame.
     */
    bool IsKeyPressed(Key key) const;
    /**
     * @brief Checks whether a key was released during the current frame.
     * @param key Key to query.
     * @return True if a release transition occurred this frame.
     */
    bool IsKeyReleased(Key key) const;
    /**
     * @brief Checks whether a mouse button is currently held down.
     * @param button Mouse button to query.
     * @return True if the button is down.
     */
    bool IsMouseButtonDown(MouseButton button) const;
    /**
     * @brief Checks whether a mouse button was pressed during the current frame.
     * @param button Mouse button to query.
     * @return True if a press transition occurred this frame.
     */
    bool IsMouseButtonPressed(MouseButton button) const;
    /**
     * @brief Checks whether a mouse button was released during the current frame.
     * @param button Mouse button to query.
     * @return True if a release transition occurred this frame.
     */
    bool IsMouseButtonReleased(MouseButton button) const;
    /**
     * @brief Returns the horizontal mouse scroll offset accumulated this frame.
     * @return Horizontal scroll offset.
     */
    double GetMouseScrollX() const;
    /**
     * @brief Returns the vertical mouse scroll offset accumulated this frame.
     * @return Vertical scroll offset.
     */
    double GetMouseScrollY() const;
    /**
     * @brief Returns the current cursor X coordinate in window space.
     * @return Horizontal cursor position.
     */
    double GetMouseX() const;
    /**
     * @brief Returns the current cursor Y coordinate in window space.
     * @return Vertical cursor position.
     */
    double GetMouseY() const;
    /**
     * @brief Returns input events collected during the current frame.
     *
     * The collection is cleared at the beginning of the next
     * input processing frame.
     *
     * @return Read-only reference to the current frame's events.
     */
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
	