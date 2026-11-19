# Input Controller Guide

## Overview

The Input Controller provides access to keyboard and mouse input in Rendering Engine applications.

It allows applications to query the current state of input devices, detect press and release events, and respond to mouse interactions with 2D objects.

## 1. Accessing the Input Controller

`InputController` can be accessed directly from a scene:

```cpp
const auto* input = GetInputController();
```

From an actor, access it through the owning scene:

```cpp
const auto* input = GetScene().GetInputController();
```

Input states are normally queried during the application's update cycle.

**API:** [input_controller.hpp](../RenderingEngine/RenderingLibrary/Include/input_controller.hpp)

## 2. Keyboard Input

The controller provides three methods for querying individual key states:

| Method | Description |
|---|---|
| `IsKeyPressed(Key)` | True if the key was pressed during the current frame |
| `IsKeyDown(Key)` | True while the key is held down |
| `IsKeyReleased(Key)` | True if the key was released during the current frame |

Example:

```cpp
const auto* input = GetScene().GetInputController();

if (input->IsKeyPressed(Key::A))
{
    // Key A was pressed this frame
}

if (input->IsKeyDown(Key::A))
{
    // Key A is currently held down
}

if (input->IsKeyReleased(Key::A))
{
    // Key A was released this frame
}
```

The `Key` enumeration defines identifiers for supported keyboard keys.

**Example:** [PCKeyboardKeyActor::Update()](../UserApplications/InputKeyboardMouse/Source/pc_keyboard_key_actor.cpp)

## 3. Mouse Input

### Mouse Buttons

Mouse buttons can be queried similarly to keyboard keys:

```cpp
const auto* input = GetScene().GetInputController();

if (input->IsMouseButtonPressed(MouseButton::Left))
{
    // Left button pressed
}

if (input->IsMouseButtonReleased(MouseButton::Left))
{
    // Left button released
}

if (input->IsMouseButtonDown(MouseButton::Right))
{
    // Right button is held down
}
```

### Cursor Position

Read the current cursor position:

```cpp
const double x = input->GetMouseX();
const double y = input->GetMouseY();
```

Coordinates are relative to the application window, with the origin at its top-left corner.

### Mouse Wheel

Read the scroll offsets accumulated during the current frame:

```cpp
const double scrollX = input->GetMouseScrollX();
const double scrollY = input->GetMouseScrollY();
```

Positive and negative vertical offsets indicate scrolling direction.

**Example:** [PCMouseActor::Update()](../UserApplications/InputKeyboardMouse/Source/pcmouse_actor.cpp)

## 4. Interactive 2D Objects

Rendering Engine supports mouse interaction with 2D objects through physical geometry and `InteractionRouter2D`.

This can be used to implement interactive buttons, control panels, and other graphical interface elements.

### Enable Physical Geometry

Create a `Rectangle2D` with physical geometry enabled:

```cpp
Rectangle2D::Properties properties;

properties.size = {50.0f, 50.0f};
properties.physical.collisionEnabled = true;
properties.physical.geometryType =
    PhysicalGeometryType::Rectangle;
properties.physical.mobility = Mobility::Static;

auto* rectangle = CreateSubobject<Rectangle2D>(properties);
```

Retrieve the rectangle's physical geometry:

```cpp
auto* geometry = rectangle->GetPhysicalGeometry();
```

### Register Interaction Callbacks

Register the geometry with the scene's interaction router:

```cpp
auto onPressed = [this]()
{
    OnButtonPressed();
};

auto onReleased = [this](bool releasedInside)
{
    OnButtonReleased();
};

auto onCancelled = [this]()
{
    OnButtonReleased();
};

GetScene().GetInteractionRouter2D().RegisterTarget(
    geometry,
    {onPressed, onReleased, onCancelled}
);
```

The callbacks are invoked when the mouse interacts with the registered geometry:

| Callback | Description |
|---|---|
| `onPressed` | Left mouse button pressed inside the target |
| `onReleased` | Mouse button released; `releasedInside` indicates whether the cursor remains inside the target |
| `onCancelled` | Active interaction cancelled, for example when the target is unregistered |

The interaction router handles coordinate conversion and hit-testing automatically, including when the application window is resized.

**Example:** [PCKeyboardKeyActor::SetButtonShape()](../UserApplications/InputKeyboardMouse/Source/pc_keyboard_key_actor.cpp)

## 5. Input Events

Besides querying input states, applications can access the events collected during the current frame:

```cpp
const auto* input = GetScene().GetInputController();

for (const auto& event : input->GetEvents())
{
    if (const auto* keyEvent = std::get_if<KeyEvent>(&event))
    {
        // Process keyboard event
    }
    else if (const auto* mouseEvent =
                 std::get_if<MouseButtonEvent>(&event))
    {
        // Process mouse button event
    }
}
```

Input events include keyboard actions, mouse button actions, cursor movement, and scrolling.

Events are available during the current update frame. Use state queries for ordinary key and button handling, and the event collection when individual input events are needed.

**API:** [input_event.hpp](../RenderingEngine/RenderingLibrary/Include/input_event.hpp)

## 6. Example Application

The [InputKeyboardMouse example](../UserApplications/InputKeyboardMouse/README.md) demonstrates the input API in a complete graphical application.

Its main components are:

- [PCKeyboardActor](../UserApplications/InputKeyboardMouse/Source/pckeyboard_actor.cpp) — creates the virtual keyboard layout.
- [PCKeyboardKeyActor](../UserApplications/InputKeyboardMouse/Source/pc_keyboard_key_actor.cpp) — responds to physical key presses and mouse interactions.
- [PCMouseActor](../UserApplications/InputKeyboardMouse/Source/pcmouse_actor.cpp) — visualizes mouse buttons, cursor position, and scrolling.
- [StartLevel](../UserApplications/InputKeyboardMouse/Source/start_level.cpp) — creates and arranges the actors within the scene.

## Related Documentation

- [Application Development Guide](application_development_guide.md)
- [InputKeyboardMouse Example](../UserApplications/InputKeyboardMouse/README.md)