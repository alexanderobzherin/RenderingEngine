#include "drawable_2d.hpp"
#include "i_render_resources.hpp"
#include "scene.hpp"

namespace rendering_engine
{
Drawable2D::Drawable2D(RenderResourceContext renderContext, Scene& scene)
	:
	DrawableComponent(renderContext, scene)
{}

void Drawable2D::Initialize()
{
	DrawableComponent::Initialize();
}

void Drawable2D::Update(float deltaTime)
{
	mSceneComponent.UpdateModelMatrix();
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

} // namespace rendering_engine