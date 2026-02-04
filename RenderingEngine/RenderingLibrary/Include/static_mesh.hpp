// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "drawable_3d.hpp"

#include <unordered_map>
#include <string>
#include <cstdint>

namespace rendering_engine
{
struct StaticMeshParams
{
	std::string meshName;

	// {slot number, material name}
	std::unordered_map<uint32_t, std::string> materials;

	bool castShadows = false;
	bool receiveShadows = false;
};

/**
 * @class StaticMesh
 * @brief 3D drawable component for rendering static (non-animated) meshes.
 *
 * @note Not copyable or assignable.
 * @see Drawable3D, DrawableComponent, SceneComponent, Camera
 */
class RE_API StaticMesh : public Drawable3D
{
public:
	/**
	 * @brief Constructs a StaticMesh component associated with a given render context.
	 * @param renderContext Rendering resource context for material and mesh initialization.
	 */
	StaticMesh(RenderResourceContext renderContext, StaticMeshParams params);

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

	StaticMesh(const StaticMesh& rhs) = delete;
	StaticMesh& operator=(const StaticMesh& rhs) = delete;

protected:
	StaticMeshParams mParams;
};


} // namespace rendering_engine
