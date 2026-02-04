// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "scene.hpp"

#include <memory>
#include <glm/glm.hpp>

using namespace rendering_engine;

namespace rendering_engine
{
class SceneManager;
class StaticMesh;
class Quad2D;
}

namespace application
{

class StartLevel : public Scene
{
public:
	StartLevel(SceneManager& sceneManager);

	void Initialize() override;
	void Update(float deltaTime) override;

private:
	void AddTextBlock(const std::string& fontName,
					  const std::string& text,
					  glm::vec2 pos,
					  bool textShapeEnabled = true,
					  glm::vec4 color = glm::vec4(1.0f, 1.0, 1.0f, 1.0f));

};

} //application