// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "input_event.hpp"
#include "i_touch_backend.hpp"

#include <cstdint>
#include <functional>

struct _XDisplay;

namespace rendering_engine
{

/**
 * @brief Linux/X11 touchscreen input backend using XInput2.
 *
 * Receives native XInput2 touch events and translates them
 * into platform-independent Rendering Engine TouchEvents.
 *
 * The backend owns a separate X11 Display connection.
 * The target X11 window must remain valid throughout
 * the backend's lifetime.
 *
 * Requires XInput2 version 2.2 or later.
 */
class XInput2TouchBackend final : public ITouchBackend
{
public:
    using TouchEventCallback = std::function<void(const TouchEvent&)>;

    explicit XInput2TouchBackend(std::uint64_t x11WindowId, TouchEventCallback callback);

    ~XInput2TouchBackend();

    XInput2TouchBackend(const XInput2TouchBackend&) = delete;
    XInput2TouchBackend& operator=(const XInput2TouchBackend&) = delete;

    XInput2TouchBackend(XInput2TouchBackend&&) = delete;
    XInput2TouchBackend& operator=(XInput2TouchBackend&&) = delete;

    /**
     * @brief Polls pending XInput2 events.
     *
     * For each received touch event, invokes the callback
     * supplied during construction.
     *
     * Call once per application input-processing frame.
     */
    void PollEvents() override;

private:
    // Opaque X11 Display pointer.
    // Keeps X11 headers out of this interface.
    ::_XDisplay* mDisplay = nullptr;

    int mOpcode = 0;

    TouchEventCallback mCallback;
};

} // namespace rendering_engine
