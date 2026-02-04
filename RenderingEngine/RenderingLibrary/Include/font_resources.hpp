// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include <cstdint>
#include <iostream>
#include <map>
#include <algorithm>

#include "boost/filesystem.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TYPES_H
#include FT_OUTLINE_H
#include FT_RENDER_H

#include "render_resource_context.hpp"
#include "rendering_engine_export.hpp"

namespace rendering_engine
{
/**
* @struct GlyphIndex
* @brief Opaque glyph identifier within a font.
*/
struct GlyphIndex
{
    std::uint32_t index;
};

/**
 * @struct GlyphMetrics
 * @brief Metrics describing a single glyph and its atlas placement.
 *
 * Contains layout information required for text positioning
 * and texture atlas sampling.
 */
struct GlyphMetrics
{
    // Atlas placement
    uint32_t atlasX = 0;
    uint32_t atlasY = 0;
    uint32_t width = 0;
    uint32_t height = 0;

    // Layout
    int32_t bearingX = 0;
    int32_t bearingY = 0;
    int32_t advanceX = 0;
    int32_t padding = 0;
};

/**
 * @struct FontMetrics
 * @brief Global metrics describing font vertical layout.
 */
struct FontMetrics
{
    int32_t lineHeight;
    int32_t ascender;
    int32_t descender;
};

class ImageData;
class TextRenderer;

/**
 * @class FontResources
 * @brief Manages glyph data, font atlases, and font-level metrics.
 *
 * Responsible for loading glyphs on demand, generating texture atlases,
 * and providing glyph and font metrics required for text layout and rendering.
 *
 * Instances are bound to a specific font and font size.
 */
class RE_API FontResources
{

public:
    /**
     * @brief Constructs font resources from a font file.
     * @param rrc Rendering resource context.
     * @param textRenderer Owning text renderer.
     * @param filepath Path to the font file.
     * @param fontSize Font size in pixels.
     */
	FontResources(RenderResourceContext rrc, TextRenderer* textRenderer, std::string filepath, unsigned int const fontSize);
    /**
     * @brief Constructs font resources from in-memory font data.
     * @param rrc Rendering resource context.
     * @param textRenderer Owning text renderer.
     * @param fontName Logical font name.
     * @param fileBytes Font file data.
     * @param fontSize Font size in pixels.
     */
    FontResources(RenderResourceContext rrc, TextRenderer* textRenderer, std::string fontName, std::vector<uint8_t> const& fileBytes, unsigned int const fontSize);

    /**
     * @brief Releases all font-related resources.
     */
	~FontResources();

    /**
     * @brief Preloads glyphs for a continuous Unicode range.
     * @param begin First Unicode code point.
     * @param end Last Unicode code point.
     */
    void LoadGlyphsFromCodePointRange(std::uint32_t begin, std::uint32_t end);

    /**
     * @brief Ensures glyphs for the given Unicode code points exist.
     * @param codePoints List of Unicode code points.
     */
    void EnsureGlyphs(const std::vector<std::uint32_t>& codePoints);

    /**
     * @brief Ensures glyphs for the given glyph indices exist.
     * @param glyphIndexes List of glyph indices.
     */
    void EnsureGlyphs(const std::vector<GlyphIndex>& glyphIndexes);

    /**
     * @brief Enables or disables storing generated font atlases on disk.
     * @param in True to store atlases in files.
     */
    void StoreFontAtlasesInFiles(bool in);

    /**
     * @brief Returns global font metrics.
     * @return Font metrics.
     */
    const FontMetrics& GetFontMetrics() const;

    /**
     * @brief Returns metrics for a specific glyph.
     * @param glyphIndex Glyph identifier.
     * @return Glyph metrics or default-initialized metrics if unavailable.
     */
    GlyphMetrics GetGlyphMetrics(GlyphIndex glyphIndex) const;

    /**
     * @brief Returns the texture name of the atlas containing the glyph.
     * @param glyphIndex Glyph identifier.
     * @return Texture name or empty string if unavailable.
     */
    std::string GetFontAtlasTextureName(GlyphIndex glyphIndex) const;

    /**
     * @brief Returns the material name associated with the glyph.
     * @param glyphIndex Glyph identifier.
     * @return Material name or empty string if unavailable.
     */
    std::string GetFontAtlasMaterialName(GlyphIndex glyphIndex) const;

    /**
     * @brief Resolves a Unicode code point to a glyph index.
     * @param codePoint Unicode code point.
     * @return Glyph index.
     */
    GlyphIndex GetIndexFromCodePoint(std::uint32_t codePoint) const;

    /**
     * @brief Returns the internal font face handle.
     * @note Intended for internal use only.
     */
    FT_Face GetFontFace();

protected:
    /**
     * @brief Creates a bitmap and metrics for a glyph by index.
     * @param glyphIndex Glyph identifier.
     * @return Pair of glyph metrics and image data.
     */
    std::pair<GlyphMetrics, ImageData> CreateGlyphBitmapBy(GlyphIndex glyphIndex);

    /**
     * @brief Creates a bitmap and metrics for a glyph by code point.
     * @param codePoint Unicode code point.
     * @return Pair of glyph metrics and image data.
     */
    std::pair<GlyphMetrics, ImageData> CreateGlyphBitmapBy(std::uint32_t codePoint);

    /**
     * @brief Creates a font atlas for a list of glyphs.
     * @param glyphIndexes List of glyph indices.
     */
    void CreateFontAtlasFromList(const std::vector<GlyphIndex>& glyphIndexes);

    /**
     * @brief Creates a font atlas for a continuous code point range.
     * @param begin First Unicode code point.
     * @param end Last Unicode code point.
     */
    void CreateFontAtlasFromRange(std::uint32_t begin, std::uint32_t end);

    /**
     * @brief Checks whether a glyph exists for a code point.
     * @param codePoint Unicode code point.
     * @return True if the glyph exists.
     */
    bool HasGlyph(uint32_t codePoint) const;

private:
	FontResources(FontResources const&);
	FontResources operator=(FontResources const&);

protected:
    RenderResourceContext mRenderResourceContext;
    TextRenderer* mTextRenderer;
    std::string mFontName;
    const unsigned int mFontSize;

	FT_Error   mErrorResult = FT_Err_Ok;
	FT_Face    mFace = 0;

    FontMetrics mFontMetrics;

    // Font atlases stored as unordered map where:
    // key - std::uint32_t - glyph's corresponding index
    // value - GlyphMetrics - metrics belong to particular glyph
    //       - std::string - material name, to which TextBlock can refer to.
    std::unordered_map<std::uint32_t, std::pair<GlyphMetrics, std::string>> mGlyphsByIndex;

    // Next container store names of created material name and corresponding font atlas textures name 
    std::unordered_map<std::string, std::string> mFontAtlases;

    std::vector<uint8_t> mFontFileBytes;

    bool bStoreFontAtlasesInFiles = false;

    /// Maximum number of glyphs per font atlas.
    static std::uint32_t sMaxGlyphsPerFontAtlas;
    /// Padding in pixels around glyphs inside atlases.
    static unsigned int sFontAtlasPaddingPx;
};
} // namespace rendering_engine
