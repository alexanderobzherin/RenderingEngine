// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "render_resource_context.hpp"

#include <memory>
#include <unordered_map>
#include <cstdint>
#include <string>

namespace rendering_engine
{
class FontResources;

class TextRenderer
{
public:
    TextRenderer(RenderResourceContext rrc);
    ~TextRenderer();

    void LoadFontsFromFolder(std::string pathToFolder);
    void LoadFontsFromPackage();
    const RenderResourceContext& GetRenderResourceContext() const;

    std::shared_ptr<FontResources> GetFontResources(std::string fontName);

    void StoreFontAtlasesInFiles(bool in);

private:
    RenderResourceContext mRenderResourceContext;

    std::unordered_map<std::string, std::shared_ptr<FontResources>> mFontResources;

    bool bStoreFontAtlasesInFiles = false;

    // Script - <Begin, End>
    static std::unordered_map<std::string, std::pair<std::uint32_t, std::uint32_t>> sScriptRanges;
};

} // namespace rendering_engine

/*
latin	    Basic Latin + Latin Extended
cyrillic	Cyrillic
greek	    Greek

hebrew	    Hebrew (RTL)
arabic	    Arabic (RTL + shaping)
aramaic

han	        CJK ideographs
hiragana	Japanese Hiragana
katakana	Japanese Katakana
hangul	    Korean Hangul

*/