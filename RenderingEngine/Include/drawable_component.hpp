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
class Actor;
class SceneComponent;
// Class contains needed renderer specific instrumentation to interact with renderer itself.
// For example, create renderer specific vertex buffer to make it usable for drawing.
class DrawableComponent
{
public:
	DrawableComponent();
	virtual ~DrawableComponent();

	virtual void Initialize();
	virtual void Update(float delta);
	virtual void Draw() = 0;

	virtual void Shutdown() = 0;

	std::shared_ptr<SceneComponent> GetSceneComponent();

private:
	DrawableComponent(const DrawableComponent& rhs);
	DrawableComponent& operator=(const DrawableComponent& rhs);

protected:
	std::shared_ptr<SceneComponent> mSceneComponent;

};

} //rendering_engine