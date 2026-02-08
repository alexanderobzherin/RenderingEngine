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
class Actor;
class Actor2D;

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
	friend class Actor;
	friend class Actor2D;
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

	/**
	 * @brief Returns the active 3D camera of the scene.
	 * @return Active 3D camera.
	 */
	std::shared_ptr<Camera>  GetActiveCamera3D();

	/**
	 * @brief Returns the active 2D camera of the scene.
	 * @return Active 2D camera.
	 */
	std::shared_ptr<Camera2D> GetActiveCamera2D();

	/**
	 * @brief Spawns and registers a drawable of type T.
	 *
	 * All drawables must be created via this function to ensure
	 * correct Scene-level ownership and lifecycle management.
	 * Creation logic is provided by explicit template specializations.
	 *
	 * @tparam T    Drawable type.
	 * @tparam V Construction argument type.
	 * @param arg  Construction argument.
	 * @return Pointer to the created drawable.
	 */
	template <typename T, typename V>
	T* Spawn(V arg);

	template <typename T>
	T* SpawnActor();

protected:
	/**
	 * @brief Schedules a 3D drawable for deferred destruction.
	 */
	void DestroyDrawable3D(Drawable3D* drawable3D);
	/**
	 * @brief Schedules a 2D drawable for deferred destruction.
	 */
	void DestroyDrawable2D(Drawable2D* drawable2D);
	/**
	 * @brief Schedules a 2D actor for deferred destruction.
	 */
	void DestroyActor(Actor* actor);

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

	std::vector<Actor*> mActors;
	std::vector<Actor*> mPendingDestroyActors;

	std::vector<Actor2D*> mActors2D;
};	

template <typename T>
inline T* Scene::SpawnActor()
{
	static_assert(std::is_base_of_v<Actor, T>,
		"SpawnActor<T>: T must derive from Actor");

	mActors.push_back(new T(*this));
	T* ptr = static_cast<T*>(mActors.back());

	ptr->Initialize();
	return ptr;
}

} // namespace rendering_engine
