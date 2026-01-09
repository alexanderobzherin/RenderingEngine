// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "scene.hpp"

#include <memory>

using namespace rendering_engine;

namespace rendering_engine
{
class SceneManager;
class StaticMesh;
class Quad2D;
class TextBlock2D;
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

	TextBlock2D* mTextBlockUpdating;
};

} //application