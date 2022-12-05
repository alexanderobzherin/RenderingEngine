#include "actor.hpp"
#include "scene_component.hpp"
#include "vulkan_drawable_component.hpp"

namespace rendering_engine
{
Actor::Actor(RendererBase* renderer)
    :
    mRenderer(renderer)
{
}
Actor::~Actor()
{
}
void Actor::Initialize()
{
}

void Actor::Update(float const delta)
{
}

void Actor::Draw()
{
}

void Actor::Shudtown()
{
}

} //rendering_engine