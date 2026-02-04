#include "font_resources.hpp"
#include "image_data.hpp"
#include "texture_atlas_maker.hpp"
#include "texture_cache.hpp"
#include "material_types.hpp"
#include "material_cache.hpp"
#include "material.hpp"
#include "utility.hpp"
#include "text_renderer.hpp"

#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

namespace rendering_engine
{
std::uint32_t FontResources::sMaxGlyphsPerFontAtlas = 1024;
unsigned int FontResources::sFontAtlasPaddingPx = 4;

FontResources::FontResources(RenderResourceContext rrc, TextRenderer* textRenderer, std::string filepath, unsigned int const fontSize)
	:
    mRenderResourceContext(rrc),
    mTextRenderer(textRenderer),
    mFontSize(fontSize),
	mErrorResult(FT_Err_Ok),
	mFace(0)
{
    mFontName = boost::filesystem::path(filepath).stem().string();

    mErrorResult = FT_New_Face(mTextRenderer->GetFontLibrary(), filepath.c_str(), 0, &mFace);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to create new face!" };
    }

    mErrorResult = FT_Set_Char_Size(mFace, mFontSize << 6, mFontSize << 6, 90, 90);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to set char size!" };
    }

    mFontMetrics.lineHeight = mFace->size->metrics.height >> 6;
    mFontMetrics.ascender = mFace->size->metrics.ascender >> 6;
    mFontMetrics.descender = mFace->size->metrics.descender >> 6;
}

FontResources::FontResources(RenderResourceContext rrc, TextRenderer* textRenderer, std::string fontName, std::vector<uint8_t> const& fileBytes, unsigned int const fontSize)
    :
    mRenderResourceContext(rrc),
    mTextRenderer(textRenderer),
    mErrorResult(FT_Err_Ok),
    mFace(0),
    mFontName(fontName),
    mFontSize(fontSize),
    mFontFileBytes(fileBytes)
{
    mErrorResult = FT_New_Memory_Face(mTextRenderer->GetFontLibrary(), mFontFileBytes.data(), static_cast<FT_Long>(mFontFileBytes.size()), 0, &mFace);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to create new face!" };
    }

    mErrorResult = FT_Set_Char_Size(mFace, mFontSize << 6, mFontSize << 6, 90, 90);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to set char size!" };
    }

    mFontMetrics.lineHeight = mFace->size->metrics.height >> 6;
    mFontMetrics.ascender = mFace->size->metrics.ascender >> 6;
    mFontMetrics.descender = mFace->size->metrics.descender >> 6;
}

FontResources::~FontResources()
{
	FT_Done_Face(mFace);
}

void FontResources::LoadGlyphsFromCodePointRange(std::uint32_t begin, std::uint32_t end)
{
    auto distance = end - begin;
    std::uint32_t start = begin;
    while (start < end) 
    {
        std::uint32_t currentEnd = std::min(start + sMaxGlyphsPerFontAtlas - 1, end);
        CreateFontAtlasFromRange(start, currentEnd);
        start += sMaxGlyphsPerFontAtlas;
    } 
}

void FontResources::EnsureGlyphs(const std::vector<std::uint32_t>& codePoints)
{
    if (codePoints.empty())
        return;

    std::vector<GlyphIndex> glyphs;
    for (auto codePoint : codePoints)
    {
        glyphs.push_back(GetIndexFromCodePoint(codePoint));
    }

    EnsureGlyphs(glyphs);
}

void FontResources::EnsureGlyphs(const std::vector<GlyphIndex>& glyphIndexes)
{
    if (glyphIndexes.empty())
        return;

    std::vector<GlyphIndex> lackingGlyphs;
    for (const auto& glyphIndex : glyphIndexes)
    {
        auto found = mGlyphsByIndex.find(glyphIndex.index);
        if (found == mGlyphsByIndex.end())
        {
            GlyphIndex gi;
            gi.index = glyphIndex.index;
            lackingGlyphs.push_back(gi);
        }
    }
    if (!lackingGlyphs.empty())
    {
        CreateFontAtlasFromList(lackingGlyphs);
    }
}

void FontResources::StoreFontAtlasesInFiles(bool in)
{
    bStoreFontAtlasesInFiles = in;
}

std::pair<GlyphMetrics, ImageData> FontResources::CreateGlyphBitmapBy(GlyphIndex glyphIndex)
{
    mErrorResult = FT_Load_Glyph(mFace, glyphIndex.index, FT_LOAD_DEFAULT);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to load glyph!" };
    }

    mErrorResult = FT_Render_Glyph(mFace->glyph, FT_RENDER_MODE_NORMAL);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to render glyph!" };
    }

    const unsigned padding = sFontAtlasPaddingPx;

    const auto srcW = mFace->glyph->bitmap.width;
    const auto srcH = mFace->glyph->bitmap.rows;

    const auto dstW = srcW + padding * 2;
    const auto dstH = srcH + padding * 2;

    const auto bufferSize = dstW * dstH * 4;
    std::vector<uint8_t> buffer(bufferSize, 0);
    for (int y = 0; y < srcH; ++y)
    {
        for (int x = 0; x < srcW; ++x)
        {
            uint8_t coverage =
                mFace->glyph->bitmap.buffer[y * srcW + x];

            int dstX = x + padding;
            int dstY = y + padding;

            const size_t idx = (static_cast<size_t>(dstY) * static_cast<size_t>(dstW)
                    + static_cast<size_t>(dstX)) * 4u;

            buffer[idx + 0] = coverage;
            buffer[idx + 1] = coverage;
            buffer[idx + 2] = coverage;
            buffer[idx + 3] = coverage;
        }
    }
    GlyphMetrics glyphMetrics;
    glyphMetrics.width =  srcW;
    glyphMetrics.height = srcH;


    glyphMetrics.bearingX = mFace->glyph->bitmap_left;
    glyphMetrics.bearingY = mFace->glyph->bitmap_top;

    glyphMetrics.advanceX = mFace->glyph->advance.x >> 6;

    glyphMetrics.padding = padding;

    return std::pair(glyphMetrics, ImageData(dstW, dstH, buffer));
}

std::pair<GlyphMetrics, ImageData> FontResources::CreateGlyphBitmapBy(std::uint32_t codePoint)
{
    GlyphIndex glyphIndex;
    glyphIndex.index = static_cast<std::uint32_t>(FT_Get_Char_Index(mFace, static_cast<FT_ULong>(codePoint)));

    return CreateGlyphBitmapBy(glyphIndex);
}

void FontResources::CreateFontAtlasFromRange(std::uint32_t begin, std::uint32_t end)
{
    if (begin > end)
    {
        // range is not valid
        return;
    }

    // 1. Create font atlas in range of [begin; end]
    std::unordered_map<std::uint32_t, std::pair<GlyphMetrics, ImageData>> glyphs;
    for (auto codePoint = begin; codePoint <= end; ++codePoint)
    {
        if (!HasGlyph(codePoint))
        {
            continue;
        }
        // Creating bitmaps of glyphs should be considered to dome multythreaded
        glyphs[codePoint] = CreateGlyphBitmapBy(codePoint);
    }
    if (glyphs.empty())
        return;

    auto fontAtlas = TextureAtlasMaker::CreateTextureAtlas(glyphs);

    // 2. Upload texture via TextureCache

    auto textureCache = mRenderResourceContext.textureCache;
    const std::string textureName = mFontName + "_" + std::to_string(mFontSize) + "_FontAtlas_" + std::to_string(mFontAtlases.size());
    if (bStoreFontAtlasesInFiles)
    {
        const std::string fileName = textureName + ".png";
        auto fullPath = Utility::GetContentFolderPath() / fileName;
        fontAtlas.WritePngFile(fullPath.string().c_str());
    }
    textureCache->LoadTexture(textureName, fontAtlas);

    // 3. Create a material via MaterialCache and add texture to it
    auto materialCache = mRenderResourceContext.materialCache;
    const std::string materialName = textureName + "_Mat";
    MaterialSettings materialSettings;
    materialSettings.parentMaterialName = std::string{ "Font2D" };
    materialSettings.materialName = materialName;
    materialSettings.materialDomain = MaterialDomain::Sprite2D;
    materialSettings.shadingModel = ShadingModel::Unlit;
    materialSettings.blendMode = BlendMode::Translucent;
    materialSettings.parameterLayout = &Font2DLayout;

    materialCache->AddMaterial(materialSettings);
    Material* material = materialCache->GetMaterial(materialName);
    material->SetVec4("FontColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    material->SetVec4("OutlineColor", glm::vec4(0, 0, 0, 1));
    material->SetFloat("OutlineThicknessPx", 0.0); // OFF by default
    const float invW = 1.0f / static_cast<float>(fontAtlas.GetWidth());
    const float invH = 1.0f / static_cast<float>(fontAtlas.GetHeight());
    material->SetFloat("InvAtlasSizeWidth", invW);
    material->SetFloat("InvAtlasSizeHeight", invH);

    material->AddTexture(textureName);

    // material->SetFloat >> Add custom material parameters for UV manipulation
    material->InitializeRenderResources();

    // 4 Upload to container mFontAtlases
    for (auto& it : glyphs)
    {
        const std::uint32_t glyphIndex = static_cast<std::uint32_t>(FT_Get_Char_Index(mFace, static_cast<FT_ULong>(it.first)));
        mGlyphsByIndex[glyphIndex] = std::pair<GlyphMetrics, std::string>(it.second.first, materialName);
    }

    mFontAtlases[materialName] = textureName;
}

bool FontResources::HasGlyph(uint32_t codePoint) const
{
    return FT_Get_Char_Index(mFace, codePoint) != 0;
}

const FontMetrics& FontResources::GetFontMetrics() const
{
    return mFontMetrics;
}

GlyphMetrics FontResources::GetGlyphMetrics(GlyphIndex glyphIndex) const
{
    auto search = mGlyphsByIndex.find(glyphIndex.index);
    if (search == mGlyphsByIndex.end())
    {
        return GlyphMetrics();
    }
    return search->second.first;
}

std::string FontResources::GetFontAtlasTextureName(GlyphIndex glyphIndex) const
{
    auto matName = GetFontAtlasMaterialName(glyphIndex);
    if (matName.empty())
    {
        return std::string();
    }
    auto search = mFontAtlases.find(matName);
    if(search == mFontAtlases.end())
    {
        return std::string();
    }
    return search->second;
}

std::string FontResources::GetFontAtlasMaterialName(GlyphIndex glyphIndex) const
{
    auto search = mGlyphsByIndex.find(glyphIndex.index);
    if (search == mGlyphsByIndex.end())
    {
        return std::string();
    }
    return search->second.second;
}

GlyphIndex FontResources::GetIndexFromCodePoint(std::uint32_t codePoint) const
{
    GlyphIndex result;
    result.index = FT_Get_Char_Index(mFace, codePoint);
    return result;
}

FT_Face FontResources::GetFontFace()
{
    return mFace;
}

void FontResources::CreateFontAtlasFromList(const std::vector<GlyphIndex>& glyphIndexes)
{
    // 1. Create font atlas from list
    std::unordered_map<std::uint32_t, std::pair<GlyphMetrics, ImageData>> glyphs;
    for (auto glyphIndex : glyphIndexes)
    {
        auto found = mGlyphsByIndex.find(glyphIndex.index);
        if (found != mGlyphsByIndex.end())
            continue;
        // Creating bitmaps of glyphs should be considered to dome multithreaded
        glyphs[glyphIndex.index] = CreateGlyphBitmapBy(glyphIndex);
    }
    if (glyphs.empty())
        return;

    auto fontAtlas = TextureAtlasMaker::CreateTextureAtlas(glyphs);

    // 2. Upload texture via TextureCache

    auto textureCache = mRenderResourceContext.textureCache;
    const std::string textureName = mFontName + "_" + std::to_string(mFontSize) + "_FontAtlas_" + std::to_string(mFontAtlases.size());
    if (bStoreFontAtlasesInFiles)
    {
        const std::string fileName = textureName + ".png";
        auto fullPath = Utility::GetContentFolderPath() / fileName;
        fontAtlas.WritePngFile(fullPath.string().c_str());
    }
    textureCache->LoadTexture(textureName, fontAtlas);

    // 3. Create a material via MaterialCache and add texture to it
    auto materialCache = mRenderResourceContext.materialCache;
    const std::string materialName = textureName + "_Mat";
    MaterialSettings materialSettings;
    materialSettings.parentMaterialName = std::string{ "Font2D" };
    materialSettings.materialName = materialName;
    materialSettings.materialDomain = MaterialDomain::Sprite2D;
    materialSettings.shadingModel = ShadingModel::Unlit;
    materialSettings.blendMode = BlendMode::Translucent;
    materialSettings.parameterLayout = &Font2DLayout;

    materialCache->AddMaterial(materialSettings);
    Material* material = materialCache->GetMaterial(materialName);
    material->SetVec4("FontColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    material->SetVec4("OutlineColor", glm::vec4(0, 0, 0, 1));
    material->SetFloat("OutlineThicknessPx", 0.0f); // OFF by default
    const float invW = 1.0f / static_cast<float>(fontAtlas.GetWidth());
    const float invH = 1.0f / static_cast<float>(fontAtlas.GetHeight());
    material->SetFloat("InvAtlasSizeWidth", invW);
    material->SetFloat("InvAtlasSizeHeight", invH);

    material->AddTexture(textureName);

    // material->SetFloat >> Add custom material parameters for UV manipulation
    material->InitializeRenderResources();

    // 4 Upload to container mFontAtlases
    for (auto& it : glyphs)
    {
        mGlyphsByIndex[it.first] = std::pair<GlyphMetrics, std::string>(it.second.first, materialName);
    }

    mFontAtlases[materialName] = textureName;
}

} // namespace rendering_engine