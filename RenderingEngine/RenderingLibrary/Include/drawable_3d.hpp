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
class Camera;

/**
 * @class Drawable3D
 * @brief 3D drawable component for rendering objects in 3D space.
 *
 * Encapsulates a transform (SceneComponent).
 * Provides per-frame update, world transform access, and render submission.
 *
 * @note Not copyable or assignable.
 * @see DrawableComponent, SceneComponent, Camera
 */
class RE_API Drawable3D : public DrawableComponent
{
public:
	/**
	 * @brief Constructs the Drawable3D with a render context.
	 * @param renderContext Resource context.
	 */
	Drawable3D(RenderResourceContext renderContext);

	/**
	 * @copydoc DrawableComponent::Initialize
	 */
	void Initialize() override;

	/**
	 * @copydoc DrawableComponent::Update
	 */
	void Update(float deltaTime) override;

	/**
	 * @brief Submits this mesh to the renderer for drawing.
	 */
	virtual void Draw(const Camera& camera) = 0;

	/**
	 * @brief Sets the mesh position in world space.
	 * @param position New position vector (x, y, z).
	 */
	void SetPosition(const glm::vec3& position);

	/**
	 * @brief Sets the mesh rotation in degrees.
	 * @param rotation New rotation vector (pitch, yaw, roll), in degrees.
	 */
	void SetRotation(const glm::vec3& rotation);

	/**
	 * @brief Sets the mesh scale along each axis.
	 * @param scale New scale vector (x, y, z).
	 */
	void SetScale(const glm::vec3& scale);

	/**
	 * @brief Gets the mesh position.
	 */
	const glm::vec3& GetPosition() const;

	/**
	 * @brief Gets the mesh rotation (pitch, yaw, roll in degrees).
	 */
	const glm::vec3& GetRotation() const;

	/**
	 * @brief Gets the mesh scale.
	 */
	const glm::vec3& GetScale() const;

	/**
	 * @brief Access to the underlying SceneComponent (transform).
	 */
	SceneComponent& GetTransform();
	const SceneComponent& GetTransform() const;
	///@}

	Drawable3D(const Drawable3D& rhs) = delete;
	Drawable3D& operator=(const Drawable3D& rhs) = delete;

protected:
	SceneComponent mSceneComponent;
};

} // namespace rendering_engine