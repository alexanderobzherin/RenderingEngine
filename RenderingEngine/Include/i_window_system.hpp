#pragma once

#include "i_application.hpp"
#include <string>

namespace rendering_engine
{
struct WindowResolution
{
    unsigned int width;
    unsigned int height;
};
class IWindowSystem
{
public:
    virtual void CreateAppWindow(unsigned int width, unsigned int height, const std::string& title) = 0;
    virtual void PollEvents() = 0;
    virtual bool ShouldClose() const = 0;
    virtual void* GetNativeHandle() const = 0;

    virtual bool IsFramebufferResized() const = 0;
    virtual void ResetFramebufferResizedFlag() = 0;

    virtual void Shutdown() = 0;
    virtual const IApplication& GetApplication() = 0;

    virtual WindowResolution GetFullScreenResolution() const = 0;

    virtual ~IWindowSystem() = default;
};
} //rendering_engine