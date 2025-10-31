#include "camera.hpp"
#include "i_application.hpp"
#include "scene_component.hpp"

namespace rendering_engine
{

const float Camera::sDefaultFieldOfView = 45.0f;
const float Camera::sDefaultNearPlaneDistance = 0.01f;
const float Camera::sDefaultFarPlaneDistance = 1000.0f;

Camera::Camera(IApplication& app)
	:
	mApp(app),
	mSceneComponent(std::make_unique<SceneComponent>()),
	mFieldOfView(sDefaultFieldOfView),
	mNearPlaneDistance(sDefaultNearPlaneDistance),
	mFarPlaneDistance(sDefaultFarPlaneDistance)
{
	mAspectRatio = static_cast<float>(mApp.GetScreenSettings().width) /
		static_cast<float>(mApp.GetScreenSettings().height);
}

void Camera::Initialize()
{
	UpdateProjectionMatrix();
	Reset();
	UpdateViewMatrix();
}
void Camera::Update(float deltaTime)
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();
}
void Camera::SetPosition(const glm::vec3& position)
{
	mSceneComponent->SetPosition(position);
}
void Camera::SetRotation(const glm::quat& rotation)
{
	mSceneComponent->SetRotation(rotation);
}
void Camera::SetRotation(const glm::vec3& rotation)
{
	mSceneComponent->SetRotation(rotation);
}
glm::vec3 Camera::GetPosition() const
{
	return mSceneComponent->GetPosition();
}
glm::quat Camera::GetRotationQuat() const
{
	return mSceneComponent->GetRotationQuat();
}
glm::vec3 Camera::GetRotation() const
{
	return mSceneComponent->GetRotation();
}
glm::vec3 Camera::GetForward() const
{
	return mSceneComponent->GetForward();
}
glm::vec3 Camera::GetUp() const
{
	return mSceneComponent->GetUp();
}
glm::vec3 Camera::GetRight() const
{
	return mSceneComponent->GetRight();
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
	// Turn leff, as by default it will look to the right
	SetPosition(glm::vec3(-10.0f, 0.0f, 0.0f));
	SetRotation(glm::vec3(0.0f, 0.0f, 0.0f)); 
}
void Camera::UpdateProjectionMatrix()
{
	mAspectRatio = static_cast<float>(mApp.GetScreenSettings().width) /
		static_cast<float>(mApp.GetScreenSettings().height);
	mProjectionMatrix = glm::perspective(glm::radians(mFieldOfView), mAspectRatio, mNearPlaneDistance, mFarPlaneDistance);
}
void Camera::UpdateViewMatrix()
{
	const glm::vec3 target = mSceneComponent->GetPosition() + mSceneComponent->GetForward();
	mViewMatrix = glm::lookAt(mSceneComponent->GetPosition(), target, mSceneComponent->GetUp());
}
} //rendering_engine