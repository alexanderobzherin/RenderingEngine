// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <memory>

using namespace rendering_engine;

namespace rendering_engine
{

}

namespace application
{
class XInput2DebugListener
{
public:
    explicit XInput2DebugListener(GLFWwindow* window);
    ~XInput2DebugListener();

    XInput2DebugListener(const XInput2DebugListener&) = delete;
    XInput2DebugListener& operator=(const XInput2DebugListener&) = delete;

    void PollEvents();
};

};