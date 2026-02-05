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
class RE_API Camera2D;

/**
 * @class Drawable2D
 * @brief 2D drawable component for rendering objects in 2D space.
 *
 * Encapsulates a 2D transform (SceneComponent2D).
 * Provides per-frame update, transform access, and render submission.
 *
 * @note Not copyable or assignable.
 * @see DrawableComponent, SceneComponent2D, Camera2D
 */
class RE_API Drawable2D : public DrawableComponent
{
public:
	/**
	 * @brief Constructs the Drawable2D with a resource context.
	 * @param renderContext Rendering resource context (renderer, caches).
	 */
	Drawable2D(RenderResourceContext renderContext, Scene& scene);

	/**
	 * @brief Initializes render resources.
	 */
	void Initialize() override;

	/**
	 * @brief Updates model matrix (and any other logic).
	 * @param deltaTime Time step (seconds).
	 */
	void Update(float deltaTime) override;

	/**
	 * @brief Submits this quad to the renderer for drawing.
	 */
	virtual void Draw(const Camera2D& camera) = 0;

	/**
	 * @brief Sets the quad position in 2D space.
	 * @param position New position vector (x, y).
	 */
	void SetPosition(const glm::vec2& position);

	/**
	 * @brief Sets the quad rotation.
	 * @param angleDegrees Rotation angle in degrees (counterclockwise).
	 */
	void SetRotation(float angleDegrees);

	/**
	 * @brief Sets the quad scale along each axis.
	 * @param scale New scale vector (x, y).
	 */
	void SetScale(const glm::vec2& scale);

	/**
	 * @brief Gets the quad position.
	 */
	const glm::vec2& GetPosition() const;

	/**
	 * @brief Gets the quad rotation angle (degrees).
	 */
	float GetRotation() const;

	/**
	 * @brief Gets the quad scale.
	 */
	const glm::vec2& GetScale() const;

	/**
	 * @brief Access to the underlying SceneComponent2D (transform).
	 */
	SceneComponent2D& GetTransform();
	const SceneComponent2D& GetTransform() const;
	///@}

	/**
	 * @copydoc DrawableComponent::Destroy
	 */
	void Destroy() override;

protected:
	SceneComponent2D mSceneComponent;
};

} // namespace rendering_engine