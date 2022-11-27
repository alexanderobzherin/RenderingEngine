#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <optional>
#include <fstream>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace rendering_engine
{

class DrawableComponent
{
public:
	DrawableComponent() = default;
	virtual ~DrawableComponent() = default;

	virtual void Initialize() = 0;
	virtual void Update(float delta) = 0;
	virtual void Draw() = 0;

	virtual void Shutdown() = 0;

private:
	DrawableComponent(const DrawableComponent& rhs);
	DrawableComponent& operator=(const DrawableComponent& rhs);
};

} //rendering_engine