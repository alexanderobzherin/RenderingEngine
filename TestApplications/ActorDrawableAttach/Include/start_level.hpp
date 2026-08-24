// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "scene.hpp"

#include <memory>

using namespace rendering_engine;

namespace rendering_engine
{
class SceneManager;
}

namespace application
{
class CubeClusterActor;
class TestActor2D;

class StartLevel : public Scene
{
public:
	StartLevel(SceneManager& sceneManager);

	void Initialize() override;
	void Update(float deltaTime) override;

private:
	const float mRespawnIntervalSeconds = 60.0f;
	float mElapsedTime = 0.0f;
	CubeClusterActor* mCubeClusterActor;
	TestActor2D* mTestActor2D;
};

} //application