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

#include "vertex_declarations.hpp"

namespace rendering_engine
{
class Actor;
class SceneComponent;
class ImageData;
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
	void SetColorTexture(std::string path);
	void SetModelMesh(std::string path);

private:
	DrawableComponent(const DrawableComponent& rhs);
	DrawableComponent& operator=(const DrawableComponent& rhs);

protected:
	void LoadModel(std::string const modelFilepath);

protected:
	std::shared_ptr<SceneComponent> mSceneComponent;

	std::shared_ptr<ImageData> mColorTextureImageData;

	std::string mMeshPath;
	std::string mColorTexturePath;

	std::vector<Vertex> mVertices;
	std::vector<uint32_t> mIndices;
};

} //rendering_engine