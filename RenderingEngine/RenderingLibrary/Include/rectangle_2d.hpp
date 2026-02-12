// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "drawable_2d.hpp"

namespace rendering_engine
{
class Scene;
   /**
	* @class Rectangle2D
	* @brief 2D drawable component for rendering rectangle.
	*
	* @note Not copyable or assignable.
	* @see Sprite2D, Quad2D, Drawable2D, DrawableComponent, SceneComponent2D, Camera2D
	*/
class RE_API Rectangle2D : public Drawable2D
{
public:
struct Properties
{
	glm::vec2 size = {50.0f, 50.0f};
	glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
};
public:
	/**
	* @brief Constructs the Rectangle2D with a render context.
	* @param renderContext Resource context.
	*
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
	* @brief Sets color.
	*
	* @param color RGBA [0.0f - 1.0f].
	*/
	void SetColor(glm::vec4 color);

	Rectangle2D(const Rectangle2D& rhs) = delete;
	Rectangle2D& operator=(const Rectangle2D& rhs) = delete;

protected:
	Properties mProperties;

};

} // namespace rendering_engine
