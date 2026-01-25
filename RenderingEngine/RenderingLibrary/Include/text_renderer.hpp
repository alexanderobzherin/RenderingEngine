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
#include <vector>

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
    inline bool IsTextShapingEnabled()
    {
        return bTextShapingEnabled;
    }

    std::vector<std::string> GetScriptsRequiredShaping();
    std::pair<std::uint32_t, std::uint32_t> GetScriptRange(std::string script);

private:
    RenderResourceContext mRenderResourceContext;

    std::unordered_map<std::string, std::shared_ptr<FontResources>> mFontResources;

    bool bStoreFontAtlasesInFiles = false;

    bool bTextShapingEnabled = false;

    // Script - <Begin, End>
    static std::unordered_map<std::string, std::pair<std::uint32_t, std::uint32_t>> sScriptRanges;
    static std::vector<std::string> sScriptsRequiresShaping;
    static std::vector<std::uint32_t> sFontSizesPreload;
};

} // namespace rendering_engine

