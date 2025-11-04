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
    mModelMatrix{ 1.0f }
{
    UpdateModelMatrix();
}

void SceneComponent2D::SetPosition(const glm::vec2& position)
{
    mPosition = position;
    UpdateModelMatrix();
}

void SceneComponent2D::SetRotation(float angleDegrees)
{
    mRotation = angleDegrees;
    UpdateModelMatrix();
}

void SceneComponent2D::SetScale(const glm::vec2& scale)
{
    mScale = scale;
    UpdateModelMatrix();
}

void SceneComponent2D::UpdateModelMatrix()
{
    const float r = glm::radians(mRotation);

    const glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(mScale, 1.0f));
    const glm::mat4 R = glm::rotate(glm::mat4(1.0f), r, glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(mPosition, 0.0f));

    mModelMatrix = T * R * S;
}

} //rendering_engine