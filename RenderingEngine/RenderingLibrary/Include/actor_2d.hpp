// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "drawable_component.hpp"
#include "scene_component_2d.hpp"

namespace rendering_engine
{

class DrawableComponent;
class Drawable2D;

/**
 * @class Actor2D
 * @brief Base class representing a 2D entity within a Scene.
 *
 * Actor2D encapsulates:
 *  - A root SceneComponent2D defining its 2D transform.
 *  - A collection of attached 2D drawable subobjects.
 *  - Per-frame update logic.
 *  - Deferred destruction lifecycle integration with Scene.
 *
 * Actor2D operates strictly in the 2D domain:
 *  - Position is defined in (x, y).
 *  - Rotation is a single angle (degrees) around the Z-axis.
 *  - Scale is defined in (x, y).
 *
 * Transformations applied to the Actor2D propagate to all attached
 * drawable subobjects via hierarchical SceneComponent2D attachment.
 *
 * Actors are owned and lifetime-managed by the Scene that spawns them.
 * They must not be manually deleted.
 *
 * @note
 *  - Actor2D is conceptually separated from Actor (3D).
 *  - Destruction is deferred and handled safely by Scene.
 *
 * @see Scene, SceneComponent2D, Drawable2D, Actor
 */
class RE_API Actor2D
{
    friend class Scene;
public:
	/**
	 * @brief Constructs a 2D actor associated with a Scene.
	 *
	 * The Scene becomes the owner of this actor.
	 * The root SceneComponent2D is initialized with identity transform.
	 *
	 * @param scene Reference to the owning Scene.
	 */
    Actor2D(Scene& scene);

    virtual ~Actor2D();
	/**
	 * @brief Initializes the actor after creation.
	 *
	 * Called automatically by Scene::SpawnActor2D().
	 * Derived classes should override this method to:
	 *  - Create drawable subobjects
	 *  - Attach components
	 *  - Set initial transforms
	 */
	virtual void Initialize();
	/**
	 * @brief Sets the actor's position in world space.
	 * @param position New position vector (x, y).
	 */
	void SetPosition(const glm::vec2& position);

	/**
	 * @brief Sets the actor's rotation in degrees.
	 * @param angleDegrees Rotation angle in degrees (counterclockwise).
	 *
	 * Rotation is applied around the Z-axis.
	 */
	void SetRotation(float angleDegrees);

	/**
	 * @brief Sets the actor's scale along each axis.
	 * @param scale New scale vector (x, y).
	 */
	void SetScale(const glm::vec2& scale);

	/**
	 * @brief Gets the actor 2d position.
	 */
	const glm::vec2& GetPosition() const;

	/**
	 * @brief Gets the actor 2d rotation angle (degrees).
	 */
	float GetRotation() const;

	/**
	 * @brief Gets the actor 2d scale.
	 */
	const glm::vec2& GetScale() const;

	/**
	 * @brief Access to the underlying SceneComponent2D (transform).
	 *
	 * Allows direct manipulation of local or hierarchical transforms.
	 *
	 * @return Reference to the root 2D transform component.
	 */
	SceneComponent2D& GetTransform();
	const SceneComponent2D& GetTransform() const;
	///@}

	/**
	 * @brief Updates actor logic and root transform state.
	 *
	 * Called once per frame by Scene::Update().
	 *
	 * The base implementation updates the root SceneComponent2D's
	 * world matrix if ticking is enabled.
	 *
	 * Derived classes should call Actor2D::Update(deltaTime)
	 * before or after custom logic as appropriate.
	 *
	 * @param deltaTime Time elapsed since last frame (seconds).
	 */
	virtual void Update(float deltaTime);
	/**
	 * @brief Returns the render resource context associated with this actor.
	 *
	 * Provides access to renderer-level resources
	 * required by attached 2D drawable components.
	 *
	 * @return RenderResourceContext for this actor.
	 */
	RenderResourceContext GetRenderContext() const;
	/**
	 * @brief Requests deferred destruction of this 2D actor.
	 *
	 * Marks the actor as pending destroy and schedules safe removal
	 * via the owning Scene.
	 *
	 * The actor is not deleted immediately. Instead:
	 *  - Attached drawable subobjects are detached and scheduled for destruction.
	 *  - The actor is added to the Scene's 2D pending destruction queue.
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

	Actor2D(const Actor2D&) = delete;
	Actor2D& operator=(const Actor2D&) = delete;

protected:
	/**
	 * @brief Performs internal cleanup before destruction.
	 *
	 * Called by Scene during the destruction flush phase.
	 * Derived classes may override this to release resources,
	 * but must not delete the actor manually.
	 *
	 * @note Part of the deferred destruction pipeline.
	 */
	virtual void Shutdown();
	/**
	 * @brief Creates and attaches a 2D drawable subobject to this actor.
	 *
	 * This helper function delegates creation to the owning Scene,
	 * registers the drawable for lifetime management, and stores it
	 * as a ward (child drawable) of the actor.
	 *
	 * @tparam T Drawable type (must derive from Drawable2D).
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

	/**
	 * @brief Returns the owning Scene.
	 *
	 * Provides access to the scene that contains this actor.
	 *
	 * @return Reference to the parent Scene.
	 */
	Scene& GetScene();

protected:
	SceneComponent2D mRootComponent;
	bool bUpdateOnTick;

	bool bPendingDestroy = false;

private:
	Scene& mScene;
	RenderResourceContext mRenderContext;
	std::vector<Drawable2D*> mWards;
};

}// namespace rendering_engine
 