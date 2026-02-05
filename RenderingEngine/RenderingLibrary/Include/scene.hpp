// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <memory>
#include <vector>
#include <string>

#include "rendering_engine_export.hpp"
#include "drawable_component.hpp"

namespace rendering_engine
{
class SceneManager;
class Camera;
class Camera2D;
class Drawable3D;
class Drawable2D;

/**
 * @class Scene
 * @brief Base class representing a renderable scene.
 *
 * A Scene acts as a container for drawable objects (2D and 3D) and defines
 * the update and draw lifecycle of a game or rendering context.
 *
 * Derived scene classes should override Initialize(), Update(), and Draw()
 * to implement specific logic and populate the drawable lists.
 *
 * @note Scenes are created and managed by SceneManager.
 * @see SceneManager, Drawable3D, Drawable2D, Camera, Camera2D
 */
class RE_API Scene
{
	friend class Drawable3D;
	friend class Drawable2D;
public:
	/**
	 * @brief Constructs a Scene instance associated with a SceneManager.
	 * @param sceneManager Reference to the SceneManager that owns this Scene.
	 */
	Scene(SceneManager& sceneManager);
	/**
	 * @brief Virtual destructor for safe polymorphic destruction.
	 */
	~Scene() = default;
	/**
	 * @brief Initializes scene resources and drawables.
	 *
	 * Called once when the scene becomes active.
	 * Derived classes should override this to load assets and create objects.
	 */
	virtual void Initialize();
	/**
	 * @brief Updates all objects within the scene.
	 * @param deltaTime Time elapsed since the last frame (in milliseconds).
	 *
	 * Called once per frame. Override to implement per-frame logic such as
	 * animation, physics updates, or scene transitions.
	 */
	virtual void Update(float deltaTime);
	/**
	 * @brief Renders all 3D and 2D drawables in the scene.
	 *
	 * The base implementation can iterate through mDrawables3D and mDrawables2D
	 * to call each object�s Draw() method, using the active cameras.
	 */
	virtual void Draw();
	/**
	 * @brief Releases scene-specific resources and prepares for shutdown.
	 *
	 * Called when the scene is being unloaded or replaced.
	 */
	virtual void Shutdown();
	/**
	 * @brief Gets a reference to the SceneManager that owns this scene.
	 * @return Reference to the owning SceneManager.
	 */
	SceneManager& GetSceneManager();
	/**
	 * @brief Requests to load another scene.
	 * @param newSceneName The name of the registered scene to load next.
	 *
	 * This function delegates to SceneManager::LoadScene().
	 */
	void LoadScene(std::string newSceneName);

	std::shared_ptr<Camera>  GetActiveCamera3D();
	std::shared_ptr<Camera2D> GetActiveCamera2D();

	template <typename T, typename Args>
	T* Spawn(Args args);

protected:
	friend class Drawable;
	void DestroyDrawable3D(Drawable3D* drawable3D);
	void DestroyDrawable2D(Drawable2D* drawable2D);

private:
	void FlushDestroyed();

private:
	SceneManager& mSceneManager;

	std::shared_ptr<Camera>  mActiveCamera3D;
	std::shared_ptr<Camera2D> mActiveCamera2D;

	std::vector<Drawable3D*> mDrawables3D;
	std::vector<Drawable2D*> mDrawables2D;

	std::vector<Drawable3D*> mPendingDestroy3D;
	std::vector<Drawable2D*> mPendingDestroy2D;
};	


} // namespace rendering_engine