#include "font_resources.hpp"
#include "image_data.hpp"
#include "texture_atlas_maker.hpp"
#include "texture_cache.hpp"
#include "material_types.hpp"
#include "material_cache.hpp"
#include "material.hpp"
#include "utility.hpp"

namespace rendering_engine
{

FontResources::FontResources(RenderResourceContext rrc, std::string filepath, unsigned int const fontSize)
	:
    mRenderResourceContext(rrc),
	mErrorResult(FT_Err_Ok),
	mLibrary(0),
	mFace(0)
{
    mFontName = boost::filesystem::path(filepath).stem().string();
    mErrorResult = FT_Init_FreeType(&mLibrary);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to initialize FreeType library!" };
    }

    mErrorResult = FT_New_Face(mLibrary, filepath.c_str(), 0, &mFace);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to create new face!" };
    }

    mErrorResult = FT_Set_Char_Size(mFace, fontSize << 6, fontSize << 6, 90, 90);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to set char size!" };
    }

    mFontMetrics.lineHeight = mFace->size->metrics.height >> 6;
    mFontMetrics.ascender = mFace->size->metrics.ascender >> 6;
    mFontMetrics.descender = mFace->size->metrics.descender >> 6;
}

FontResources::FontResources(RenderResourceContext rrc, std::string fontName, std::vector<uint8_t> const& fileBytes, unsigned int const fontSize)
    :
    mRenderResourceContext(rrc),
    mErrorResult(FT_Err_Ok),
    mLibrary(0),
    mFace(0),
    mFontName(fontName)
{
    mErrorResult = FT_Init_FreeType(&mLibrary);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to initialize FreeType library!" };
    }

    mErrorResult = FT_New_Memory_Face(mLibrary, &fileBytes[0], fileBytes.size(), 0, &mFace);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to create new face!" };
    }

    mErrorResult = FT_Set_Char_Size(mFace, fontSize << 6, fontSize << 6, 90, 90);
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
	FT_Done_FreeType(mLibrary);
}

void FontResources::LoadGlyphsFromRange(std::uint32_t begin, std::uint32_t end)
{
    CreateFontAtlasFromRange(begin, end);
}

void FontResources::EnsureGlyphs(std::vector<std::uint32_t> codePoints)
{
}

void FontResources::StoreFontAtlasesInFiles(bool in)
{
    bStoreFontAtlasesInFiles = in;
}

std::pair<GlyphMetrics, ImageData> FontResources::CreateGlyphBitmap(std::uint32_t codePoint)
{
    FT_UInt glyphIndex = 0;
    glyphIndex = FT_Get_Char_Index(mFace, static_cast<FT_ULong>(codePoint));
    mErrorResult = FT_Load_Glyph(mFace, glyphIndex, FT_LOAD_DEFAULT);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to load glyph!" };
    }

    mErrorResult = FT_Render_Glyph(mFace->glyph, FT_RENDER_MODE_NORMAL);
    if (mErrorResult)
    {
        throw std::runtime_error{ "Failed to render glyph!" };
    }

    GlyphMetrics glyphMetrics;
    glyphMetrics.atlasX = 0;
    glyphMetrics.atlasY = 0;
    glyphMetrics.width = mFace->glyph->bitmap.width;
    glyphMetrics.height = mFace->glyph->bitmap.rows;
    
    glyphMetrics.bearingX = mFace->glyph->bitmap_left;
    glyphMetrics.bearingY = mFace->glyph->bitmap_top;
    glyphMetrics.advanceX = mFace->glyph->advance.x >> 6;

    std::vector<std::uint8_t> buffer;
    for (int i = 0; i < (mFace->glyph->bitmap.width * mFace->glyph->bitmap.rows); ++i)
    {
        std::uint8_t coverage = mFace->glyph->bitmap.buffer[i];
        buffer.push_back(coverage);
        buffer.push_back(coverage);
        buffer.push_back(coverage);
        buffer.push_back(coverage);
    }

    return std::pair(glyphMetrics, ImageData(mFace->glyph->bitmap.width, mFace->glyph->bitmap.rows, buffer));
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
    for (auto glyph = begin; glyph <= end; ++glyph)
    {
        if (!HasGlyph(glyph))
        {
            continue;
        }
        // Creating bitmaps of glyphs should be considered to dome multythreaded
        glyphs[glyph] = CreateGlyphBitmap(glyph);
    }
    if (glyphs.empty())
        return;

    auto fontAtlas = TextureAtlasMaker::CreateTextureAtlas(glyphs);

    // 2. Upload texture via TextureCache

    auto textureCache = mRenderResourceContext.textureCache;
    const std::string textureName = mFontName + "_FontAtlas_" + std::to_string(mFontAtlases.size());
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
    materialSettings.blendMode = BlendMode::Opaque;
    materialSettings.parameterLayout = &Font2DLayout;

    materialCache->AddMaterial(materialSettings);
    Material* material = materialCache->GetMaterial(materialName);
    material->SetVec4("FontColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    material->AddTexture(textureName);

    // material->SetFloat >> Add custom material parameters for UV manipulation
    material->InitializeRenderResources();

    // 4 Upload to container mFontAtlases
    for (auto& it : glyphs)
    {
        mGlyphs[it.first] = std::pair<GlyphMetrics, std::string>(it.second.first, materialName);
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

GlyphMetrics FontResources::GetGlyphMetrics(std::uint32_t codePoint) const
{
    auto search = mGlyphs.find(codePoint);
    if (search == mGlyphs.end())
    {
        return GlyphMetrics();
    }
    return search->second.first;
}

std::string FontResources::GetFontAtlasTextureName(std::uint32_t codePoint) const
{
    auto matName = GetFontAtlasMaterialName(codePoint);
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

std::string FontResources::GetFontAtlasMaterialName(std::uint32_t codePoint) const
{
    auto search = mGlyphs.find(codePoint);
    if (search == mGlyphs.end())
    {
        return std::string();
    }
    return search->second.second;
}

void FontResources::CreateFontAtlasFromList(std::vector<std::uint32_t> codePoints)
{
    for (std::uint32_t codePoint : codePoints)
    {
        if (!HasGlyph(codePoint))
        {
            continue;
        }
    }
}

} // namespace rendering_engine