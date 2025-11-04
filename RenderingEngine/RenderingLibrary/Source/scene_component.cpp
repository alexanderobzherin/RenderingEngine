#include "scene_component.hpp"

namespace rendering_engine
{

SceneComponent::SceneComponent()
    :
    mPosition{ 0.0f },
    mRotation{ 1.0f, 0.0f, 0.0f, 0.0f },
    mEulerRotation{ 0.0f },
    mScale{ 1.0f, 1.0f, 1.0f },
    mWorldMatrix{ 1.0f }
{
}

void SceneComponent::UpdateWorldMatrix()
{
    // T * R * S
    mWorldMatrix = glm::translate(glm::mat4(1.0f), mPosition) * glm::toMat4(mRotation) * glm::scale(glm::mat4(1.0f), mScale);
}
const glm::mat4& SceneComponent::GetWorldMatrix() const
{
    return mWorldMatrix;
}

void SceneComponent::SetPosition(const glm::vec3& position)
{
    mPosition = position;
    UpdateWorldMatrix();
}

void SceneComponent::SetRotation(const glm::quat& rotation)
{
    mRotation = rotation;
    mEulerRotation = glm::eulerAngles(rotation); // radians
    UpdateWorldMatrix();
}

/*
Pitch rotate around Right vector (0.0f, 1.0f, 0.0f)
Roll rotate around Forward vector (1.0f, 0.0f, 0.0f)
Yaw rotate around Up vector (0.0f, 0.0f, 1.0f)
*/
void SceneComponent::SetRotation(const glm::vec3& rotation)
{
    mEulerRotation = glm::radians(rotation);
    mRotation = glm::angleAxis(-rotation.y, axes::UpVector)// Yaw
        * glm::angleAxis(-rotation.x, axes::RightVector)   // Pitch
        * glm::angleAxis(-rotation.z, axes::ForwardVector);// Roll
    UpdateWorldMatrix();
}

void SceneComponent::SetScale(const glm::vec3& scale)
{
    mScale = scale;
    UpdateWorldMatrix();
}

glm::vec3 SceneComponent::GetForward() const
{
    return glm::rotate(mRotation, axes::ForwardVector);
}

glm::vec3 SceneComponent::GetRight() const
{
    return glm::rotate(mRotation, axes::RightVector);
}

glm::vec3 SceneComponent::GetUp() const
{
    return glm::rotate(mRotation, axes::UpVector);
}



} //rendering_engine