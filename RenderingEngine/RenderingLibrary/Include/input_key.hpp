// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

namespace rendering_engine
{

enum class Key
{
    Unknown = 0,

    // Letters
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // Main number row
    Num0, Num1, Num2, Num3, Num4,
    Num5, Num6, Num7, Num8, Num9,

    // Editing and control
    Escape,
    Enter,
    Tab,
    Backspace,
    Space,

    // Punctuation (US physical key positions)
    GraveAccent,
    Minus,
    Equal,
    LeftBracket,
    RightBracket,
    Backslash,
    Semicolon,
    Apostrophe,
    Comma,
    Period,
    Slash,
    World1,
    World2,

    // Function keys
    F1, F2, F3, F4, F5, F6,
    F7, F8, F9, F10, F11, F12,

    // Navigation
    Insert,
    Delete,
    Home,
    End,
    PageUp,
    PageDown,
    Up,
    Down,
    Left,
    Right,

    // Modifiers
    LeftShift,
    RightShift,
    LeftControl,
    RightControl,
    LeftAlt,
    RightAlt,
    LeftSuper,
    RightSuper,
    Menu,

    // Numeric keypad
    Keypad0, Keypad1, Keypad2, Keypad3, Keypad4,
    Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,
    KeypadDecimal,
    KeypadDivide,
    KeypadMultiply,
    KeypadSubtract,
    KeypadAdd,
    KeypadEnter,
    KeypadEqual,

    // System keys
    CapsLock,
    NumLock,
    ScrollLock,
    PrintScreen,
    Pause
};

} // namespace rendering_engine
