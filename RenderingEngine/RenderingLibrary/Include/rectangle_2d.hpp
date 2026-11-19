// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "drawable_2d.hpp"
#include "physical_geometry_definitions.hpp"

namespace rendering_engine
{
class Scene;
/**
 * @brief Drawable rectangle in 2D space.
 *
 * Supports configurable size, color, and optional
 * physical geometry for collision queries and interactions.
 *
 * @note Not copyable or assignable.
 * @see Drawable2D, PhysicalGeometry2D
 */
class RE_API Rectangle2D : public Drawable2D
{
public:
/**
 * @brief Configuration properties for a 2D rectangle.
 */
struct Properties
{
	/// Rectangle dimensions in local coordinates.
	glm::vec2 size = {50.0f, 50.0f};
	/// Rectangle color in normalized RGBA components.
	glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	/// Optional physical geometry configuration.
	PhysicalProperties2D physical;
};
public:
	/**
	 * @brief Constructs a 2D rectangle.
	 *
	 * @param renderContext Rendering resource context.
	 * @param scene Scene that owns the rectangle.
	 * @param properties Rectangle configuration.
	 */
	Rectangle2D(RenderResourceContext renderContext, Scene& scene, Properties properties);

	/**
	* @copydoc DrawableComponent::Initialize
	*/
	void Initialize() override;

	/**
	* @copydoc DrawableComponent::Update
	*/
	void Update(float deltaTime) override;

	/**
	* @copydoc Drawable2D::Draw
	*/
	void Draw(const Camera2D& camera) override;

	/**
	 * @brief Sets the rectangle's rendering color.
	 * @param color Color in normalized RGBA components.
	 */
	void SetColor(glm::vec4 color);
	/**
	 * @brief Initializes the rectangle's optional physical geometry.
	 *
	 * Configures the rectangle's scale from its size and attaches
	 * physical geometry according to the specified properties.
	 *
	 * If collision is disabled, no physical geometry is attached.
	 */
	void InitializePhysicalGeometry();

	Rectangle2D(const Rectangle2D& rhs) = delete;
	Rectangle2D& operator=(const Rectangle2D& rhs) = delete;

protected:
	Properties mProperties;

};

} // namespace rendering_engine
