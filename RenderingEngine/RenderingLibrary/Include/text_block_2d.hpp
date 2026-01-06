// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "drawable_2d.hpp"
#include "rendering_engine_export.hpp"
#include "vertex_declarations.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
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
class FontResources;

class RE_API TextBlock2D : public Drawable2D 
{
struct Mesh
{
	std::vector<glm::vec2> positions2D;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec4> colors;
	std::vector<std::uint32_t> indices;
};
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

	void SetText(std::string text);
	void SetTextColor(glm::vec4 color);

	void DrawFontAtlas();

protected:
	void SetFont(std::string fontName);
    std::vector<std::uint32_t> DecodeUtf8(const std::string& text);

	void ConstructMesh(const std::vector<std::uint32_t>& byteCodes);

	std::string CodepointToUtf8(std::uint32_t codePoint);

private:
	static std::uint64_t sNumOfTextBlocks;
	std::string mTextBlockID;

	/*Stores material used in string and corresponding name of created mesh*/
	std::unordered_map<std::string, std::string> mMaterialMesh;

	std::shared_ptr<TextRenderer> mTextRenderer;
	std::shared_ptr<FontResources> mFontResources;
	glm::vec4 mColor;
	std::string mFontName;

	std::string mText;

};
}
