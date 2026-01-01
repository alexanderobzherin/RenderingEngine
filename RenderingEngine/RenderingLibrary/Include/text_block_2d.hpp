// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "drawable_2d.hpp"
#include "rendering_engine_export.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>

#include <glm/glm.hpp>

namespace rendering_engine
{
/*
penX, penY  - current cursor (baseline)
x = penX + bearingX
y = penY - bearingY
width, height - quad size
penX += advanceX
*/
class TextRenderer;

class RE_API TextBlock2D : public Drawable2D 
{
public:
	TextBlock2D(std::shared_ptr<TextRenderer> textRenderer, std::string fontName);

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

	void SetFont(std::string fontName);
	void SetText(std::string text);
	void SetTextColor(glm::vec4 color);

	void DrawFontAtlas();

protected:
    std::vector<std::uint32_t> DecodeUtf8(const std::string& text);

	std::shared_ptr<TextRenderer> mTextRenderer;
	glm::vec4 mColor;
	std::string mFontName;

	std::string mText;
};
}
