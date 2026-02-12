#include "camera_2d.hpp"
#include "scene_component_2d.hpp"
#include "i_application.hpp"

namespace rendering_engine
{

Camera2D::Camera2D(IApplication& app)
	:
	mApp(app),
	mSceneComponent(std::make_unique<SceneComponent2D>()),
	mViewportSize(glm::vec2(0.0f, 0.0f))
{}

void Camera2D::Initialize()
{
	Reset();

}

void Camera2D::Update(float deltaTime)
{
}

void Camera2D::SetViewportSize(float width, float height)
{
	mViewportSize = { width, height };
}

glm::mat4 Camera2D::GetProjectionMatrix() const
{
	float halfWidth = mViewportSize.x * 0.5f;
	float halfHeight = mViewportSize.y * 0.5f;

	float left = -halfWidth;
	float right = halfWidth;
	float bottom = -halfHeight;
	float top = halfHeight;
	float near = -1.0f;
	float far = 1.0f;

	return glm::ortho(left, right, bottom, top, near, far);
}

void Camera2D::SetPosition(const glm::vec2& position)
{
	mSceneComponent->SetPosition(position);
}

void Camera2D::SetRotation(float angleDegrees)
{
	mSceneComponent->SetRotation(angleDegrees);
}

void Camera2D::SetZoom(float zoom)
{
	mSceneComponent->SetScale(glm::vec2(zoom, zoom));
}

void Camera2D::Reset()
{
	const glm::vec2 startPos = glm::vec2(0.0f, 0.0f);
	const float startRot = 0.0f;
	const float startZoom = 1.0f;
	mSceneComponent->SetPosition(startPos);
	mSceneComponent->SetRotation(startRot);
	mSceneComponent->SetScale(glm::vec2(startZoom, startZoom));

	SetViewportSize(mApp.GetScreenSettings().width, mApp.GetScreenSettings().height);
}

glm::vec2 Camera2D::GetPosition() const
{
	return mSceneComponent->GetPosition();
}

float Camera2D::GetRotation() const
{
	return mSceneComponent->GetRotation();
}

float Camera2D::GetZoom() const
{
	return mSceneComponent->GetScale().x;
}

const glm::mat4& Camera2D::GetWorldView() const
{
	return mSceneComponent->GetWorldMatrix();
}

glm::mat4 Camera2D::ComputeOrthographicMatrix(float left, float right, float top, float bottom, float nearPlane, float farPlane) const
{
	glm::mat4 result(1.0f);

	result[0][0] = 2.0f / (right - left);
	result[1][1] = 2.0f / (bottom - top);
	result[2][2] = 1.0f / (farPlane - nearPlane);

	result[3][0] = -(right + left) / (right - left);
	result[3][1] = -(bottom + top) / (bottom - top);
	result[3][2] = -nearPlane / (farPlane - nearPlane);

	return result;
}

} // namespace rendering_engine