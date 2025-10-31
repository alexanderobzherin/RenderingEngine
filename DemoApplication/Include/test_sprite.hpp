#pragma once


#include "actor.hpp"

#include <memory>

using namespace rendering_engine;

namespace rendering_engine
{
class Scene;
class Camera2D;
class Quad2D;
}

namespace demo_application
{
class TestSprite : public Actor
{
public:
    TestSprite(Scene& scene);

    void Initialize() override;

    void Update(float deltaTime) override;

    void Draw(const Camera& camera) override;

private:
    std::unique_ptr<Quad2D> mQuad2D;
};
}