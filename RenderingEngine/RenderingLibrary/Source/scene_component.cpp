#include "scene_component.hpp"

namespace rendering_engine
{

SceneComponent::SceneComponent()
    :
    mPosition{ 0.0f },
    mRotation{ 1.0f, 0.0f, 0.0f, 0.0f },
    mEulerRotation{ 0.0f },
    mScale{ 1.0f, 1.0f, 1.0f },
    mWorldMatrix{ 1.0f },
    mParent(nullptr),
    bIsDirty(true)
{
}

void SceneComponent::UpdateLocalMatrix()
{
    if (!bIsDirty)
    {
        return;
    }
    // T * R * S
    mLocalMatrix =
        glm::translate(glm::mat4(1.0f), mPosition) *
        glm::toMat4(mRotation) *
        glm::scale(glm::mat4(1.0f), mScale);
}
void SceneComponent::UpdateWorldMatrix()
{
    UpdateLocalMatrix();

    if (mParent)
    {
        mWorldMatrix = mParent->GetWorldMatrix() * mLocalMatrix;
    }
    else
    {
        mWorldMatrix = mLocalMatrix;
    }
    bIsDirty = false;
}
const glm::mat4& SceneComponent::GetWorldMatrix()
{
    if (bIsDirty)
    {
        UpdateWorldMatrix();
    }
    return mWorldMatrix;
}

void SceneComponent::SetPosition(const glm::vec3& position)
{
    mPosition = position;

    bIsDirty = true;
}

void SceneComponent::SetRotation(const glm::quat& rotation)
{
    mRotation = rotation;
    mEulerRotation = glm::eulerAngles(rotation); // radians

    bIsDirty = true;
}

/*
Pitch rotate around Right vector (0.0f, 1.0f, 0.0f)
Roll rotate around Forward vector (1.0f, 0.0f, 0.0f)
Yaw rotate around Up vector (0.0f, 0.0f, 1.0f)
*/
void SceneComponent::SetRotation(const glm::vec3& rotation)
{
    mEulerRotation = rotation;
    const glm::vec3 radians = glm::radians(rotation);
    mRotation = glm::angleAxis(-radians.y, axes::UpVector)// Yaw
        * glm::angleAxis(-radians.x, axes::RightVector)   // Pitch
        * glm::angleAxis(-radians.z, axes::ForwardVector);// Roll

    bIsDirty = true;
}

void SceneComponent::SetScale(const glm::vec3& scale)
{
    mScale = scale;

    bIsDirty = true;
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

glm::vec3 SceneComponent::GetWorldPosition() const
{
    const glm::mat4& world = const_cast<SceneComponent*>(this)->GetWorldMatrix();
    return glm::vec3(world[3]);
}

glm::quat SceneComponent::GetWorldRotation() const
{
    // Extract rotation from the upper-left 3x3 matrix
    const glm::mat4& world = const_cast<SceneComponent*>(this)->GetWorldMatrix();
    glm::mat3 rotationMatrix(world);
    return glm::quat_cast(rotationMatrix);
}

glm::vec3 SceneComponent::GetWorldScale() const
{
    const glm::mat4& world = const_cast<SceneComponent*>(this)->GetWorldMatrix();
    return glm::vec3(
        glm::length(glm::vec3(world[0])),
        glm::length(glm::vec3(world[1])),
        glm::length(glm::vec3(world[2]))
    );
}

void SceneComponent::AttachTo(SceneComponent* parent)
{
    mParent = parent;

    bIsDirty = true;
}

} //rendering_engine