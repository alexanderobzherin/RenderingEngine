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
    if (bPendingDestroy)
        return;

    bPendingDestroy = true;

    for (auto* ward : mWards)
        if (ward) ward->Destroy();

	mScene.DestroyActor(this);
}
	
void Actor::Shutdown()
{
    // Detach wards from this actor to avoid dangling parent pointers
    for (auto* ward : mWards)
    {
        if (ward)
            ward->GetTransform().AttachTo(nullptr);
    }
    mWards.clear();

    mRootComponent.AttachTo(nullptr);
}



} // namespace rendering_engine
