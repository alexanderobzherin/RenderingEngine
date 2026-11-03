// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "input_key.hpp"

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
};

struct MouseMoveEvent
{
    double x;
    double y;
};

} // namespace rendering_engine