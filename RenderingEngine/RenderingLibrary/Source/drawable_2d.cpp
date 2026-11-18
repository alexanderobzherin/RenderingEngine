#include "drawable_2d.hpp"
#include "i_render_resources.hpp"
#include "scene.hpp"
#include "physical_geometry_2d.hpp"
#include "collision_world_2d.hpp"

#include <stdexcept>
#include <utility>

namespace rendering_engine
{
Drawable2D::Drawable2D(RenderResourceContext renderContext, Scene& scene)
	:
	DrawableComponent(renderContext, scene)
{}

Drawable2D::~Drawable2D()
{
	// Also covers destruction paths that bypass Shutdown().
	// The owning Scene must still be alive at this point.
	if (mPhysicalGeometry)
	{
		mScene.GetCollisionWorld2D().Unregister(mPhysicalGeometry.get());
	}
}

void Drawable2D::Initialize()
{
	DrawableComponent::Initialize();
}

void Drawable2D::Update(float)
{
	if (bUpdateOnTick)
	{
		mSceneComponent.UpdateWorldMatrix();
	}
}

void Drawable2D::Shutdown()
{
	// Unregister before destroying the physical geometry.
	SetPhysicalGeometry(nullptr);

	DrawableComponent::Shutdown();
}

void Drawable2D::SetPosition(const glm::vec2& position)
{
	mSceneComponent.SetPosition(position);
}

void Drawable2D::SetRotation(float angleDegrees)
{
	mSceneComponent.SetRotation(angleDegrees);
}

void Drawable2D::SetScale(const glm::vec2& scale)
{
	mSceneComponent.SetScale(scale);
}

const glm::vec2& Drawable2D::GetPosition() const
{
	return mSceneComponent.GetPosition();
}

float Drawable2D::GetRotation() const
{
	return mSceneComponent.GetRotation();
}

const glm::vec2& Drawable2D::GetScale() const
{
	return mSceneComponent.GetScale();
}

SceneComponent2D& Drawable2D::GetTransform()
{
	return mSceneComponent;
}

const SceneComponent2D& Drawable2D::GetTransform() const
{
	return mSceneComponent;
}

void Drawable2D::Destroy()
{
	mScene.DestroyDrawable2D(this);
}

void Drawable2D::SetPhysicalGeometry(std::unique_ptr<PhysicalGeometry2D> geometry)
{
	// Validate before changing the existing geometry.
	if (geometry && &geometry->GetTransform() != &mSceneComponent)
	{
		throw std::invalid_argument(
			"Physical geometry must reference "
			"the owning Drawable2D transform.");
	}

	CollisionWorld2D& collisionWorld = mScene.GetCollisionWorld2D();

	if (mPhysicalGeometry)
	{
		auto* oldGeometry = mPhysicalGeometry.get();

		mScene.GetInteractionRouter2D().UnregisterTarget(oldGeometry);
		collisionWorld.Unregister(oldGeometry);
	}

	// Destroys the previous geometry, if present.
	mPhysicalGeometry = std::move(geometry);

	if (mPhysicalGeometry)
	{
		collisionWorld.Register(mPhysicalGeometry.get());
	}
}

PhysicalGeometry2D* Drawable2D::GetPhysicalGeometry()
{
	return mPhysicalGeometry.get();
}

const PhysicalGeometry2D* Drawable2D::GetPhysicalGeometry() const
{
	return mPhysicalGeometry.get();
}

} // namespace rendering_engine