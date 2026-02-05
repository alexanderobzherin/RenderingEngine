// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "scene.hpp"

#include <memory>
#include <functional>
#include <vector>
#include <list>

using namespace rendering_engine;

namespace rendering_engine
{
class SceneManager;
class DrawableComponent;
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
	int GenerateNumber(int min, int max);
	void SpawnRandomObject();
	void DestroyRandomObject();

private:
	const int mFrequency = 100;
	const int mInnitNumberOfObjects = 10;
	int mTicking = 0;
	std::vector<std::function<DrawableComponent* ()>> mDrawableSpawners;
	std::vector<DrawableComponent*> mHandledDrawables;
};

} //application