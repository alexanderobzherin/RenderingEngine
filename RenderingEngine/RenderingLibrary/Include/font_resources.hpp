// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
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
struct GlyphIndex
{
    std::uint32_t index;
};

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

struct FontMetrics
{
    int32_t lineHeight;
    int32_t ascender;
    int32_t descender;
};

class ImageData;
class TextRenderer;

class RE_API FontResources
{

public:
	FontResources(RenderResourceContext rrc, TextRenderer* textRenderer, std::string filepath, unsigned int const fontSize);
    FontResources(RenderResourceContext rrc, TextRenderer* textRenderer, std::string fontName, std::vector<uint8_t> const& fileBytes, unsigned int const fontSize);
	~FontResources();

    void LoadGlyphsFromCodePointRange(std::uint32_t begin, std::uint32_t end);
    void EnsureGlyphs(const std::vector<std::uint32_t>& codePoints);
    void EnsureGlyphs(const std::vector<GlyphIndex>& glyphIndexes);
    void StoreFontAtlasesInFiles(bool in);

    const FontMetrics& GetFontMetrics() const;
    GlyphMetrics GetGlyphMetrics(GlyphIndex glyphIndex) const;
    std::string GetFontAtlasTextureName(GlyphIndex glyphIndex) const;
    std::string GetFontAtlasMaterialName(GlyphIndex glyphIndex) const;
    GlyphIndex GetIndexFromCodePoint(std::uint32_t codePoint) const;
    FT_Face GetFontFace();

protected:
    std::pair<GlyphMetrics, ImageData> CreateGlyphBitmapBy(GlyphIndex glyphIndex);
    std::pair<GlyphMetrics, ImageData> CreateGlyphBitmapBy(std::uint32_t codePoint);
    void CreateFontAtlasFromList(const std::vector<GlyphIndex>& glyphIndexes);
    void CreateFontAtlasFromRange(std::uint32_t begin, std::uint32_t end);

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
    static std::uint32_t sMaxGlyphsPerFontAtlas;
    static unsigned int sFontAtlasPaddingPx;

};
} // namespace rendering_engine
