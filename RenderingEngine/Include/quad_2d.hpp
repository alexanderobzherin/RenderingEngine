// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "drawable_2d.hpp"

namespace rendering_engine
{
/**
 * @class Quad2D
 * @brief 2D drawable component for for rendering 2D quads 
 *        (sprites, UI widgets, fonts, or procedural materials).
 *
 * @note Not copyable or assignable.
 * @see Drawable2D, DrawableComponent, SceneComponent2D, Camera2D
 */
class Quad2D : public Drawable2D
{
public:
	/**
	 * @brief Constructs the Quad2D with a render context.
	 * @param renderContext Resource context.
	 */
	Quad2D(RenderResourceContext renderContext);

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

	using DrawableComponent::SetMaterialName; ///< @copydoc DrawableComponent::SetMaterialName
	using DrawableComponent::SetMeshName; ///< @copydoc DrawableComponent::SetMeshName

	Quad2D(const Quad2D& rhs) = delete;
	Quad2D& operator=(const Quad2D& rhs) = delete;
};

} // namespace rendering_engine
