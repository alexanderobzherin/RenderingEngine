#include "scene_component.hpp"

namespace rendering_engine
{
    SceneComponent::SceneComponent()
        :
        mPosition(0.0f),
        mDirection(0.0f, 0.0f, -1.0f), // Forward)
        mUp(0.0f, 1.0f, 0.0f), // Up
        mRight(1.0f, 0.0f, 0.0f), // Right
        mWorldMatrix(1.0f),
        mScaleMatrix(1.0f),
        mRotationMatrix(1.0f)
    {
    }
    SceneComponent::~SceneComponent()
    {
    }
    void SceneComponent::Initialize()
    {
    }
    void SceneComponent::Update(float const delta)
    {
        SetRotation(45.0f * delta);
        
        mWorldMatrix = glm::mat4(1.0f);
        mWorldMatrix = glm::translate(mWorldMatrix, mPosition);
        mWorldMatrix = mScaleMatrix * mWorldMatrix * mRotationMatrix;
        
    }
    glm::mat4& SceneComponent::GetWorldMatrix()
    {
        return mWorldMatrix;
    }
    glm::mat4& SceneComponent::GetScaleMatrix()
    {
        return mScaleMatrix;
    }
    glm::mat4& SceneComponent::GetRotationMatrix()
    {
        return mRotationMatrix;
    }
    void SceneComponent::SetPosition(glm::vec3 position)
    {
        mPosition = position;
        mWorldMatrix = glm::translate(mWorldMatrix, mPosition);
    }
    void SceneComponent::SetRotation(float angle)
    {
        mRotationMatrix = glm::rotate(mRotationMatrix, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    void SceneComponent::SetScale(glm::vec3 scale)
    {
        mScaleMatrix = glm::scale( mScaleMatrix, scale );
    }
} //rendering_engine