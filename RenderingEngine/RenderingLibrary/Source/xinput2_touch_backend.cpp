
#include "xinput2_touch_backend.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

#include <stdexcept>
#include <utility>

namespace rendering_engine
{

XInput2TouchBackend::XInput2TouchBackend(std::uint64_t x11WindowId, TouchEventCallback callback)
    : 
    mCallback(std::move(callback))
{
    const Window xWindow = static_cast<Window>(x11WindowId);

    if (xWindow == None)
    {
        throw std::invalid_argument("XInput2TouchBackend: invalid X11 window ID");
    }

    if (!mCallback)
    {
        throw std::invalid_argument("XInput2TouchBackend: touch callback is empty");
    }

    mDisplay = XOpenDisplay(nullptr);

    if (!mDisplay)
    {
        throw std::runtime_error("XInput2TouchBackend: cannot open X11 display");
    }

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
            throw std::runtime_error("XInput2TouchBackend: XInput extension unavailable");
        }

        int major = 2;
        int minor = 2;

        if (XIQueryVersion(mDisplay, &major, &minor) != Success ||
            major < 2 ||
            (major == 2 && minor < 2))
        {
            throw std::runtime_error("XInput2TouchBackend: XInput2.2 unavailable");
        }

        unsigned char mask[XIMaskLen(XI_TouchEnd)] = {};

        XISetMask(mask, XI_TouchBegin);
        XISetMask(mask, XI_TouchUpdate);
        XISetMask(mask, XI_TouchEnd);

        XIEventMask eventMask{};
        eventMask.deviceid = XIAllMasterDevices;
        eventMask.mask_len = sizeof(mask);
        eventMask.mask = mask;

        if (XISelectEvents(
                mDisplay,
                xWindow,
                &eventMask,
                1) != Success)
        {
            throw std::runtime_error("XInput2TouchBackend: XISelectEvents failed");
        }

        XSync(mDisplay, False);
    }
    catch (...)
    {
        XCloseDisplay(mDisplay);
        mDisplay = nullptr;
        throw;
    }
}

XInput2TouchBackend::~XInput2TouchBackend()
{
    if (mDisplay)
    {
        XCloseDisplay(mDisplay);
        mDisplay = nullptr;
    }
}

void XInput2TouchBackend::PollEvents()
{
    if (!mDisplay)
    {
        return;
    }

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
        {
            continue;
        }

        try
        {
            TouchPhase phase;

            switch (event.xcookie.evtype)
            {
            case XI_TouchBegin:
                phase = TouchPhase::Started;
                break;

            case XI_TouchUpdate:
                phase = TouchPhase::Moved;
                break;

            case XI_TouchEnd:
                phase = TouchPhase::Ended;
                break;

            default:
                XFreeEventData(mDisplay, &event.xcookie);
                continue;
            }

            const auto* nativeEvent = static_cast<const XIDeviceEvent*>(event.xcookie.data);

            TouchEvent touchEvent{
                static_cast<TouchId>(nativeEvent->detail),
                phase,
                glm::vec2{
                    static_cast<float>(nativeEvent->event_x),
                    static_cast<float>(nativeEvent->event_y)
                }
            };

            mCallback(touchEvent);
        }
        catch (...)
        {
            XFreeEventData(mDisplay, &event.xcookie);
            throw;
        }

        XFreeEventData(mDisplay, &event.xcookie);
    }
}

} // namespace rendering_engine
