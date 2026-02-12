#include "scene_component_2d.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace rendering_engine
{

SceneComponent2D::SceneComponent2D()
    :
    mPosition{ 0.0f, 0.0f },
    mRotation{ 0.0f },
    mScale{ 1.0f, 1.0f },
    mWorldMatrix{ 1.0f },
    mLocalMatrix{ 1.0f },
    mParent(nullptr),
    bIsDirty(true)
{
}

void SceneComponent2D::SetPosition(const glm::vec2& position)
{
    mPosition = position;
    bIsDirty = true;
}

void SceneComponent2D::SetRotation(float angleDegrees)
{
    mRotation = angleDegrees;
    bIsDirty = true;
}

void SceneComponent2D::SetScale(const glm::vec2& scale)
{
    mScale = scale;
    bIsDirty = true;
}

glm::vec2 SceneComponent2D::GetWorldPosition() const
{
    const glm::mat4& world = const_cast<SceneComponent2D*>(this)->GetWorldMatrix();
    return glm::vec2(world[3]);
}

float SceneComponent2D::GetWorldRotation() const
{
    const glm::mat4& world = const_cast<SceneComponent2D*>(this)->GetWorldMatrix();

    float angle = atan2(world[1][0], world[0][0]);
    return glm::degrees(angle);
}

glm::vec2 SceneComponent2D::GetWorldScale() const
{
    const glm::mat4& world = const_cast<SceneComponent2D*>(this)->GetWorldMatrix();

    return glm::vec2(
        glm::length(glm::vec2(world[0])),
        glm::length(glm::vec2(world[1]))
    );
}

const glm::mat4& SceneComponent2D::GetWorldMatrix()
{
    if (bIsDirty)
    {
        UpdateWorldMatrix();
    }
    return mWorldMatrix;
}

void SceneComponent2D::UpdateLocalMatrix()
{
    if (!bIsDirty)
    {
        return;
    }
    const float r = glm::radians(mRotation);

    const glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(mScale, 1.0f));
    const glm::mat4 R = glm::rotate(glm::mat4(1.0f), r, glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(mPosition, 0.0f));

    mLocalMatrix = T * R * S;
}

void SceneComponent2D::UpdateWorldMatrix()
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

void SceneComponent2D::AttachTo(SceneComponent2D* parent)
{
    mParent = parent;

    bIsDirty = true;
}

} //rendering_engine