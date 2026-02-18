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
class StatsOverlay;

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
 * 
 * The Scene implements a deferred destruction mechanism to guarantee
 * safe removal of actors and drawables during update and render passes.
 * All objects spawned via the Scene are owned and lifetime-managed by it.
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

	/**
	 * @brief Spawns and registers a 3D actor of type T.
	 *
	 * The actor is:
	 *  - Allocated and owned by the Scene.
	 *  - Stored internally in the 3D actor list.
	 *  - Automatically initialized via Initialize().
	 *
	 * The Scene manages the full lifecycle of the actor, including
	 * deferred destruction and Shutdown() invocation.
	 *
	 * @tparam T Type deriving from Actor.
	 * @return Pointer to the created actor instance.
	 *
	 * @note The returned pointer is owned by the Scene.
	 *       Do not manually delete the actor.
	 *
	 * @see DestroyActor(), SpawnActor2D()
	 */
	template <typename T>
	T* SpawnActor();

	/**
	 * @brief Spawns and registers a 2D actor of type T.
	 *
	 * The actor is:
	 *  - Allocated and owned by the Scene.
	 *  - Stored internally in the 2D actor list.
	 *  - Automatically initialized via Initialize().
	 *
	 * This function mirrors SpawnActor() but operates in the 2D domain.
	 * 2D actors use SceneComponent2D for hierarchical transformations.
	 *
	 * @tparam T Type deriving from Actor2D.
	 * @return Pointer to the created 2D actor instance.
	 *
	 * @note The returned pointer is owned by the Scene.
	 *       Do not manually delete the actor.
	 *
	 * @see SpawnActor(), DestroyActor(Actor2D*)
	 */
	template <typename T>
	T* SpawnActor2D();

protected:
	/**
	 * @brief Schedules a 3D drawable for deferred destruction.
	 *
	 * The drawable will be shut down and removed safely during
	 * the next Scene flush cycle.
	 *
	 * @param drawable3D Pointer to the drawable to destroy.
	 */
	void DestroyDrawable3D(Drawable3D* drawable3D);
	/**
	 * @brief Schedules a 2D drawable for deferred destruction.
	 *
	 * The drawable will be shut down and removed safely during
	 * the next Scene flush cycle.
	 *
	 * @param drawable2D Pointer to the drawable to destroy.
	 */
	void DestroyDrawable2D(Drawable2D* drawable2D);
	/**
	 * @brief Schedules a 3D actor for deferred destruction.
	 *
	 * The actor is not destroyed immediately. Instead, it is placed into
	 * a pending destruction queue and safely removed during the next
	 * Scene update cycle.
	 *
	 * This ensures that destruction does not invalidate iterators or
	 * interfere with the current update/draw pass.
	 *
	 * @param actor Pointer to the actor to destroy.
	 *
	 * @note Do not delete actors manually. Always use Actor::Destroy().
	 */
	void DestroyActor(Actor* actor);
	/**
	 * @brief Schedules a 2D actor for deferred destruction.
	 *
	 * Mirrors DestroyActor(Actor*) but operates on the 2D actor list.
	 * The actor will be safely shut down and deleted during
	 * the next flush phase.
	 *
	 * @param actor2D Pointer to the 2D actor to destroy.
	 *
	 * @note Do not delete actors manually. Always use Actor2D::Destroy().
	 */
	void DestroyActor(Actor2D * actor2D);

private:
	/**
	 * @brief Processes all pending destruction queues.
	 *
	 * This method:
	 *  - Shuts down and deletes pending 3D drawables
	 *  - Shuts down and deletes pending 2D drawables
	 *  - Shuts down and deletes pending 3D actors
	 *  - Shuts down and deletes pending 2D actors
	 *
	 * Called internally during Scene::Update() and Scene::Shutdown().
	 *
	 * @note Ensures safe destruction without invalidating update/draw loops.
	 */
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
	std::vector<Actor2D*> mPendingDestroyActors2D;

	StatsOverlay* mStatsOverlay;
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

template <typename T>
inline T* Scene::SpawnActor2D()
{
	static_assert(std::is_base_of_v<Actor2D, T>,
		"SpawnActor<T>: T must derive from Actor2D");

	mActors2D.push_back(new T(*this));
	T* ptr = static_cast<T*>(mActors2D.back());

	ptr->Initialize();
	return ptr;
}

} // namespace rendering_engine
