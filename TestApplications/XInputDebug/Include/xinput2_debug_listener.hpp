// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <memory>
#include <cstdint>

struct GLFWwindow;
struct _XDisplay;
using Display = _XDisplay;

namespace application
{
class XInput2DebugListener
{
public:
    explicit XInput2DebugListener(std::uint64_t windowId);
    ~XInput2DebugListener();

    XInput2DebugListener(const XInput2DebugListener&) = delete;
    XInput2DebugListener& operator=(const XInput2DebugListener&) = delete;

    void PollEvents();

private:
    Display* mDisplay = nullptr;
    int mOpcode = 0;
};

} // namespace application