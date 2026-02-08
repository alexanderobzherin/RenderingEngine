#include "actor.hpp"
#include "scene.hpp"
#include "scene_manager.hpp"
#include "spawn_drawables.hpp"
#include "static_mesh.hpp"

namespace rendering_engine
{

Actor::Actor(Scene& scene)
	:
	mScene(scene),
	bUpdateOnTick(true)
{
	mRenderContext = mScene.GetSceneManager().GetRenderResourceContext();
}

Actor::~Actor()
{
}

void Actor::Initialize()
{
}

void Actor::SetPosition(const glm::vec3& position)
{
	mRootComponent.SetPosition(position);
}

void Actor::SetRotation(const glm::vec3& rotation)
{
	mRootComponent.SetRotation(rotation);

}

void Actor::SetScale(const glm::vec3& scale)
{
	mRootComponent.SetScale(scale);
}

const glm::vec3& Actor::GetPosition() const
{
	return mRootComponent.GetPosition();
}

const glm::vec3& Actor::GetRotation() const
{
	return mRootComponent.GetRotation();
}

const glm::vec3& Actor::GetScale() const
{
	return mRootComponent.GetScale();
}

SceneComponent& Actor::GetTransform()
{
	return mRootComponent;
}

const SceneComponent& Actor::GetTransform() const
{
	return mRootComponent;
}

void Actor::Update(float deltaTime)
{
	mRootComponent.UpdateWorldMatrix();
}

RenderResourceContext Actor::GetRenderContext() const
{
	return mRenderContext;
}

void Actor::Destroy()
{
	for (auto& ward : mWards)
	{
		ward->Destroy();
	}
	mScene.DestroyActor(this);
}

void Actor::Shutdown()
{

}



} // namespace rendering_engine
