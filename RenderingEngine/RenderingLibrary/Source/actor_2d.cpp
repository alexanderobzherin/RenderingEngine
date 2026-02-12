#include "actor_2d.hpp"
#include "scene.hpp"
#include "scene_manager.hpp"
#include "drawable_2d.hpp"

namespace rendering_engine
{
Actor2D::Actor2D(Scene& scene)
	:
	mScene(scene),
	bUpdateOnTick(true)
{
	mRenderContext = mScene.GetSceneManager().GetRenderResourceContext();
}

Actor2D::~Actor2D()
{
}

void Actor2D::Initialize()
{
}

void Actor2D::SetPosition(const glm::vec2& position)
{
	mRootComponent.SetPosition(position);
}

void Actor2D::SetRotation(float angleDegrees)
{
	mRootComponent.SetRotation(angleDegrees);
}

void Actor2D::SetScale(const glm::vec2& scale)
{
	mRootComponent.SetScale(scale);
}

const glm::vec2& Actor2D::GetPosition() const
{
	return mRootComponent.GetPosition();
}

float Actor2D::GetRotation() const
{
	return mRootComponent.GetRotation();
}

const glm::vec2& Actor2D::GetScale() const
{
	return mRootComponent.GetScale();
}

SceneComponent2D& Actor2D::GetTransform()
{
	return mRootComponent;
}

const SceneComponent2D& Actor2D::GetTransform() const
{
	return mRootComponent;
}

void Actor2D::Update(float deltaTime)
{
	if (!bUpdateOnTick)
		return;

	mRootComponent.UpdateWorldMatrix();
}

RenderResourceContext Actor2D::GetRenderContext() const
{
	return mRenderContext;
}

void Actor2D::Destroy()
{
	if (bPendingDestroy)
		return;

	bPendingDestroy = true;

	for (auto* ward : mWards)
		if (ward) ward->Destroy();

	mScene.DestroyActor(this);
}

void Actor2D::Shutdown()
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
