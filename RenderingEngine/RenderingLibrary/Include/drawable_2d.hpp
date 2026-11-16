// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "drawable_component.hpp"
#include "scene_component_2d.hpp"
#include "physical_geometry_2d.hpp"

#include <memory>

namespace rendering_engine
{

class Camera2D;

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

	~Drawable2D() override;


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
	 * @brief Unregisters and releases physical geometry, then
	 * performs normal drawable resource shutdown.
	 */
	void Shutdown() override;

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

	/**
	 * @brief Attaches or replaces optional physical geometry.
	 *
	 * The drawable takes ownership of the geometry and registers
	 * it with its owning Scene.
	 *
	 * Passing nullptr removes the current geometry.
	 *
	 * @note The supplied geometry must reference this drawable's
	 * SceneComponent2D and must not be registered elsewhere.
	 */
	void SetPhysicalGeometry(std::unique_ptr<PhysicalGeometry2D> geometry);

	/**
	 * @brief Returns the attached physical geometry, if any.
	 */
	PhysicalGeometry2D* GetPhysicalGeometry();
	const PhysicalGeometry2D* GetPhysicalGeometry() const;

protected:
	SceneComponent2D mSceneComponent;
	std::unique_ptr<PhysicalGeometry2D> mPhysicalGeometry;
};

} // namespace rendering_engine