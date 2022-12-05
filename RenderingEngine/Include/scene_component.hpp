#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <memory>

namespace rendering_engine
{
//This class contains main kinematik staff like position, rotations etc. to be able to live in 3D scene
class SceneComponent
{
public:
	SceneComponent();
	virtual ~SceneComponent();

	virtual void Initialize();
	virtual void Update(float const delta);

	glm::mat4& GetWorldMatrix();
	glm::mat4& GetScaleMatrix();
	glm::mat4& GetRotationMatrix();

	void SetPosition(glm::vec3 position);
	void SetRotation(float angle);
	void SetScale(glm::vec3 scale);

private:
	SceneComponent(SceneComponent const& rhs);
	SceneComponent& operator=(SceneComponent const& rhs);

protected:
	glm::vec3 mPosition;
	glm::vec3 mDirection;
	glm::vec3 mUp;
	glm::vec3 mRight;

	glm::mat4 mWorldMatrix;
	glm::mat4 mScaleMatrix;
	glm::mat4 mRotationMatrix;
};

} //rendering_engine