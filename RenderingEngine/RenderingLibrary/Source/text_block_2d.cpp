#include "text_block_2d.hpp"
#include "camera_2d.hpp"
#include "text_renderer.hpp"
#include "font_resources.hpp"
#include "texture_cache.hpp"
#include "image_data_gpu.hpp"
#include "image_data.hpp"
#include "model_cache.hpp"

#include "drawable_component.hpp"

#include "i_renderer.hpp"
#include "i_render_resources.hpp"
#include "model_cache.hpp"
#include "material_cache.hpp"
#include "material.hpp"

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

void TextBlock2D::SetMaxLineLength(float maxLineLength)
{
    mMaxLineLength = maxLineLength;
}

void TextBlock2D::SetTextColor(glm::vec4 color)
{
    SetMaterialVec4("FontColor", color);
}

void TextBlock2D::SetTextAlign(TextAlign textAlign)
{
    mTextAlign = textAlign;
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
        if (glyph == 0x000A || glyph == 0x000D || glyph == 0x0009)
        {
            continue;
        }

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

    const std::uint32_t space{ 0x20 };
    const std::uint32_t newLine{ 0x0A };

    const bool wordWrappingRequested = mMaxLineLength > 0.0f;
    if (!wordWrappingRequested)
    {
        for (std::uint32_t glyph : byteCodes)
        {
            if (glyph == newLine)
            {
                penX = 0.0f;
                penY += fontMetrics.lineHeight;
                continue;
            }

            GlyphQuad glyphQuad = MakeGlyphQuad(glyph, penX, penY);
            const std::string meshName = mMaterialMesh[glyphQuad.fontAtlasMaterialName];
            PushQuad(meshName, meshes, glyphQuad);
            penX += glyphQuad.advanceX;
        }
    }
    else
    {
        if (mRenderResources)
        {
            mRenderResources->Shutdown();
            mRenderResources.reset();
        }
        mRenderResources = std::unique_ptr<IRenderResources>(mRenderContext.renderer->ProvideRenderResources());

        bool isStringComplete = false;

        // This variable describe the index we stay until new word is added.
        std::uint32_t currentIndex = 0;
        bool isLastGlyphProcessed = false;
        float lineLength = 0.0f;
        std::uint32_t nextGlyphIndex = 0;

        std::vector<GlyphQuad> line;
        std::vector<GlyphQuad> nextWord;

        while (!isStringComplete)
        {
            std::string stringProcessed;

            while (nextGlyphIndex < byteCodes.size())
            {
                if (byteCodes[nextGlyphIndex] == space)
                    break;

                const std::string curGlyph = CodepointToUtf8(byteCodes[nextGlyphIndex]);
                stringProcessed.append(curGlyph);
                GlyphQuad glyphQuad = MakeGlyphQuad(byteCodes[nextGlyphIndex], penX, penY);
                penX += glyphQuad.advanceX;
                nextWord.push_back(glyphQuad);
                ++nextGlyphIndex;
                isLastGlyphProcessed = nextGlyphIndex >= byteCodes.size();
            }

            const bool isNewWordFitLine = penX <= mMaxLineLength;

            if (line.empty() || isNewWordFitLine)
            {
                if (!line.empty())
                {
                    //Insert SPACE
                    GlyphQuad glyphQuad = MakeGlyphQuad(space, penX, penY);
                    stringProcessed.append(" ");
                    penX += glyphQuad.advanceX;
                    nextWord.push_back(glyphQuad);
                    ++nextGlyphIndex;
                }
                // Add new word to the line
                std::copy(nextWord.begin(), nextWord.end(), std::back_inserter(line));
                nextWord.clear();
                lineLength = penX;
                currentIndex = nextGlyphIndex;
                isStringComplete = isLastGlyphProcessed;
            }

            if (!isNewWordFitLine || isLastGlyphProcessed)
            {
                nextWord.clear();
                // Finalize current line and switch to next. Push quads for all line, setting horizontal alignment
                for (auto& quad : line)
                {
                    float horizontalShift = 0.0f;
                    if (mTextAlign == TextAlign::Center)
                    {
                        horizontalShift = (mMaxLineLength - lineLength) / 2.0f;
                    }
                    if (mTextAlign == TextAlign::Right)
                    {
                        horizontalShift = (mMaxLineLength - lineLength);
                    }
                    const std::string meshName = mMaterialMesh[quad.fontAtlasMaterialName];
                    PushQuad(meshName, meshes, quad, horizontalShift);
                }
                line.clear();

                penX = 0.0f;
                penY += fontMetrics.lineHeight;
                nextGlyphIndex = currentIndex;
            }
        }
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

    Initialize();
}

TextBlock2D::GlyphQuad TextBlock2D::MakeGlyphQuad(std::uint32_t glyph, float penX, float penY)
{
    GlyphQuad result;

    const GlyphMetrics& glyphMetrics = mFontResources->GetGlyphMetrics(glyph);
    auto fontAtlasMaterialName = mFontResources->GetFontAtlasMaterialName(glyph);
    const auto fontAtlasTextureName = mFontResources->GetFontAtlasTextureName(glyph);
    auto textureCache = mTextRenderer->GetRenderResourceContext().textureCache;
    const auto& fontAtlas = textureCache->GetTextureResources(fontAtlasTextureName);

    result.fontAtlasMaterialName = fontAtlasMaterialName;

    // Positions
    const float x0 = penX + glyphMetrics.bearingX;
    const float y0 = penY - glyphMetrics.bearingY; // y0 - top
    const float y1 = y0 + glyphMetrics.height; // y1 - bottom
    const float x1 = x0 + glyphMetrics.width;

    result.x0 = x0;
    result.y0 = y0;
    result.x1 = x1;
    result.y1 = y1;

    // UVs
    const auto atlasWidth = fontAtlas->GetCpuImageData().GetWidth();
    const auto atlasHeight = fontAtlas->GetCpuImageData().GetHeight();

    const float u0 = static_cast<float>(glyphMetrics.atlasX) / static_cast<float>(atlasWidth);
    const float v0 = static_cast<float>(glyphMetrics.atlasY) / static_cast<float>(atlasHeight);
    const float u1 = static_cast<float>(glyphMetrics.atlasX + glyphMetrics.width) / static_cast<float>(atlasWidth);
    const float v1 = static_cast<float>(glyphMetrics.atlasY + glyphMetrics.height) / static_cast<float>(atlasHeight);

    result.u0 = u0;
    result.v0 = v0;
    result.u1 = u1;
    result.v1 = v1;

    result.advanceX = glyphMetrics.advanceX;

    return result;
}

void TextBlock2D::PushQuad(std::string meshName, std::unordered_map<std::string, TextBlock2D::Mesh>& meshes, GlyphQuad glyphQuad, float horizontalShift)
{
    const uint32_t vertexBase = meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).positions2D.size();

    const glm::vec2 shift(horizontalShift, 0.0f);

    const glm::vec2 vert_0 = glm::vec2(glyphQuad.x0, glyphQuad.y0) + shift;
    const glm::vec2 vert_1 = glm::vec2(glyphQuad.x1, glyphQuad.y0) + shift;
    const glm::vec2 vert_2 = glm::vec2(glyphQuad.x1, glyphQuad.y1) + shift;
    const glm::vec2 vert_3 = glm::vec2(glyphQuad.x0, glyphQuad.y1) + shift;

    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).positions2D.push_back(vert_0);
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).positions2D.push_back(vert_1);
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).positions2D.push_back(vert_2);
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).positions2D.push_back(vert_3);

    // UVs
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).texCoords.push_back(glm::vec2(glyphQuad.u0, glyphQuad.v0));
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).texCoords.push_back(glm::vec2(glyphQuad.u1, glyphQuad.v0));
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).texCoords.push_back(glm::vec2(glyphQuad.u1, glyphQuad.v1));
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).texCoords.push_back(glm::vec2(glyphQuad.u0, glyphQuad.v1));

    // Colors

    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Indices
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).indices.push_back(vertexBase + 0);
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).indices.push_back(vertexBase + 1);
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).indices.push_back(vertexBase + 2);

    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).indices.push_back(vertexBase + 2);
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).indices.push_back(vertexBase + 3);
    meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]).indices.push_back(vertexBase + 0);
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
