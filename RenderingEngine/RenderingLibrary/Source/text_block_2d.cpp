#include "text_block_2d.hpp"
#include "camera_2d.hpp"
#include "text_renderer.hpp"
#include "font_resources.hpp"
#include "texture_cache.hpp"
#include "image_data_gpu.hpp"
#include "image_data.hpp"

namespace rendering_engine
{
TextBlock2D::TextBlock2D(std::shared_ptr<TextRenderer> textRenderer, std::string fontName)
    :
    Drawable2D(textRenderer->GetRenderResourceContext()),
    mTextRenderer(textRenderer),
    mColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
    SetFont(fontName);
}

void TextBlock2D::Initialize()
{
    // For now assumeing we need only one quad, one material and one texture atlas

    SetMeshName("Quad2D");
    
    auto fontResources = mTextRenderer->GetFontResources(mFontName);
    
    if (!fontResources)
    {
        return;
    }

    // Set Material[s] according this font resource
    auto fontAtlasMaterialName = fontResources->GetFontAtlasMaterialName(std::uint32_t{ 0x0020 });
    SetMaterialName(fontAtlasMaterialName);

    Drawable2D::Initialize();

    //Set scale in respect to texture size, in future in computed string batch demensions
    

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
        //Text has not changes, no new ensure needed.
        return;
    }
    mText = text;

    auto fontResources = mTextRenderer->GetFontResources(mFontName);
    fontResources->EnsureGlyphs(DecodeUtf8(mText));
}

void TextBlock2D::SetTextColor(glm::vec4 color)
{
    SetMaterialVec4("FontColor", color);
}

void TextBlock2D::DrawFontAtlas()
{
    auto fontResources = mTextRenderer->GetFontResources(mFontName);
    auto textureCache = mTextRenderer->GetRenderResourceContext().textureCache;
    auto fontAtlasName = fontResources->GetFontAtlasTextureName(std::uint32_t{ 0x0020 });

    auto imageData = textureCache->GetTextureResources(fontAtlasName);
    auto width = imageData->GetCpuImageData().GetWidth();
    auto height = imageData->GetCpuImageData().GetHeight();
    SetScale(glm::vec2(width, height));
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

} // namespace rendering_engine
