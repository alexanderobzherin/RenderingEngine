
#include "xinput2_debug_listener.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

#include "logger.hpp"

#include <stdexcept>
#include <string>
#include <iostream>

using namespace rendering_engine;

namespace application
{

XInput2DebugListener::XInput2DebugListener(std::uint64_t windowId)
{
    const Window xWindow = static_cast<Window>(windowId);

    if (xWindow == None)
    {
        throw std::invalid_argument("Invalid X11 window ID");
    }

    mDisplay = XOpenDisplay(nullptr);
    if (!mDisplay)
        throw std::runtime_error("Cannot open X11 display");

    try
    {
        int firstEvent = 0;
        int firstError = 0;

        if (!XQueryExtension(
                mDisplay,
                "XInputExtension",
                &mOpcode,
                &firstEvent,
                &firstError))
        {
            throw std::runtime_error("XInput extension unavailable");
        }

        int major = 2;
        int minor = 2;

        if (XIQueryVersion(mDisplay, &major, &minor) != Success ||
            major < 2 || (major == 2 && minor < 2))
        {
            throw std::runtime_error("XInput2.2 unavailable");
        }

        unsigned char mask[XIMaskLen(XI_TouchEnd)] = {};

        XISetMask(mask, XI_TouchBegin);
        XISetMask(mask, XI_TouchUpdate);
        XISetMask(mask, XI_TouchEnd);

        XISetMask(mask, XI_Motion);
        XISetMask(mask, XI_ButtonPress);
        XISetMask(mask, XI_ButtonRelease);

        XIEventMask eventMask{};
        eventMask.deviceid = XIAllMasterDevices;
        eventMask.mask_len = sizeof(mask);
        eventMask.mask = mask;

        if (XISelectEvents(mDisplay, xWindow, &eventMask, 1)
            != Success)
        {
            throw std::runtime_error("XISelectEvents failed");
        }

        XSync(mDisplay, False);

        LOG_INFO(
            "XInput2 initialized: version " +
            std::to_string(major) + "." +
            std::to_string(minor)
        );

        LOG_INFO(
            "XInput2 listening on X11 window " +
            std::to_string(static_cast<unsigned long>(xWindow))
        );
    }
    catch (...)
    {
        XCloseDisplay(mDisplay);
        mDisplay = nullptr;
        throw;
    }
}

XInput2DebugListener::~XInput2DebugListener()
{
    if (mDisplay)
        XCloseDisplay(mDisplay);
}

void XInput2DebugListener::PollEvents()
{
    while (XPending(mDisplay) > 0)
    {
        XEvent event{};
        XNextEvent(mDisplay, &event);

        if (event.type != GenericEvent ||
            event.xcookie.extension != mOpcode)
        {
            continue;
        }

        if (!XGetEventData(mDisplay, &event.xcookie))
            continue;

        const int type = event.xcookie.evtype;

        if (type == XI_Motion ||
            type == XI_ButtonPress ||
            type == XI_ButtonRelease)
        {
            const auto* pointer =
                static_cast<XIDeviceEvent*>(event.xcookie.data);

            if (type == XI_ButtonPress || type == XI_ButtonRelease)
            {
                LOG_INFO(
                    std::string("XInput2 mouse ") +
                    (type == XI_ButtonPress ? "PRESS" : "RELEASE") +
                    " Button=" + std::to_string(pointer->detail) +
                    " X=" + std::to_string(pointer->event_x) +
                    " Y=" + std::to_string(pointer->event_y) +
                    " Device=" + std::to_string(pointer->deviceid) +
                    " Source=" + std::to_string(pointer->sourceid)
                );
            }
        }

        if (type == XI_TouchBegin ||
            type == XI_TouchUpdate ||
            type == XI_TouchEnd)
        {
            const auto* touch =
                static_cast<XIDeviceEvent*>(event.xcookie.data);

            const char* phase =
                type == XI_TouchBegin  ? "BEGIN" :
                type == XI_TouchUpdate ? "MOVE"  :
                                        "END";

            LOG_INFO(
                std::string("Touch ") + phase +
                " ID=" + std::to_string(touch->detail) +
                " X=" + std::to_string(touch->event_x) +
                " Y=" + std::to_string(touch->event_y) +
                " RootX=" + std::to_string(touch->root_x) +
                " RootY=" + std::to_string(touch->root_y) +
                " Device=" + std::to_string(touch->deviceid) +
                " Source=" + std::to_string(touch->sourceid) +
                " Time=" + std::to_string(touch->time) +
                " Flags=" + std::to_string(touch->flags)
            );
        }

        XFreeEventData(mDisplay, &event.xcookie);
    }
}

} // namespace application
