#include "camera.hpp"

namespace rendering_engine
{

const float Camera::sDefaultFieldOfView = 45.0f;
const float Camera::sDefaultNearPlaneDistance = 0.01f;
const float Camera::sDefaultFarPlaneDistance = 1000.0f;

Camera::Camera(RendererBase* renderer)
	:
	mRenderer(renderer),
	mFieldOfView(sDefaultFieldOfView),
	mAspectRatio(mRenderer->GetAspectRation()),
	mNearPlaneDistance(sDefaultNearPlaneDistance),
	mFarPlaneDistance(sDefaultFarPlaneDistance)
{
		
}
Camera::~Camera()
{
}
void Camera::Initialize()
{
	UpdateProjectionMatrix();
	Reset();
	UpdateViewMatrix();
}
void Camera::Update(float const delta)
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}
const glm::mat4& Camera::ViewMatrix() const
{
	return mViewMatrix;
}
const glm::mat4& Camera::ProjectionMatrix() const
{
	return mProjectionMatrix;
}
glm::mat4 Camera::ViewProjectionMatrix() const
{
	return mProjectionMatrix * mViewMatrix;
}
void Camera::Reset()
{
	mPosition = glm::vec3(0.0f, 0.0f, 10.0f); // Zero
	mDirection = glm::vec3(0.0f, 0.0f, -1.0f); // Forward
	mUp = glm::vec3(0.0f, 1.0f, 0.0f); // Up
	mRight = glm::vec3(1.0f, 0.0f, 0.0f); // Right
}
void Camera::UpdateProjectionMatrix()
{
	mProjectionMatrix = glm::perspective(glm::radians(mFieldOfView), mRenderer->GetAspectRation(), mNearPlaneDistance, mFarPlaneDistance);
}
void Camera::UpdateViewMatrix()
{
	glm::vec3 target = mPosition + mDirection;
	mViewMatrix = glm::lookAt(mPosition, target, mUp);
}
} //rendering_engine