// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "drawable_component.hpp"
#include "scene_component.hpp"

namespace rendering_engine
{

class DrawableComponent;
class Drawable3D;

/**
 * @class Actor
 * @brief Base class representing a 3D entity within a Scene.
 *
 * An Actor encapsulates:
 *  - A root SceneComponent defining its world transform.
 *  - A collection of attached 3D drawable subobjects.
 *  - Per-frame update logic.
 *  - Deferred destruction lifecycle integration with Scene.
 *
 * Actors are owned and lifetime-managed by the Scene that spawns them.
 * They must not be manually deleted.
 *
 * Transformations applied to the Actor propagate to all attached
 * drawable subobjects via hierarchical SceneComponent attachment.
 *
 * @note
 *  - Actors operate in the 3D domain and use SceneComponent.
 *  - Destruction is deferred and handled safely by Scene.
 *
 * @see Scene, SceneComponent, Drawable3D
 */
class RE_API Actor
{
    friend class Scene;
public:
	/**
	 * @brief Constructs an Actor associated with a Scene.
	 *
	 * The Scene becomes the owner of this Actor.
	 * The root SceneComponent is initialized with identity transform.
	 *
	 * @param scene Reference to the owning Scene.
	 */
    Actor(Scene& scene);

    virtual ~Actor();

	/**
	 * @brief Initializes the actor after creation.
	 *
	 * Called automatically by Scene::SpawnActor().
	 * Derived classes should override this method to:
	 *  - Create drawable subobjects
	 *  - Attach components
	 *  - Set initial transforms
	 */
    virtual void Initialize();

	/**
	 * @brief Sets the actor's position in world space.
	 * @param position New position vector (x, y, z).
	 */
	void SetPosition(const glm::vec3& position);

	/**
	 * @brief Sets the actor's rotation in degrees.
	 * @param rotation New rotation vector (pitch, yaw, roll), in degrees.
	 */
	void SetRotation(const glm::vec3& rotation);

	/**
	 * @brief Sets the actor's scale along each axis.
	 * @param scale New scale vector (x, y, z).
	 */
	void SetScale(const glm::vec3& scale);

	/**
	 * @brief Gets the actor's position.
	 */
	const glm::vec3& GetPosition() const;

	/**
	 * @brief Gets the actor's rotation (pitch, yaw, roll in degrees).
	 */
	const glm::vec3& GetRotation() const;

	/**
	 * @brief Gets the actor's scale.
	 */
	const glm::vec3& GetScale() const;

	/**
	 * @brief Access to the underlying SceneComponent (transform).
	 */
	SceneComponent& GetTransform();
	const SceneComponent& GetTransform() const;
	///@}

	/**
	 * @brief Updates actor logic and root transform state.
	 *
	 * Called once per frame by Scene::Update().
	 *
	 * The base implementation updates the root SceneComponent's
	 * world matrix if ticking is enabled.
	 *
	 * Derived classes should call Actor::Update(deltaTime)
	 * before or after custom logic as appropriate.
	 *
	 * @param deltaTime Time elapsed since last frame (seconds).
	 */
    virtual void Update(float deltaTime);
	/**
	 * @brief Returns the render resource context associated with this actor.
	 *
	 * This context provides access to renderer-level resources
	 * required by attached drawable components.
	 *
	 * @return RenderResourceContext for this actor.
	 */
    RenderResourceContext GetRenderContext() const;
	/**
	 * @brief Requests deferred destruction of this actor.
	 *
	 * Marks the actor as pending destroy and schedules safe removal
	 * via the owning Scene.
	 *
	 * The actor is not deleted immediately. Instead:
	 *  - Attached drawable subobjects are detached and scheduled for destruction.
	 *  - The actor is added to the Scene's pending destruction queue.
	 *
	 * @note
	 *  - Do not manually delete actors.
	 *  - After calling Destroy(), IsPendingDestroy() will return true.
	 *  - Actual deletion occurs during Scene's flush phase.
	 */
    void Destroy();
	/**
	 * @brief Indicates whether this actor is scheduled for destruction.
	 *
	 * @return True if Destroy() has been called and the actor is awaiting removal.
	 */
	bool IsPendingDestroy() const { return bPendingDestroy; }

    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;

protected:
	/**
	 * @brief Performs internal cleanup before destruction.
	 *
	 * Called by Scene during the destruction flush phase.
	 * Derived classes may override this to release resources,
	 * but must not delete the actor manually.
	 *
	 * @note This is part of the deferred destruction pipeline.
	 */
    virtual void Shutdown();

	/**
	 * @brief Creates and attaches a drawable subobject to this actor.
	 *
	 * This helper function delegates creation to the owning Scene,
	 * registers the drawable for lifetime management, and stores it
	 * as a ward (child drawable) of the actor.
	 *
	 * @tparam T Drawable type (must derive from Drawable3D).
	 * @tparam V Construction parameter type.
	 * @param arg Argument forwarded to Scene::Spawn().
	 *
	 * @return Pointer to the created drawable subobject.
	 *
	 * @note
	 *  - The drawable is owned by the Scene.
	 *  - The actor keeps a non-owning reference in mWards.
	 *  - Destruction is automatically handled when the actor is destroyed.
	 */
    template <typename T, typename V>
    T* CreateSubobject(V arg);

protected:
    SceneComponent mRootComponent;
	bool bUpdateOnTick;

	bool bPendingDestroy = false;

private:
    Scene& mScene;
    RenderResourceContext mRenderContext;
    std::vector<Drawable3D*> mWards;

};

} // namespace rendering_engine