// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "input_key.hpp"

#include <variant>
#include <glm/glm.hpp>
#include <cstdint>

namespace rendering_engine
{

enum class MouseButton
{
    Left,
    Right,
    Middle
};

enum class InputAction
{
    Press,
    Release,
    Repeat
};

struct KeyEvent
{
    Key key;
    InputAction action;
};

struct MouseButtonEvent
{
    MouseButton button;
    InputAction action;

    double x = 0.0;
    double y = 0.0;
};

struct MouseMoveEvent
{
    double x;
    double y;
};

struct MouseScrollEvent
{
    double xOffset;
    double yOffset;
};

using InputEvent = std::variant<
    KeyEvent,
    MouseButtonEvent,
    MouseMoveEvent,
    MouseScrollEvent
>;

/**
 * @brief Identifies the source of a pointer interaction.
 */
enum class PointerType
{
    Mouse,
    Touch
};

/**
 * @brief Describes the phase of a pointer interaction.
 */
enum class PointerAction
{
    Press,
    Move,
    Release,
    Cancel
};

/**
 * @brief Device-independent pointer event.
 *
 * Coordinates are expressed in screen space.
 * The pointer ID identifies an individual mouse or touch contact.
 */
struct PointerEvent2D
{
    PointerType type;
    std::uint64_t pointerId;
    PointerAction action;
    glm::vec2 screenPosition;
};

} // namespace rendering_engine