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

class RE_API Actor2D
{
    friend class Scene;
public:
    Actor2D(Scene& scene);

    virtual ~Actor2D();

	virtual void Initialize();
	/**
	 * @brief Sets the actor's position in world space.
	 * @param position New position vector (x, y).
	 */
	void SetPosition(const glm::vec2& position);

	/**
	 * @brief Sets the actor's rotation in degrees.
	 * @param rotation New rotation vector (pitch, yaw, roll), in degrees.
	 */
	void SetRotation(float angleDegrees);

	/**
	 * @brief Sets the actor's scale along each axis.
	 * @param scale New scale vector (x, y, z).
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
	 * @brief Access to the underlying SceneComponent (transform).
	 */
	SceneComponent2D& GetTransform();
	const SceneComponent2D& GetTransform() const;
	///@}

	/**
	 * @brief Updates logic (animation, movement, etc.) for this actor.
	 * @param deltaTime Time step (seconds).
	 */
	virtual void Update(float deltaTime);

	RenderResourceContext GetRenderContext() const;

	void Destroy();
	bool IsPendingDestroy() const { return bPendingDestroy; }

	Actor2D(const Actor2D&) = delete;
	Actor2D& operator=(const Actor2D&) = delete;

protected:
	virtual void Shutdown();

	template <typename T, typename V>
	T* CreateSubobject(V arg);

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
 