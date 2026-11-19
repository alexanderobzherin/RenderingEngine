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
 * @brief Drawable 2D sprite rendered using a texture.
 *
 * The sprite's initial dimensions are determined by its
 * texture size.
 *
 * @note Not copyable or assignable.
 * @see Drawable2D, Camera2D
 */
class RE_API Sprite2D : public Drawable2D
{
public:
	/**
	 * @brief Constructs a textured 2D sprite.
	 *
	 * @param renderContext Rendering resource context.
	 * @param scene Scene that owns the sprite.
	 * @param textureName Name of the texture resource.
	 */
	Sprite2D(RenderResourceContext renderContext, Scene& scene, std::string textureName);

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
	 * @brief Sets a uniform scale relative to the sprite's texture size.
	 *
	 * The scale factor is applied to the original texture dimensions,
	 * allowing convenient uniform resizing of the sprite.
	 *
	 * @param scale Uniform scale factor (1.0 = original texture size).
	 */
	void SetSpriteScale(float scale);

	/**
	 * @brief Sets the sprite color multiplier.
	 *
	 * The supplied RGBA color is multiplied by the
	 * sampled texture color.
	 *
	 * White (1, 1, 1, 1) preserves the original texture.
	 *
	 * @param color Color multiplier in RGBA components.
	 */
	void SetColor(glm::vec4 color);

	Sprite2D(const Sprite2D& rhs) = delete;
	Sprite2D& operator=(const Sprite2D& rhs) = delete;

protected:
	std::string mTextureName;
	glm::vec2 mTextureRespectiveScale;
};

} // namespace rendering_engine
