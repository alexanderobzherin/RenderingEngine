// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "core_application.hpp"

#include <memory>

using namespace rendering_engine;

namespace application
{
class XInput2DebugListener;

class XInputDebugApplication final
    : public rendering_engine::CoreApplication
{
public:
    XInputDebugApplication();
    ~XInputDebugApplication() override;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

private:
    std::unique_ptr<XInput2DebugListener> mTouchListener;
};
}