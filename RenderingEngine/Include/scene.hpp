// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <memory>
#include <vector>
#include <string>

namespace rendering_engine
{
class SceneManager;
class Camera;
class Camera2D;
class Drawable3D;
class Drawable2D;

class Scene
{
public:
	Scene(SceneManager& sceneManager);
	~Scene() = default;

	virtual void Initialize();
	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual void Shutdown();

	SceneManager& GetSceneManager();

	void LoadScene(std::string newSceneName);

protected:
	SceneManager& mSceneManager;

	std::shared_ptr<Camera>  mActiveCamera3D;
	std::shared_ptr<Camera2D> mActiveCamera2D;

	std::vector<Drawable3D*> mDrawables3D;
	std::vector<Drawable2D*> mDrawables2D;
};

} // namespace rendering_engine