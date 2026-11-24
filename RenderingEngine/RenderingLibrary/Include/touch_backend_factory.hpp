// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "i_touch_backend.hpp"
#include "input_event.hpp"

#include <functional>
#include <memory>

namespace rendering_engine
{

struct TouchBackendContext
{
    // Opaque, platform-specific context.
    void* nativeWindow = nullptr;
    void* nativeApplication = nullptr;
};

using TouchEventCallback = std::function<void(const TouchEvent&)>;

class TouchBackendfactory
{
public:
    static std::unique_ptr<ITouchBackend> CreateTouchBackend(const TouchBackendContext& context, TouchEventCallback callback);

};

} // namespace rendering_engine