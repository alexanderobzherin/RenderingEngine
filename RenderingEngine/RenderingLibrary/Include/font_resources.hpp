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

namespace rendering_engine
{
struct GlyphMetrics
{
    // Atlas placement
    uint32_t atlasX;
    uint32_t atlasY;
    uint32_t width;
    uint32_t height;

    // Layout
    int32_t bearingX;
    int32_t bearingY;
    int32_t advanceX;
};

struct FontMetrics
{
    int32_t lineHeight;
    int32_t ascender;
    int32_t descender;
};

class ImageData;

class FontResources
{

public:
	FontResources(RenderResourceContext rrc, std::string filepath, unsigned int const fontSize);
    FontResources(RenderResourceContext rrc, std::string fontName, std::vector<uint8_t> const& fileBytes, unsigned int const fontSize);
	~FontResources();

    void LoadGlyphsFromRange(std::uint32_t begin, std::uint32_t end);
    void EnsureGlyphs(std::vector<std::uint32_t> codePoints);
    void StoreFontAtlasesInFiles(bool in);

    const FontMetrics& GetFontMetrics() const;
    GlyphMetrics GetGlyphMetrics(std::uint32_t codePoint) const;
    std::string GetFontAtlasTextureName(std::uint32_t codePoint) const;
    std::string GetFontAtlasMaterialName(std::uint32_t codePoint) const;

protected:
    std::pair<GlyphMetrics, ImageData> CreateGlyphBitmap(std::uint32_t codePoint);
    void CreateFontAtlasFromList(std::vector<std::uint32_t> codePoints);
    void CreateFontAtlasFromRange(std::uint32_t begin, std::uint32_t end);

    bool HasGlyph(uint32_t codePoint) const;

private:
	FontResources(FontResources const&);
	FontResources operator=(FontResources const&);

protected:
    RenderResourceContext mRenderResourceContext;
    std::string mFontName;

	FT_Error   mErrorResult = FT_Err_Ok;
	FT_Library mLibrary = 0;
	FT_Face    mFace = 0;

    FontMetrics mFontMetrics;

    // Font atlases stored as unordered map where:
    // key - std::uint32_t - glyph's corresponding byte code
    // value - GlyphMetrics - metrics belong to particular glyph
    //       - std::string - material name, to which TextBlock can refer to.
    std::unordered_map<std::uint32_t, std::pair<GlyphMetrics, std::string>> mGlyphs;

    // Next container store names of created material name and corresponding font atlas textures name 
    std::unordered_map<std::string, std::string> mFontAtlases;

    bool bStoreFontAtlasesInFiles = false;
};
} // namespace rendering_engine
