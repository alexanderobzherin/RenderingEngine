// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
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

/**
 * @class TextRenderer
 * @brief Central text system manager and font resource registry.
 *
 * Owns the font backend instance, manages font discovery and caching,
 * defines script metadata, and coordinates font atlas preloading
 * based on application configuration.
 */
class RE_API TextRenderer
{
public:
    /**
     * @brief Constructs the text renderer and initializes internal font systems.
     * @param rrc Rendering resource context.
     *
     * This object owns the lifetime of the font backend and must outlive
     * all font resources created through it.
     */
    TextRenderer(RenderResourceContext rrc);
    /**
     * @brief Shuts down the text system and releases font backend resources.
     *
     * All FontResources obtained from this renderer must be destroyed
     * before the renderer itself.
     */
    ~TextRenderer();
    /**
     * @brief Loads fonts from a filesystem folder.
     * @param pathToFolder Path to the folder containing font files.
     */
    void LoadFontsFromFolder(std::string pathToFolder);
    /**
     * @brief Loads fonts bundled inside an application package.
     */
    void LoadFontsFromPackage();
    /**
     * @brief Returns the rendering resource context.
     * @return Reference to the rendering resource context.
     */
    const RenderResourceContext& GetRenderResourceContext() const;
    /**
     * @brief Retrieves or creates font resources for a font and size.
     *
     * If the requested font resources are not yet loaded, they will be
     * created on demand from available font sources.
     *
     * @param fontName Font family name.
     * @param fontSize Font size in pixels.
     * @return Shared pointer to font resources, or nullptr if unavailable.
     */
    std::shared_ptr<FontResources> GetFontResources(const std::string& fontName, int fontSize);
    /**
     * @brief Enables or disables storing generated font atlases to files.
     * @param in True to store atlases on disk.
     */
    void StoreFontAtlasesInFiles(bool in);
    /**
     * @brief Returns scripts that require shaping before rendering.
     * @return List of script identifiers.
     */
    const std::vector<std::string>& GetScriptsRequiredShaping() const;
    /**
     * @brief Returns the Unicode codepoint range for a script.
     * @param script Script identifier.
     * @return Pair of <begin, end> Unicode codepoints.
     */
    std::pair<std::uint32_t, std::uint32_t> GetScriptRange(std::string script);
    /**
     * @brief Returns the internal FreeType library instance.
     * @return Reference to FT_Library.
     */
    inline FT_Library& GetFontLibrary()
    {
        return mLibrary;
    }

protected:
    /**
     * @brief Discovers fonts available in a filesystem folder.
     * @param pathToFolder Path to the font folder.
     */
    void LoadFontsAvailableInFolder(std::string pathToFolder);
    /**
     * @brief Loads preloadable font atlases from a folder.
     * @param availableFontsInFolder Map of font names to file paths.
     */
    void LoadPreloadableFontAtlasesFromFolder(const std::unordered_map<std::string, std::string>& availableFontsInFolder);
    /**
     * @brief Discovers fonts available in the application package.
     */
    void LoadFontsAvailableInPackage();
    /**
     * @brief Loads preloadable font atlases from the application package.
     * @param availableFontsInPackage Map of font names to package paths.
     */
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

    /**
     * @brief Unicode script ranges known to the text system.
     *
     * Used for script detection, shaping decisions, and glyph preloading.
     */
    static std::unordered_map<std::string, std::pair<std::uint32_t, std::uint32_t>> sScriptRanges;
    /**
     * @brief Scripts that require text shaping during layout.
     */
    static std::vector<std::string> sScriptsRequiresShaping;
    /**
     * @brief Scripts whose glyphs can be safely preloaded without shaping.
     */
    static std::vector<std::string> sFontAtlasPreloadableScripts;
};

} // namespace rendering_engine

