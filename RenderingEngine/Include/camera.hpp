#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

#include "renderer_base.hpp"

namespace rendering_engine
{

class Camera
{
public:
	Camera(RendererBase* renderer);
	virtual ~Camera();

	virtual void Initialize();
	virtual void Update(float const delta);

	const glm::mat4& ViewMatrix() const;
	const glm::mat4& ProjectionMatrix() const;
	glm::mat4 ViewProjectionMatrix() const;

	void Reset();
	void UpdateProjectionMatrix();
	void UpdateViewMatrix();

private:
	Camera(Camera const& rhs);
	Camera& operator=(Camera const& rhs);

protected:
	RendererBase* mRenderer;

	glm::vec3 mPosition;
	glm::vec3 mDirection;
	glm::vec3 mUp;
	glm::vec3 mRight;

	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;

	static const float sDefaultFieldOfView;
	static const float sDefaultAspectRatio;
	static const float sDefaultNearPlaneDistance;
	static const float sDefaultFarPlaneDistance;

	float mFieldOfView;
	float mAspectRatio;
	float mNearPlaneDistance;
	float mFarPlaneDistance;
};
} //rendering_engine