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
 * @brief Base class for drawable objects in 2D space.
 *
 * Provides a 2D transform, rendering interface, and optional
 * physical geometry for collision queries and interactions.
 *
 * @note Not copyable or assignable.
 * @see DrawableComponent, SceneComponent2D, PhysicalGeometry2D
 */
class RE_API Drawable2D : public DrawableComponent
{
public:
	/**
	 * @brief Constructs a 2D drawable component.
	 *
	 * @param renderContext Rendering resource context.
	 * @param scene Scene that owns the drawable.
	 */
	Drawable2D(RenderResourceContext renderContext, Scene& scene);

	~Drawable2D() override;


	/**
	 * @brief Initializes render resources.
	 */
	void Initialize() override;

	/**
	 * @brief Updates the drawable's world transform when enabled.
	 *
	 * @param deltaTime Time step in seconds.
	 */
	void Update(float deltaTime) override;

	/**
	 * @brief Submits the drawable for rendering.
	 *
	 * @param camera Camera used for rendering.
	 */
	virtual void Draw(const Camera2D& camera) = 0;

	/**
	 * @brief Releases physical geometry and rendering resources.
	 *
	 * Unregisters attached physical geometry before destroying it.
	 */
	void Shutdown() override;

	/**
	 * @brief Sets the drawable's position in 2D space.
	 * @param position New position.
	 */
	void SetPosition(const glm::vec2& position);

	/**
	 * @brief Sets the drawable's rotation angle in degrees.
	 * @param angleDegrees Rotation angle in degrees.
	 */
	void SetRotation(float angleDegrees);

	/**
	 * @brief Sets the drawable's scale.
	 * @param scale Scale along the X and Y axes.
	 */
	void SetScale(const glm::vec2& scale);

	/** @brief Returns the drawable's position. */
	const glm::vec2& GetPosition() const;

	/**
	 * @brief Returns the drawable's rotation angle in degrees.
	 */
	float GetRotation() const;

	/**
	 * @brief Gets the quad scale.
	 */
	const glm::vec2& GetScale() const;

	/**
	 * @brief Returns the drawable's scene component.
	 */
	SceneComponent2D& GetTransform();
	const SceneComponent2D& GetTransform() const;
	///@}

	/**
	 * @copydoc DrawableComponent::Destroy
	 */
	void Destroy() override;

	/**
	 * @brief Attaches or replaces the drawable's physical geometry.
	 *
	 * Takes ownership of the geometry and registers it with the
	 * scene's collision world.
	 *
	 * Replacing or removing geometry unregisters the previous
	 * geometry and its interaction target.
	 *
	 * Passing nullptr removes the current geometry.
	 *
	 * @param geometry Physical geometry to attach.
	 *
	 * @throws std::invalid_argument If the geometry references
	 *         a different scene component.
	 */
	void SetPhysicalGeometry(std::unique_ptr<PhysicalGeometry2D> geometry);

	/**
	 * @brief Returns the attached physical geometry.
	 * @return Non-owning pointer, or nullptr if no geometry is attached.
	 */
	PhysicalGeometry2D* GetPhysicalGeometry();
	const PhysicalGeometry2D* GetPhysicalGeometry() const;

protected:
	SceneComponent2D mSceneComponent;
	std::unique_ptr<PhysicalGeometry2D> mPhysicalGeometry;
};

} // namespace rendering_engine