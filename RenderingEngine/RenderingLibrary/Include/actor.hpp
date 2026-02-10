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

class SceneComponent;
class DrawableComponent;
class Drawable3D;

class RE_API Actor
{
    friend class Scene;
public:
    Actor(Scene& scene);

    virtual ~Actor();

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
     * @brief Updates logic (animation, movement, etc.) for this actor.
     * @param deltaTime Time step (seconds).
     */
    virtual void Update(float deltaTime);

    RenderResourceContext GetRenderContext() const;

    void Destroy();
	bool IsPendingDestroy() const { return bPendingDestroy; }

    Actor(const Actor&) = delete;
    Actor& operator=(const Actor&) = delete;

protected:
    virtual void Shutdown();

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