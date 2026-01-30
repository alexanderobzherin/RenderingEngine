// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "render_resource_context.hpp"
#include "rendering_engine_export.hpp"
#include "utility.hpp"

#include <memory>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TYPES_H
#include FT_OUTLINE_H
#include FT_RENDER_H

namespace rendering_engine
{
class FontResources;

class RE_API TextRenderer
{
public:
    TextRenderer(RenderResourceContext rrc);
    ~TextRenderer();

    void LoadFontsFromFolder(std::string pathToFolder);
    void LoadFontsFromPackage();
    const RenderResourceContext& GetRenderResourceContext() const;

    std::shared_ptr<FontResources> GetFontResources(const std::string& fontName, int fontSize);

    void StoreFontAtlasesInFiles(bool in);

    const std::vector<std::string>& GetScriptsRequiredShaping() const;
    std::pair<std::uint32_t, std::uint32_t> GetScriptRange(std::string script);
    inline FT_Library& GetFontLibrary()
    {
        return mLibrary;
    }

protected:
    void LoadFontsAvailableInFolder(std::string pathToFolder);
    void LoadPreloadableFontAtlasesFromFolder(const std::unordered_map<std::string, std::string>& availableFontsInFolder);

    void LoadFontsAvailableInPackage();
    void LoadPreloadableFontAtlasesFromPackage(const std::unordered_map<std::string, std::string>& availableFontsInPackage);
private:
    struct PairHash
    {
        std::size_t operator()(const std::pair<std::string, int>& p) const {
            return std::hash<std::string>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
        }
    };

    FT_Library mLibrary = 0;
    FT_Error   mErrorResult = FT_Err_Ok;

    RenderResourceContext mRenderResourceContext;

    // Store name of fonts avalable from font's folder
    std::unordered_map<std::string, std::string> mAvailableFontsInFolder;
    std::unordered_map<std::string, std::string> mAvailableFontsInPackage;

    std::unordered_map<std::pair<std::string, int>, std::shared_ptr<FontResources>, PairHash> mFontResources;

    bool bStoreFontAtlasesInFiles = false;

    // Script - <Begin, End>
    static std::unordered_map<std::string, std::pair<std::uint32_t, std::uint32_t>> sScriptRanges;
    static std::vector<std::string> sScriptsRequiresShaping;
    static std::vector<std::string> sFontAtlasPreloadableScripts;
};

} // namespace rendering_engine

