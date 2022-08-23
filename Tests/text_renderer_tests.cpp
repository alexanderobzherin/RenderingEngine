#include <stdio.h>
#include <cstdint>
#include "gtest/gtest.h"

#include "../RenderingEngine/Include/image_data.hpp"
#include "../RenderingEngine/Include/text_renderer.hpp"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::TestSuite;
using ::testing::UnitTest;

using namespace rendering_engine;

TEST(ImageDataTest, CreateGlyphBitmapStatic)
{
    std::string const pathToFont{ "../Intermediate/Fonts/Exo/Exo-Medium.otf" };

    auto imageData = std::make_shared<ImageData>(TextRenderer::CreateGlyphBitmap(pathToFont, '7' ));
    imageData->WritePngFile("testGlyphStatic.png");

    EXPECT_NE( imageData, nullptr );
}

TEST(ImageDataTest, CreateGlyphBitmap)
{
    std::string const pathToFont{ "../Intermediate/Fonts/Exo/Exo-Medium.otf" };

    TextRenderer textRenderer(pathToFont, 14);

    auto imageData = std::make_shared<ImageData>( textRenderer.CreateGlyphBitmap('4') );
    imageData->WritePngFile("testGlyphBitmap.png");

    EXPECT_NE(imageData, nullptr);
}

TEST(ImageDataTest, CreateTextBitmap1)
{
    std::string const pathToFont{ "../Intermediate/Fonts/Pirulen/pirulen_rg.otf" };

    TextRenderer textRenderer( pathToFont, 48 );

    std::string testText{"This is Pirulen font test text 123 string"};
    auto imageData = std::make_shared<ImageData>( textRenderer.CreateStringBitmap(testText) );
    imageData->WritePngFile("testPirulenFontTextString.png");

    EXPECT_NE(imageData, nullptr);
}

TEST(ImageDataTest, CreateTextBitmap2)
{
    std::string const pathToFont{ "../Intermediate/Fonts/DigitalDream/DIGITALDREAM.ttf" };

    TextRenderer textRenderer(pathToFont, 48);

    std::string testText{ "This_is_DIGITAL-DREAM_font_test_text_123_string" };
    auto imageData = std::make_shared<ImageData>( textRenderer.CreateStringBitmap(testText) );
    imageData->WritePngFile("testDigitalDreamFontTextString.png");

    EXPECT_NE(imageData, nullptr);
}