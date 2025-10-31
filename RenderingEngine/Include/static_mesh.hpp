// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "drawable_3d.hpp"

namespace rendering_engine
{
/**
 * @class StaticMesh
 * @brief 3D drawable component for rendering static (non-animated) meshes.
 *
 * @note Not copyable or assignable.
 * @see Drawable3D, DrawableComponent, SceneComponent, Camera
 */
class StaticMesh : public Drawable3D
{
public:
	/**
	 * @brief Constructs the StaticMesh with a render context.
	 * @param renderContext Resource context.
	 */
	StaticMesh(RenderResourceContext renderContext);

	/**
	 * @copydoc DrawableComponent::Initialize
	 */
	void Initialize() override;

	/**
	 * @copydoc DrawableComponent::Update
	 */
	void Update(float deltaTime) override;

	/**
	 * @copydoc Drawable3D::Draw
	 */
	void Draw(const Camera& camera) override;

	using DrawableComponent::SetMaterialName; ///< @copydoc DrawableComponent::SetMaterialName
	using DrawableComponent::SetMeshName; ///< @copydoc DrawableComponent::SetMeshName

	StaticMesh(const StaticMesh& rhs) = delete;
	StaticMesh& operator=(const StaticMesh& rhs) = delete;
};


} // namespace rendering_engine
