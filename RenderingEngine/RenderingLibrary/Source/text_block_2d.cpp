#include "text_block_2d.hpp"
#include "camera_2d.hpp"
#include "text_renderer.hpp"
#include "font_resources.hpp"
#include "texture_cache.hpp"
#include "image_data_gpu.hpp"
#include "image_data.hpp"
#include "model_cache.hpp"
#include "scene.hpp"

#include "drawable_component.hpp"

#include "i_renderer.hpp"
#include "i_render_resources.hpp"
#include "model_cache.hpp"
#include "material_cache.hpp"
#include "material.hpp"

#include <hb.h>
#include <hb-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TYPES_H
#include FT_OUTLINE_H
#include FT_RENDER_H
#include <cassert>

namespace rendering_engine
{
std::string TextBlock2D::sDefaultFontName = "RobotoMono-Regular";

std::uint64_t TextBlock2D::sNumOfTextBlocks = 0;

template<>
std::unordered_map<std::string, TextBlock2D::Mesh> TextBlock2D::PrepareMeshSlots(const std::vector<std::uint32_t>& glyphs);

template<>
std::unordered_map<std::string, TextBlock2D::Mesh> TextBlock2D::PrepareMeshSlots(const std::vector<ShapedGlyph>& glyphs);

TextBlock2D::TextBlock2D(Scene& scene, std::shared_ptr<TextRenderer> textRenderer, Properties properties)
    :
    Drawable2D(textRenderer->GetRenderResourceContext(), scene),
    mTextRenderer(textRenderer),
    mColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
    mFontName(properties.fontName),
    mFontSize(properties.fontSize),
    mLineSpacingScale(properties.lineSpacingScale),
    mMaxLineLength(properties.maxLineLength),
    mTextAlign(properties.textAlign),
    mDimensions(glm::vec2(0.0f, 0.0f)),
    bIsTextShapeEnabled(properties.textShapeEnabled),
    mOutlineThicknessPx(properties.outlineThicknessPx > 2.0f ? 2.0f : properties.outlineThicknessPx)
{
    mFontResources = mTextRenderer->GetFontResources(mFontName, mFontSize);
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
    transformations.model = GetTransform().GetWorldMatrix();
    transformations.view = camera.GetWorldView();
    transformations.proj = camera.GetProjectionMatrix();

    for (auto& renderBatch : mRenderBatches)
    {
        renderBatch.renderResources->SubmitResources(transformations, renderBatch.materialParameters);
    }
}

void TextBlock2D::SetText(std::string text)
{
    if (mText == text)
    {
        //Text has not changes, no new ensure/mesh-construction needed.
        return;
    }
    mText = text;


    if (bIsTextShapeEnabled)
    {
        ShapeTextAndConstructMesh();
    }
    else
    {
        ConstructMesh();
    }
    SetOutlineThickness(mOutlineThicknessPx);
}

void TextBlock2D::SetTextColor(glm::vec4 color)
{
    for (auto& renderBatch : mRenderBatches)
    {
        renderBatch.materialParameters.SetMaterialVec4("FontColor", color);
    }
}

void TextBlock2D::SetOutlineColor(glm::vec4 color)
{
    for (auto& renderBatch : mRenderBatches)
    {
        renderBatch.materialParameters.SetMaterialVec4("OutlineColor", color);
    }
}

glm::vec2 TextBlock2D::GetDimensions() const
{
    return mDimensions;
}

void TextBlock2D::SetOutlineThickness(float thicknessPx)
{
    for (auto& renderBatch : mRenderBatches)
    {
        renderBatch.materialParameters.SetMaterialFloat("OutlineThicknessPx", thicknessPx);
    }
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

        if (c <= 0x7FU)
        {
            // 1-byte sequence (ASCII)
            codePoint = c;
            i += 1;
        }
        else if ((c & 0xE0U) == 0xC0U)
        {
            // 2-byte sequence
            if (i + 1 >= length) break;

            codePoint =
                ((c & 0x1FU) << 6) |
                (bytes[i + 1] & 0x3FU);

            i += 2;
        }
        else if ((c & 0xF0U) == 0xE0U)
        {
            // 3-byte sequence
            if (i + 2 >= length) break;

            codePoint =
                ((c & 0x0FU) << 12) |
                ((bytes[i + 1] & 0x3FU) << 6) |
                (bytes[i + 2] & 0x3FU);

            i += 3;
        }
        else if ((c & 0xF8U) == 0xF0U)
        {
            // 4-byte sequence
            if (i + 3 >= length) break;

            codePoint =
                ((c & 0x07U) << 18) |
                ((bytes[i + 1] & 0x3FU) << 12) |
                ((bytes[i + 2] & 0x3FU) << 6) |
                (bytes[i + 3] & 0x3FU);

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

void TextBlock2D::ConstructMeshAutoLinebreak(const std::vector<std::uint32_t>& codePoints)
{
    auto meshes = PrepareMeshSlots(codePoints);

    const FontMetrics& fontMetrics = mFontResources->GetFontMetrics();
    const float lineHeight = static_cast<float>(fontMetrics.lineHeight);
    // Pen position (baseline)
    float penX = 0.0f;
    float penY = 0.0f;

    const std::uint32_t space{ 0x20 };
    const std::uint32_t newLine{ 0x0A };

    const bool autoTextWrappingRequested = mMaxLineLength > 0.0f;
    if (!autoTextWrappingRequested)
    {
        for (std::uint32_t glyph : codePoints)
        {
            if (glyph == newLine)
            {
                penX = 0.0f;
                penY += lineHeight;
                continue;
            }

            GlyphIndex glyphIndex = mFontResources->GetIndexFromCodePoint(glyph);
            GlyphQuad glyphQuad = MakeGlyphQuad(glyphIndex, penX, penY);
            PushQuad(meshes, glyphQuad);
            penX += static_cast<float>(glyphQuad.advanceX);
        }
    }
    else
    {
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

            while (nextGlyphIndex < codePoints.size())
            {
                if (codePoints[nextGlyphIndex] == space)
                    break;

                const std::string curGlyph = CodepointToUtf8(codePoints[nextGlyphIndex]);
                stringProcessed.append(curGlyph);
                GlyphIndex glyphIndex = mFontResources->GetIndexFromCodePoint(codePoints[nextGlyphIndex]);
                GlyphQuad glyphQuad = MakeGlyphQuad(glyphIndex, penX, penY);
                penX += static_cast<float>(glyphQuad.advanceX);
                nextWord.push_back(glyphQuad);
                ++nextGlyphIndex;
                isLastGlyphProcessed = nextGlyphIndex >= codePoints.size();
            }

            const bool isNewWordFitLine = penX <= mMaxLineLength;

            if (line.empty() || isNewWordFitLine)
            {
                if (!line.empty())
                {
                    //Insert SPACE
                    GlyphIndex glyphIndex = mFontResources->GetIndexFromCodePoint(space);
                    GlyphQuad glyphQuad = MakeGlyphQuad(glyphIndex, penX, penY);
                    stringProcessed.append(" ");
                    penX += static_cast<float>(glyphQuad.advanceX);
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
                    PushQuad(meshes, quad, horizontalShift);
                }
                line.clear();

                penX = 0.0f;
                penY += lineHeight;
                nextGlyphIndex = currentIndex;
            }
        }
    }

    UploadMeshes(meshes);

    Initialize();
}

void TextBlock2D::ConstructMesh()
{
    std::vector<std::uint32_t> allUsedGlyphs;
    std::vector<std::vector<std::uint32_t>> linesOfGlyphs;

    auto textRuns = SplitString(mText, "\n");

    for (const auto& textRunString : textRuns)
    {
        auto textRunCodePoints = DecodeUtf8(textRunString);
        linesOfGlyphs.push_back(textRunCodePoints);
        std::copy(textRunCodePoints.begin(), textRunCodePoints.end(), std::back_inserter(allUsedGlyphs));
    }
    // It is better to ensure all required glyphs at once, as those if not ready yet,
    // will be added to a single font atlas
    mFontResources->EnsureGlyphs(allUsedGlyphs);
    auto meshes = PrepareMeshSlots(allUsedGlyphs);


    float maximumLineLengh = 0.0f;
    std::vector<float> lineLengths;

    std::vector<std::vector<GlyphQuad>> linesOfGlyphQuads;

    const FontMetrics& fontMetrics = mFontResources->GetFontMetrics();
    const float lineHeight = static_cast<float>(fontMetrics.lineHeight);

    float penX = 0.0f;
    float penY = 0.0f;

    for (auto line : linesOfGlyphs)
    {
        penX = 0.0f;
        std::vector<GlyphQuad> lineOfGlyphQuads;
        for (const auto& glyph : line)
        {
            GlyphIndex glyphIndex = mFontResources->GetIndexFromCodePoint(glyph);
            GlyphQuad glyphQuad = MakeGlyphQuad(glyphIndex, penX, penY);

            penX += static_cast<float>(glyphQuad.advanceX);
            lineOfGlyphQuads.push_back(glyphQuad);
        }

        lineLengths.push_back(penX);
        linesOfGlyphQuads.push_back(lineOfGlyphQuads);
        const float lineLength = penX;

        penY += (lineHeight * mLineSpacingScale);

        if (lineLength > maximumLineLengh)
        {
            maximumLineLengh = lineLength;
        }
    }

    mMaxLineLength = (mMaxLineLength > maximumLineLengh ? mMaxLineLength : maximumLineLengh);
    mDimensions = glm::vec2(mMaxLineLength, penY);

    size_t curLine = 0;
    for (const auto& line : linesOfGlyphQuads)
    {
        for (const auto& glyphQuad : line)
        {
            float horizontalShift = 0.0f;
            if (mTextAlign == TextAlign::Center)
            {
                horizontalShift = (mMaxLineLength - lineLengths[curLine]) / 2.0f;
            }
            if (mTextAlign == TextAlign::Right)
            {
                horizontalShift = (mMaxLineLength - lineLengths[curLine]);
            }
            PushQuad(meshes, glyphQuad, horizontalShift);
        }
        ++curLine;
    }

    UploadMeshes(meshes);

    Initialize();
}

void TextBlock2D::ShapeTextAndConstructMesh()
{
    auto textRuns = SplitString(mText, "\n");

    std::vector<std::vector<ShapedGlyph>> linesOfShapedGlyphs;
    std::vector<ShapedGlyph> preShapedGlyphs;
 
    for (const auto& textRun : textRuns)
    {
        auto shapedTextRun = ShapeText(textRun);
        linesOfShapedGlyphs.push_back(shapedTextRun);
        std::copy(shapedTextRun.begin(), shapedTextRun.end(), std::back_inserter(preShapedGlyphs));
    }
    std::vector<GlyphIndex> ensureGlyphs;
    for (const auto& shapedGlyph : preShapedGlyphs)
    {
        GlyphIndex gi;
        gi.index = shapedGlyph.glyphIndex;
        ensureGlyphs.push_back(gi);
    }
    mFontResources->EnsureGlyphs(ensureGlyphs);

    auto meshes = PrepareMeshSlots(preShapedGlyphs);

    // Pen position (baseline)
    float penX = 0.0f;
    float penY = 0.0f;

    float maximumLineLengh = 0.0f;
    std::vector<float> lineLengths;

    for (auto& line : linesOfShapedGlyphs)
    {
        float lineLength = 0.0f;
        for (const auto& glyph : line)
        {
            lineLength += glyph.xAdvance;
        }
        lineLengths.push_back(lineLength);
        if (lineLength > maximumLineLengh)
        {
            maximumLineLengh = lineLength;
        }
    }
    mMaxLineLength = (mMaxLineLength > maximumLineLengh ? mMaxLineLength : maximumLineLengh);

    const FontMetrics& fontMetrics = mFontResources->GetFontMetrics();
    const float lineHeight = static_cast<float>(fontMetrics.lineHeight);
    std::size_t curLine = 0U;
    for (auto& line : linesOfShapedGlyphs)
    {
        for (const auto& glyph : line)
        {
            GlyphIndex glyphIndex;
            glyphIndex.index = glyph.glyphIndex;

            GlyphQuad glyphQuad = MakeGlyphQuad(glyphIndex, penX + glyph.xOffset, penY + glyph.yOffset);

            float horizontalShift = 0.0f;
            if (mTextAlign == TextAlign::Center)
            {
                horizontalShift = (mMaxLineLength - lineLengths[curLine]) / 2.0f;
            }
            if (mTextAlign == TextAlign::Right)
            {
                horizontalShift = (mMaxLineLength - lineLengths[curLine]);
            }
            PushQuad(meshes, glyphQuad, horizontalShift);
            penX += glyph.xAdvance;
        }

        penX = 0.0f;
        penY += (lineHeight * mLineSpacingScale);
        ++curLine;
    }

    const float lineCount =
    static_cast<float>(linesOfShapedGlyphs.size());

    mDimensions = glm::vec2(mMaxLineLength, lineCount * lineHeight * mLineSpacingScale);

    UploadMeshes(meshes);

    Initialize();
}

TextBlock2D::GlyphQuad TextBlock2D::MakeGlyphQuad(GlyphIndex glyphIndex, float penX, float penY)
{
    GlyphQuad result;

    const GlyphMetrics& glyphMetrics = mFontResources->GetGlyphMetrics(glyphIndex);
    auto fontAtlasMaterialName = mFontResources->GetFontAtlasMaterialName(glyphIndex);
    const auto fontAtlasTextureName = mFontResources->GetFontAtlasTextureName(glyphIndex);
    auto textureCache = mTextRenderer->GetRenderResourceContext().textureCache;
    const auto& fontAtlas = textureCache->GetTextureResources(fontAtlasTextureName);

    result.fontAtlasMaterialName = fontAtlasMaterialName;
    
    const float bearingX = static_cast<float>(glyphMetrics.bearingX);
    const float bearingY = static_cast<float>(glyphMetrics.bearingY);
    const float glyphWidth = static_cast<float>(glyphMetrics.width);
    const float glyphHeight = static_cast<float>(glyphMetrics.height);

    // Positions
    const float x0 = penX + bearingX;
    const float y0 = penY - bearingY; // y0 - top
    const float y1 = y0 + glyphHeight; // y1 - bottom
    const float x1 = x0 + glyphWidth;

    result.x0 = x0 - mOutlineThicknessPx * 2;
    result.y0 = y0 - mOutlineThicknessPx * 2;
    result.x1 = x1 + mOutlineThicknessPx * 2;
    result.y1 = y1 + mOutlineThicknessPx * 2;

    // UVs
    const float atlasWidth =
        static_cast<float>(fontAtlas->GetCpuImageData().GetWidth());
    const float atlasHeight =
        static_cast<float>(fontAtlas->GetCpuImageData().GetHeight());

    const float atlasX = static_cast<float>(glyphMetrics.atlasX);
    const float atlasY = static_cast<float>(glyphMetrics.atlasY);

    result.u0 = (atlasX - mOutlineThicknessPx * 2) / atlasWidth;
    result.v0 = (atlasY - mOutlineThicknessPx * 2) / atlasHeight;
    result.u1 = (atlasX + glyphWidth + mOutlineThicknessPx * 2) / atlasWidth;
    result.v1 = (atlasY + glyphHeight + mOutlineThicknessPx * 2) / atlasHeight;

    result.advanceX = glyphMetrics.advanceX;

    return result;
}

void TextBlock2D::PushQuad(std::unordered_map<std::string, TextBlock2D::Mesh>& meshes, GlyphQuad glyphQuad, float horizontalShift)
{
    auto& mesh = meshes.at(mMaterialMesh[glyphQuad.fontAtlasMaterialName]);

    if (mesh.positions2D.size() >
        static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max()) - 3U)
    {
        throw std::overflow_error("TextBlock2D mesh exceeds 32-bit index range.");
    }

    const std::uint32_t vertexBase = static_cast<std::uint32_t>(mesh.positions2D.size());

    const glm::vec2 shift(horizontalShift, 0.0f);

    const glm::vec2 vert_0 = glm::vec2(glyphQuad.x0, glyphQuad.y0) + shift;
    const glm::vec2 vert_1 = glm::vec2(glyphQuad.x1, glyphQuad.y0) + shift;
    const glm::vec2 vert_2 = glm::vec2(glyphQuad.x1, glyphQuad.y1) + shift;
    const glm::vec2 vert_3 = glm::vec2(glyphQuad.x0, glyphQuad.y1) + shift;

    mesh.positions2D.push_back(vert_0);
    mesh.positions2D.push_back(vert_1);
    mesh.positions2D.push_back(vert_2);
    mesh.positions2D.push_back(vert_3);

    // UVs
    mesh.texCoords.push_back(glm::vec2(glyphQuad.u0, glyphQuad.v0));
    mesh.texCoords.push_back(glm::vec2(glyphQuad.u1, glyphQuad.v0));
    mesh.texCoords.push_back(glm::vec2(glyphQuad.u1, glyphQuad.v1));
    mesh.texCoords.push_back(glm::vec2(glyphQuad.u0, glyphQuad.v1));

    // Colors
    mesh.colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    mesh.colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    mesh.colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    mesh.colors.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Indices
    mesh.indices.push_back(vertexBase + 0U);
    mesh.indices.push_back(vertexBase + 1U);
    mesh.indices.push_back(vertexBase + 2U);

    mesh.indices.push_back(vertexBase + 2U);
    mesh.indices.push_back(vertexBase + 3U);
    mesh.indices.push_back(vertexBase + 0U);
}

void TextBlock2D::UploadMeshes(const std::unordered_map<std::string, TextBlock2D::Mesh>& meshes)
{
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

    Shutdown();
    for (const auto& materialMesh : mMaterialMesh)
    {
        const std::string meshName = materialMesh.second;
        const std::string materialName = materialMesh.first;
        AddRenderBatch(meshName, materialName);
    }
}

std::vector<std::string> TextBlock2D::SplitString(const std::string& text, std::string separator)
{
    std::vector<std::string> result;
    std::string_view sv(text);
    size_t start = 0, end = 0;

    while (true) {
        end = sv.find(separator, start);
        // Extract token (including empty ones)
        result.emplace_back(sv.substr(start, end - start));
        if (end == std::string_view::npos) break;
        start = end + 1;  // Move past the delimiter
    }

    return result;
}

bool TextBlock2D::IsTextShapingRequired(std::uint32_t codePoint) const
{
    auto scriptsRequiredShaping = mTextRenderer->GetScriptsRequiredShaping();
    for (const auto& script : scriptsRequiredShaping)
    {
        auto range = mTextRenderer->GetScriptRange(script);
        if (codePoint >= range.first && codePoint <= range.second)
        {
            return true;
        }
    }

    return false;
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

std::vector<TextBlock2D::ShapedGlyph> TextBlock2D::ShapeText(const std::string& text)
{
    std::vector<TextBlock2D::ShapedGlyph> result;

    hb_buffer_t* buf;
    buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text.c_str(), -1, 0, -1);

    hb_buffer_guess_segment_properties(buf);

    auto* fontFace = mFontResources->GetFontFace();
    hb_font_t* font = hb_ft_font_create_referenced(fontFace);

    hb_shape(font, buf, NULL, 0);

    unsigned int glyph_count;
    hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
    hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

    for (unsigned int i = 0; i < glyph_count; i++)
    {
        ShapedGlyph shapedGlyph;
        shapedGlyph.glyphIndex = glyph_info[i].codepoint;
        shapedGlyph.xOffset = static_cast<float>(glyph_pos[i].x_offset) / 64.0f;
        shapedGlyph.yOffset = static_cast<float>(glyph_pos[i].y_offset) / 64.0f;
        shapedGlyph.xAdvance =static_cast<float>(glyph_pos[i].x_advance) / 64.0f;
        shapedGlyph.yAdvance = static_cast<float>(glyph_pos[i].y_advance) / 64.0f;
        shapedGlyph.cluster = glyph_info[i].cluster;
        result.push_back(shapedGlyph);
    }

    hb_font_destroy(font);
    hb_buffer_destroy(buf);

    return result;
}

template<>
std::unordered_map<std::string, TextBlock2D::Mesh> TextBlock2D::PrepareMeshSlots(const std::vector<std::uint32_t>& glyphs)
{
    mMaterialMesh.clear();
    // Prepare map of used material names with corresponding mesh names.
    for (auto codePoint : glyphs)
    {
        if (codePoint == 0x000A || codePoint == 0x000D || codePoint == 0x0009)
        {
            continue;
        }

        GlyphIndex glyphIndex = mFontResources->GetIndexFromCodePoint(codePoint);
        auto fontAtlasMaterialName = mFontResources->GetFontAtlasMaterialName(glyphIndex);
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
    return meshes;
}

template<>
std::unordered_map<std::string, TextBlock2D::Mesh> TextBlock2D::PrepareMeshSlots(const std::vector<ShapedGlyph>& glyphs)
{
    mMaterialMesh.clear();
    // Prepare map of used material names with corresponding mesh names.
    for (const auto& glyph : glyphs)
    {
        const std::uint32_t linefeed = 0x000A;
        const std::uint32_t carriageReturn = 0x000D;
        const std::uint32_t tab = 0x0009;
        if (glyph.glyphIndex == linefeed || glyph.glyphIndex == carriageReturn || glyph.glyphIndex == tab)
        {
            continue;
        }

        GlyphIndex glyphIndex;
        glyphIndex.index = glyph.glyphIndex;
        auto fontAtlasMaterialName = mFontResources->GetFontAtlasMaterialName(glyphIndex);
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

    return meshes;
}

} // namespace rendering_engine
