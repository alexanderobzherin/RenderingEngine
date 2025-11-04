#include "drawable_3d.hpp"
#include "i_render_resources.hpp"

namespace rendering_engine
{
Drawable3D::Drawable3D(RenderResourceContext renderContext):
	DrawableComponent(renderContext)
{}

void Drawable3D::Initialize()
{
	DrawableComponent::Initialize();
}

void Drawable3D::Update(float deltaTime)
{
	mSceneComponent.UpdateWorldMatrix();
}

void Drawable3D::SetPosition(const glm::vec3& position)
{
	mSceneComponent.SetPosition(position);
}

void Drawable3D::SetRotation(const glm::vec3& rotation)
{
	mSceneComponent.SetRotation(rotation);
}

void Drawable3D::SetScale(const glm::vec3& scale)
{
	mSceneComponent.SetScale(scale);
}

const glm::vec3& Drawable3D::GetPosition() const
{
	return mSceneComponent.GetPosition();
}

const glm::vec3& Drawable3D::GetRotation() const
{
	return mSceneComponent.GetRotation();
}

const glm::vec3& Drawable3D::GetScale() const
{
	return mSceneComponent.GetScale();
}

SceneComponent& Drawable3D::GetTransform()
{
	return mSceneComponent;
}

const SceneComponent& Drawable3D::GetTransform() const
{
	return mSceneComponent;
}

} // namespace rendering_engine
