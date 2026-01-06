#include "text_block_2d.hpp"
#include "camera_2d.hpp"
#include "text_renderer.hpp"
#include "font_resources.hpp"
#include "texture_cache.hpp"
#include "image_data_gpu.hpp"
#include "image_data.hpp"
#include "model_cache.hpp"

namespace rendering_engine
{
std::uint64_t TextBlock2D::sNumOfTextBlocks = 0;


TextBlock2D::TextBlock2D(std::shared_ptr<TextRenderer> textRenderer, std::string fontName)
    :
    Drawable2D(textRenderer->GetRenderResourceContext()),
    mTextRenderer(textRenderer),
    mColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
    SetFont(fontName);
    mFontResources = mTextRenderer->GetFontResources(mFontName);
    if (!mFontResources)
    {
        throw std::runtime_error("FontResources is not initialized for this TextBlock2D.");
    }

    ++sNumOfTextBlocks;

    mTextBlockID = "TextBlock_" + std::to_string(sNumOfTextBlocks);
}

void TextBlock2D::Initialize()
{
    Drawable2D::Initialize();
}
void TextBlock2D::Update(float deltaTime)
{
    Drawable2D::Update(deltaTime);
}

void TextBlock2D::Draw(const Camera2D& camera)
{
    Transformations2D transformations;
    transformations.model = GetTransform().GetModelMatrix();
    transformations.view = camera.GetWorldView();
    transformations.proj = camera.GetProjectionMatrix();

    mRenderResources->SubmitResources(transformations, mMaterialParameters);
}

void TextBlock2D::SetFont(std::string fontName)
{
    mFontName = fontName;
}

void TextBlock2D::SetText(std::string text)
{
    if (mText == text)
    {
        //Text has not changes, no new ensure/mesh-construction needed.
        return;
    }
    mText = text;

    auto fontResources = mTextRenderer->GetFontResources(mFontName);
    auto codeBytes = DecodeUtf8(mText);
    fontResources->EnsureGlyphs(codeBytes);

    ConstructMesh(codeBytes);
}

void TextBlock2D::SetTextColor(glm::vec4 color)
{
    SetMaterialVec4("FontColor", color);
}

void TextBlock2D::DrawFontAtlas()
{
    SetMeshName("Quad2D");

    auto fontAtlasMaterialName = mFontResources->GetFontAtlasMaterialName(std::uint32_t{ 0x0020 });
    SetMaterialName(fontAtlasMaterialName);

    auto fontResources = mTextRenderer->GetFontResources(mFontName);
    auto textureCache = mTextRenderer->GetRenderResourceContext().textureCache;
    auto fontAtlasTextureName = fontResources->GetFontAtlasTextureName(std::uint32_t{ 0x0020 });

    const auto& texture = textureCache->GetTextureResources(fontAtlasTextureName);

    const auto atlasWidth = texture->GetCpuImageData().GetWidth();
    const auto atlasHeight = texture->GetCpuImageData().GetHeight();

    SetScale(glm::vec2(atlasWidth, atlasHeight));
}

std::vector<std::uint32_t> TextBlock2D::DecodeUtf8(const std::string& text)
{
    std::vector<std::uint32_t> result;
    result.reserve(text.size()); // worst case: ASCII

    const unsigned char* bytes =
        reinterpret_cast<const unsigned char*>(text.data());
    const size_t length = text.size();

    size_t i = 0;
    while (i < length)
    {
        std::uint32_t codePoint = 0;
        unsigned char c = bytes[i];

        if (c <= 0x7F)
        {
            // 1-byte sequence (ASCII)
            codePoint = c;
            i += 1;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            // 2-byte sequence
            if (i + 1 >= length) break;

            codePoint =
                ((c & 0x1F) << 6) |
                (bytes[i + 1] & 0x3F);

            i += 2;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            // 3-byte sequence
            if (i + 2 >= length) break;

            codePoint =
                ((c & 0x0F) << 12) |
                ((bytes[i + 1] & 0x3F) << 6) |
                (bytes[i + 2] & 0x3F);

            i += 3;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            // 4-byte sequence
            if (i + 3 >= length) break;

            codePoint =
                ((c & 0x07) << 18) |
                ((bytes[i + 1] & 0x3F) << 12) |
                ((bytes[i + 2] & 0x3F) << 6) |
                (bytes[i + 3] & 0x3F);

            i += 4;
        }
        else
        {
            // Invalid UTF-8 start byte
            ++i;
            continue;
        }

        result.push_back(codePoint);
    }

    return result;
}

void TextBlock2D::ConstructMesh(const std::vector<std::uint32_t>& byteCodes)
{
    // Prepare map of used material names with corresponding mesh names.
    for (auto glyph : byteCodes)
    {
        auto fontAtlasMaterialName = mFontResources->GetFontAtlasMaterialName(glyph);
        const auto search = mMaterialMesh.find(fontAtlasMaterialName);
        if (search == mMaterialMesh.end())
        {
            const std::string meshName = mTextBlockID + "_" + std::to_string(mMaterialMesh.size());
            mMaterialMesh[fontAtlasMaterialName] = meshName;
        }
    }

    // Prepare map of used mesh names with mesh structures
    std::unordered_map<std::string, TextBlock2D::Mesh> meshes;
    for (const auto& mesh : mMaterialMesh)
    {
        meshes[mesh.second] = TextBlock2D::Mesh();
    }

    const FontMetrics& fontMetrics = mFontResources->GetFontMetrics();
    // Pen position (baseline)
    float penX = 0.0f;
    float penY = 0.0f;

    for (std::uint32_t glyph : byteCodes)
    {
        if (glyph == '\n')
        {
            penX = 0.0f;
            penY -= fontMetrics.lineHeight;
            continue;
        }

        const GlyphMetrics& glyphMetrics = mFontResources->GetGlyphMetrics(glyph);
        auto fontAtlasMaterialName = mFontResources->GetFontAtlasMaterialName(glyph);
        const auto fontAtlasTextureName = mFontResources->GetFontAtlasTextureName(glyph);
        auto textureCache = mTextRenderer->GetRenderResourceContext().textureCache;
        const auto& fontAtlas = textureCache->GetTextureResources(fontAtlasTextureName);

        // Positions
        const float x0 = penX + glyphMetrics.bearingX;
        const float y0 = penY - glyphMetrics.bearingY; // y0 - top
        const float y1 = y0 + glyphMetrics.height; // y1 - bottom
        const float x1 = x0 + glyphMetrics.width;

        uint32_t vertexBase = meshes.at(mMaterialMesh[fontAtlasMaterialName]).positions2D.size();

        meshes.at(mMaterialMesh[fontAtlasMaterialName]).positions2D.push_back(glm::vec2(x0, y0));
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).positions2D.push_back(glm::vec2(x1, y0));
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).positions2D.push_back(glm::vec2(x1, y1));
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).positions2D.push_back(glm::vec2(x0, y1));

        // UVs
        const auto atlasWidth = fontAtlas->GetCpuImageData().GetWidth();
        const auto atlasHeight = fontAtlas->GetCpuImageData().GetHeight();

        const float u0 = static_cast<float>(glyphMetrics.atlasX) / static_cast<float>(atlasWidth);
        const float v0 = static_cast<float>(glyphMetrics.atlasY) / static_cast<float>(atlasHeight);
        const float u1 = static_cast<float>(glyphMetrics.atlasX + glyphMetrics.width) / static_cast<float>(atlasWidth);
        const float v1 = static_cast<float>(glyphMetrics.atlasY + glyphMetrics.height) / static_cast<float>(atlasHeight);

        meshes.at(mMaterialMesh[fontAtlasMaterialName]).texCoords.push_back(glm::vec2(u0, v0));
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).texCoords.push_back(glm::vec2(u1, v0));
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).texCoords.push_back(glm::vec2(u1, v1));
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).texCoords.push_back(glm::vec2(u0, v1));

        // Colors

        meshes.at(mMaterialMesh[fontAtlasMaterialName]).colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        // Indices
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).indices.push_back(vertexBase + 0);
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).indices.push_back(vertexBase + 1);
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).indices.push_back(vertexBase + 2);

        meshes.at(mMaterialMesh[fontAtlasMaterialName]).indices.push_back(vertexBase + 2);
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).indices.push_back(vertexBase + 3);
        meshes.at(mMaterialMesh[fontAtlasMaterialName]).indices.push_back(vertexBase + 0);

        // Advance pen
        penX += glyphMetrics.advanceX;
    }

    // Once meshes are ready, request its upload from ModelCache
    auto modelCache = mTextRenderer->GetRenderResourceContext().meshCache;

    for (auto& material : mMaterialMesh) 
    {
        auto meshName = material.second;
        auto mesh = meshes.at(meshName);
        modelCache->LoadCustomMesh(meshName,
                                   mesh.positions2D,
                                   mesh.texCoords,
                                   mesh.colors,
                                   mesh.indices);
    }

    // Set mesh name and material name for THIS drawable, so the first font atlas for now only.
    
    SetMeshName(mMaterialMesh.begin()->second);
    SetMaterialName(mMaterialMesh.begin()->first);
}

std::string TextBlock2D::CodepointToUtf8(std::uint32_t codePoint)
{
    std::string out;

    if (codePoint <= 0x7F) {
        out.push_back(static_cast<char>(codePoint));
    }
    else if (codePoint <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | ((codePoint >> 6) & 0x1F)));
        out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    }
    else if (codePoint <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | ((codePoint >> 12) & 0x0F)));
        out.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    }
    else if (codePoint <= 0x10FFFF) {
        out.push_back(static_cast<char>(0xF0 | ((codePoint >> 18) & 0x07)));
        out.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
    }

    return out;
}

} // namespace rendering_engine
