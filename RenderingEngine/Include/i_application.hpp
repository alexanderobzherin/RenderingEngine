#pragma once

#include "i_renderer.hpp"
#include <string>

namespace rendering_engine
{

struct ScreenSettings
{
    std::string name;
    bool isFullScreen;
    unsigned int width;
    unsigned int height;
};

class IApplication
{
public:
    virtual void Initialize() = 0;

    virtual void Run() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;
    virtual void Shutdown() = 0;

    virtual ~IApplication() = default;

    virtual ScreenSettings GetScreenSettings() const = 0;

};
} //rendering_engine