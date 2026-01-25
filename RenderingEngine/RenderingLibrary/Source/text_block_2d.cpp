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

#include <hb.h>
#include <hb-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TYPES_H
#include FT_OUTLINE_H
#include FT_RENDER_H

namespace rendering_engine
{
std::uint64_t TextBlock2D::sNumOfTextBlocks = 0;

template<>
std::unordered_map<std::string, TextBlock2D::Mesh> TextBlock2D::PrepareMeshSlots(const std::vector<std::uint32_t>& glyphs);

template<>
std::unordered_map<std::string, TextBlock2D::Mesh> TextBlock2D::PrepareMeshSlots(const std::vector<ShapedGlyph>& glyphs);

TextBlock2D::TextBlock2D(std::shared_ptr<TextRenderer> textRenderer, std::string fontName)
    :
    Drawable2D(textRenderer->GetRenderResourceContext()),
    mTextRenderer(textRenderer),
    mColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
    bIsTextShapeEnabled(false)
{
    SetFont(fontName);
    mFontResources = mTextRenderer->GetFontResources(mFontName);
    bIsTextShapeEnabled = mTextRenderer->IsTextShapingEnabled();
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

    for (auto& renderBatch : mRenderBatches)
    {
        renderBatch.renderResources->SubmitResources(transformations, renderBatch.materialParameters);
    }
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
    
    auto codeBytes = DecodeUtf8(mText);
    bool isTextShapeRequired = false;
    if (bIsTextShapeEnabled)
    {
        for (auto codeByte : codeBytes)
        {
            if (IsTextShapingRequired(codeByte))
            {
                isTextShapeRequired = true;
                break;
            }
        }
    }

    auto fontResources = mTextRenderer->GetFontResources(mFontName);
    
    if (bIsTextShapeEnabled && isTextShapeRequired)
    {
        ShapeTextAndConstructMesh();
    }
    else
    {
        fontResources->EnsureGlyphs(codeBytes);
        ConstructMesh(codeBytes);
    }
}

void TextBlock2D::SetMaxLineLength(float maxLineLength)
{
    mMaxLineLength = maxLineLength;
}

void TextBlock2D::SetTextColor(glm::vec4 color)
{
    for (auto& renderBatch : mRenderBatches)
    {
        renderBatch.materialParameters.SetMaterialVec4("FontColor", color);
    }
}

void TextBlock2D::SetTextAlign(TextAlign textAlign)
{
    mTextAlign = textAlign;
}

void TextBlock2D::SetTextShapeEnabled(bool in)
{
    bIsTextShapeEnabled = in;
}

void TextBlock2D::DrawFontAtlas()
{
    Shutdown();
    const std::string meshName("Quad2D");

    const std::uint32_t space{ 0x20 };
    GlyphIndex spaceGI = mFontResources->GetIndexFromCodePoint(space);
    auto fontAtlasMaterialName = mFontResources->GetFontAtlasMaterialName(spaceGI);
    AddRenderBatch(meshName, fontAtlasMaterialName);

    auto fontResources = mTextRenderer->GetFontResources(mFontName);
    auto textureCache = mTextRenderer->GetRenderResourceContext().textureCache;
    auto fontAtlasTextureName = fontResources->GetFontAtlasTextureName(spaceGI);

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

void TextBlock2D::ConstructMesh(const std::vector<std::uint32_t>& codePoints)
{
    auto meshes = PrepareMeshSlots(codePoints);

    const FontMetrics& fontMetrics = mFontResources->GetFontMetrics();
    // Pen position (baseline)
    float penX = 0.0f;
    float penY = 0.0f;

    const std::uint32_t space{ 0x20 };
    const std::uint32_t newLine{ 0x0A };

    const bool wordWrappingRequested = mMaxLineLength > 0.0f;
    if (!wordWrappingRequested)
    {
        for (std::uint32_t glyph : codePoints)
        {
            if (glyph == newLine)
            {
                penX = 0.0f;
                penY += fontMetrics.lineHeight;
                continue;
            }

            GlyphIndex glyphIndex = mFontResources->GetIndexFromCodePoint(glyph);
            GlyphQuad glyphQuad = MakeGlyphQuad(glyphIndex, penX, penY);
            const std::string meshName = mMaterialMesh[glyphQuad.fontAtlasMaterialName];
            PushQuad(meshName, meshes, glyphQuad);
            penX += glyphQuad.advanceX;
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
                penX += glyphQuad.advanceX;
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

    const FontMetrics& fontMetrics = mFontResources->GetFontMetrics();

    const bool alignmentRequested = mMaxLineLength > 0.0f;

    // Pen position (baseline)
    float penX = 0.0f;
    float penY = 0.0f;

    float maximumLineLengh = 0.0f;
    std::vector<float> lineLengths;
    if (mTextAlign == TextAlign::Center || mTextAlign == TextAlign::Right)
    {
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
    }

    int curLine = 0;
    for (auto& line : linesOfShapedGlyphs)
    {
        for (const auto& glyph : line)
        {
            GlyphIndex glyphIndex;
            glyphIndex.index = glyph.glyphIndex;

            GlyphQuad glyphQuad = MakeGlyphQuad(glyphIndex, penX + glyph.xOffset, penY + glyph.yOffset);
            const std::string meshName = mMaterialMesh[glyphQuad.fontAtlasMaterialName];

            float horizontalShift = 0.0f;
            if (mTextAlign == TextAlign::Center)
            {
                horizontalShift = (mMaxLineLength - lineLengths[curLine]) / 2.0f;
            }
            if (mTextAlign == TextAlign::Right)
            {
                horizontalShift = (mMaxLineLength - lineLengths[curLine]);
            }
            PushQuad(meshName, meshes, glyphQuad, horizontalShift);
            penX += glyph.xAdvance;
        }

        penX = 0.0f;
        penY += fontMetrics.lineHeight;
        ++curLine;
    }

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

    while ((start = sv.find_first_not_of(separator, end)) != std::string_view::npos) {
        end = sv.find(separator, start);
        result.emplace_back(sv.substr(start, end - start));
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

    // If you know the direction, script, and language
    /*
    hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
    hb_buffer_set_language(buf, hb_language_from_string("en", -1));
    */

    // If you don't know the direction, script, and language
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
        shapedGlyph.xOffset = glyph_pos[i].x_offset / 64.0f;
        shapedGlyph.yOffset = glyph_pos[i].y_offset / 64.0f;
        shapedGlyph.xAdvance = glyph_pos[i].x_advance / 64.0f;
        shapedGlyph.yAdvance = glyph_pos[i].y_advance / 64.0f;
        shapedGlyph.cluster = glyph_info[i].cluster;
        result.push_back(shapedGlyph);
    }

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
