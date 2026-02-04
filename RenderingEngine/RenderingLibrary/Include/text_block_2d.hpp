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
struct GlyphIndex;

enum class TextAlign
{
	Left = 0,
	Center,
	Right
};

class RE_API TextBlock2D : public Drawable2D 
{
public:
struct Properties
{
	std::string fontName;
	unsigned int fontSize = 10;
	TextAlign textAlign = TextAlign::Left;
	float maxLineLength = 0.0f;
	bool textShapeEnabled = false;
	float outlineThicknessPx = 0.0f;

	Properties()
	: fontName(TextBlock2D::sDefaultFontName)
    {}
};

public:
	TextBlock2D(std::shared_ptr<TextRenderer> textRenderer, Properties properties = Properties());

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

	virtual void SetText(std::string text);

	void SetTextColor(glm::vec4 color);
	void SetOutlineColor(glm::vec4 color);

	glm::vec2 GetDimensions() const;

protected:
struct Mesh
{
	std::vector<glm::vec2> positions2D;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec4> colors;
	std::vector<std::uint32_t> indices;
};

struct GlyphQuad
{
	float x0;
	float y0;
	float x1;
	float y1;

	float u0;
	float v0;
	float u1;
	float v1;

	std::string fontAtlasMaterialName;
	std::uint32_t advanceX;
};

struct ShapedGlyph
{
	std::uint32_t glyphIndex;   // FT glyph index (font-specific)
	std::uint32_t cluster;
	float xAdvance;        // pen movement AFTER this glyph
	float yAdvance;
	float xOffset;         // offset FROM current pen position
	float yOffset;
};

protected:
    std::vector<std::uint32_t> DecodeUtf8(const std::string& text);
	std::string CodepointToUtf8(std::uint32_t codePoint);

	std::vector<ShapedGlyph> ShapeText(const std::string& text);

	// Deprecated
	void ConstructMeshAutoLinebreak(const std::vector<std::uint32_t>& codePoints);

	void ConstructMesh();
	void ShapeTextAndConstructMesh();
	GlyphQuad MakeGlyphQuad(GlyphIndex glyphIndext, float penX, float penY);
	void PushQuad(std::string meshName,
		std::unordered_map<std::string, TextBlock2D::Mesh>& meshes,
		GlyphQuad glyphQuad,
		float horizontalShift = 0.0f);
	void UploadMeshes(const std::unordered_map<std::string, TextBlock2D::Mesh>& meshes);
	void SetOutlineThickness(float thicknessPx);

	template <typename T>
	std::unordered_map<std::string, TextBlock2D::Mesh> PrepareMeshSlots(const std::vector<T>& glyphs);

	std::vector<std::string> SplitString(const std::string& text, std::string separator);

	bool IsTextShapingRequired(std::uint32_t codePoint) const;

private:
	static std::uint64_t sNumOfTextBlocks;
	std::string mTextBlockID;

protected:
	/*Stores material used in string and corresponding name of created mesh*/
	std::unordered_map<std::string, std::string> mMaterialMesh;

	const std::shared_ptr<TextRenderer> mTextRenderer;
	std::shared_ptr<FontResources> mFontResources;
	glm::vec4 mColor;
	static std::string sDefaultFontName;
	const std::string mFontName;
	const unsigned int mFontSize;

	std::string mText;
	float mMaxLineLength = 0.0f;
	const TextAlign mTextAlign;
	glm::vec2 mDimensions;

	const bool bIsTextShapeEnabled;

	const float mOutlineThicknessPx;
};
}
