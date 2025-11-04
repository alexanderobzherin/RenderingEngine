#include "actor.hpp"
#include "scene.hpp"
#include "scene_manager.hpp"

namespace rendering_engine
{

Actor::Actor(Scene& scene)
	:
	mScene(scene)
{
	mRenderContext = mScene.GetSceneManager().GetRenderResourceContext();
}

void Actor::Initialize()
{
}

void Actor::Update(float deltaTime)
{
}

void Actor::Draw(const Camera& camera)
{
}

RenderResourceContext Actor::GetRenderContext() const
{
	return mRenderContext;
}

} // namespace rendering_engine
