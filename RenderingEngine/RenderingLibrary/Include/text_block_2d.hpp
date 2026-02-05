// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
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
class Scene;
class TextRenderer;
class FontResources;
struct GlyphIndex;

/**
 * @enum TextAlign
 * @brief Horizontal alignment mode for text layout.
 */
enum class TextAlign
{
	Left = 0,
	Center,
	Right
};

/**
 * @class TextBlock2D
 * @brief 2D drawable representing a block of rendered text.
 *
 * Converts UTF-8 text into glyph quads, performs layout and alignment,
 * and submits render batches using font atlas materials.
 *
 * A TextBlock2D instance is bound to a single font and font size.
 */
class RE_API TextBlock2D : public Drawable2D 
{
public:
/**
* @struct TextBlock2D::Properties
* @brief Configuration parameters for a text block.
*/
struct Properties
{
	/// Font family name.
	std::string fontName;
	/// Font size in pixels.
	unsigned int fontSize = 10;
	/// Horizontal text alignment.
	TextAlign textAlign = TextAlign::Left;
	/// Maximum line length for layout; zero disables wrapping.
	float maxLineLength = 0.0f;
	/// Enables text shaping during layout.
	bool textShapeEnabled = false;
	/// Outline thickness in pixels.
	float outlineThicknessPx = 0.0f;

	Properties()
	: fontName(TextBlock2D::sDefaultFontName)
    {}
};

public:
	/**
	 * @brief Constructs a 2D text block.
	 * @param textRenderer Owning text renderer.
	 * @param properties Text block configuration.
	 */
	TextBlock2D(Scene& scene, std::shared_ptr<TextRenderer> textRenderer, Properties properties = Properties());

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
	 * @brief Sets the displayed text.
	 * @param text UTF-8 encoded text string.
	 */
	virtual void SetText(std::string text);
	/**
	 * @brief Sets the text color.
	 * @param color RGBA color.
	 */
	void SetTextColor(glm::vec4 color);
	/**
	 * @brief Sets the outline color.
	 * @param color RGBA color.
	 */
	void SetOutlineColor(glm::vec4 color);
	/**
	 * @brief Returns text block dimensions.
	 * @return Width and height in pixels.
	 */
	glm::vec2 GetDimensions() const;

protected:
/**
* @struct Mesh
* @brief CPU-side mesh data for glyph quads.
*/
struct Mesh
{
	std::vector<glm::vec2> positions2D;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec4> colors;
	std::vector<std::uint32_t> indices;
};
/**
 * @struct GlyphQuad
 * @brief Renderable quad representing a single glyph.
 */
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
/**
 * @struct ShapedGlyph
 * @brief Result of text shaping for a single glyph.
 *
 * Contains glyph index, pen offsets, and advance values.
 */
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
	/**
	 * @brief Decodes a UTF-8 string into Unicode code points.
	 * @param text UTF-8 encoded string.
	 * @return List of Unicode code points.
	 */
    std::vector<std::uint32_t> DecodeUtf8(const std::string& text);

	/**
	 * @brief Converts a Unicode code point to UTF-8.
	 * @param codePoint Unicode code point.
	 * @return UTF-8 encoded string.
	 */
	std::string CodepointToUtf8(std::uint32_t codePoint);

	/**
	 * @brief Shapes text and returns shaped glyphs.
	 * @param text UTF-8 encoded text.
	 * @return List of shaped glyphs.
	 */
	std::vector<ShapedGlyph> ShapeText(const std::string& text);

	// Deprecated
	void ConstructMeshAutoLinebreak(const std::vector<std::uint32_t>& codePoints);

	/**
	 * @brief Constructs glyph geometry without shaping.
	 */
	void ConstructMesh();

	/**
	 * @brief Shapes text and constructs glyph geometry.
	 */
	void ShapeTextAndConstructMesh();
	/**
	 * @brief Creates a glyph quad at the current pen position.
	 * @param glyphIndex Glyph identifier.
	 * @param penX Horizontal pen position.
	 * @param penY Vertical pen position.
	 * @return Glyph quad.
	 */
	GlyphQuad MakeGlyphQuad(GlyphIndex glyphIndext, float penX, float penY);
	/**
	 * @brief Appends a glyph quad to a mesh.
	 * @param meshName Target mesh name.
	 * @param meshes Mesh container.
	 * @param glyphQuad Glyph quad data.
	 * @param horizontalShift Alignment offset.
	 */
	void PushQuad(std::string meshName,
		std::unordered_map<std::string, TextBlock2D::Mesh>& meshes,
		GlyphQuad glyphQuad,
		float horizontalShift = 0.0f);
	/**
	 * @brief Uploads prepared meshes to GPU resources.
	 * @param meshes CPU-side mesh data.
	 */
	void UploadMeshes(const std::unordered_map<std::string, TextBlock2D::Mesh>& meshes);
	/**
	 * @brief Sets outline thickness for all render batches.
	 * @param thicknessPx Thickness in pixels.
	 */
	void SetOutlineThickness(float thicknessPx);
	/**
	 * @brief Prepares mesh slots based on glyph usage.
	 * @param glyphs Glyph collection.
	 * @return Map of mesh names to mesh data.
	 */
	template <typename T>
	std::unordered_map<std::string, TextBlock2D::Mesh> PrepareMeshSlots(const std::vector<T>& glyphs);
	/**
	 * @brief Splits a string by a separator.
	 * @param text Input string.
	 * @param separator Separator string.
	 * @return List of substrings.
	 */
	std::vector<std::string> SplitString(const std::string& text, std::string separator);
	/**
	 * @brief Checks whether shaping is required for a code point.
	 * @param codePoint Unicode code point.
	 * @return True if shaping is required.
	 */
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
