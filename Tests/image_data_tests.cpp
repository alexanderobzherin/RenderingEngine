#include <stdio.h>
#include <cstdint>
#include "gtest/gtest.h"
#include "../RenderingEngine/Include/image_data.hpp"
#include "../RenderingEngine/Include/model.hpp"
#include "../RenderingEngine/Include/mesh.hpp"
#include "../RenderingEngine/Include/text_renderer.hpp"
#include "../RenderingEngine/Include/texture_atlas_maker.hpp"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::TestSuite;
using ::testing::UnitTest;

using namespace rendering_engine;

TEST(ImageDataTests, PixelAssignment) 
{
  Color pixel1(1,2,3, 50);
  Color pixel2 = pixel1;

  EXPECT_EQ(pixel1.a, pixel2.a);
  EXPECT_EQ(pixel1.r, pixel2.r);
  EXPECT_EQ(pixel1.g, pixel2.g);
  EXPECT_EQ(pixel1.b, pixel2.b);
}

TEST(ImageDataTests, ImageDataAssignment)
{
  ImageData image1(3U, 3U);
  Color const testColor(115U, 100U, 10U, 150U);
  image1.Fill(testColor);

  ImageData image2(1, 1);
  image2 = image1;

  ASSERT_EQ(image2.GetHeight(), image1.GetHeight());
  ASSERT_EQ(image2.GetWidth(), image1.GetWidth());
  
  for( unsigned int y = 0; y < image1.GetHeight(); ++y )
  {
    for( unsigned int x = 0; x < image1.GetWidth(); ++x )
    {
      EXPECT_EQ(image1.GetPixel(x, y), image2.GetPixel(x, y));
    }    
  }
}

TEST(ImageDataTests, GetImageDataEmpty)
{
  ImageData image;
  EXPECT_TRUE(image.GetImageDataRGBA().empty());
  image = ImageData(3, 3);
  image.Fill(Color(255, 255, 255, 255));
  EXPECT_FALSE(image.GetImageDataRGBA().empty());
}

TEST(ImageDataTests, InappropriateAccess)
{
  ImageData image(3, 3);
  image.Fill(Color(255, 255, 255, 255));

  EXPECT_THROW(image.GetPixel(4, 4), std::runtime_error);
}

TEST(ImageDataTest, SaveLoadTextureJpg)
{
    std::string const filepath{"test_jpeg_texture_file.jpg"};
    ImageData image1(10U, 10U);
    image1.Fill(Color(255, 0, 0, 255));
    image1.WriteJpegFile(filepath.c_str());
    
    ImageData image2("test_jpeg_texture_file.jpg");

    EXPECT_EQ( image1.GetHeight(), image2.GetHeight() );
    EXPECT_EQ( image1.GetWidth(), image2.GetWidth() );
    //Pixels in the output jpeg file don't match completely to source pixels, because the compression algorithm in jpeg might slightly change values.
}

TEST(ImageDataTest, SaveLoadTexturePng)
{
    std::string const filepath{ "test_png_texture_file.png" };
    ImageData image1(10U, 10U);
    image1.Fill(Color(255, 0, 0, 255));
    image1.WritePngFile(filepath.c_str());

    ImageData image2("test_png_texture_file.png");

    EXPECT_EQ(image1.GetHeight(), image2.GetHeight());
    EXPECT_EQ(image1.GetWidth(), image2.GetWidth());
    EXPECT_EQ(image1.GetImageDataRGBA(), image2.GetImageDataRGBA());
}

TEST(ImageDataTest, IncorrectTextureFilepath)
{
    std::string const filepath{ "missing_texture_file.png" };

    EXPECT_THROW(ImageData image(filepath), std::runtime_error);
}

TEST(ImageDataTest, ModelsLoading)
{
    std::string const modelFilepath{ "../Intermediate/Models/TestCube/test_cube.fbx" };
    std::string const textureFilepath{ "../Intermediate/Models/TestCube/test_cube_color.png" };

    ImageData textureImageData(textureFilepath);
    textureImageData.GetHeight();

    Model model(modelFilepath);
    ASSERT_TRUE(model.HasMeshes());

    auto const meshName = model.Meshes().at(0)->Name();
    auto const vertices = model.Meshes().at(0)->Vertices();
    auto const indices = model.Meshes().at(0)->Indices();
    auto const normals = model.Meshes().at(0)->Normals();
    auto const texCoord = model.Meshes().at(0)->TextureCoordinates();
    EXPECT_NE(0, vertices.size());
}

TEST(ImageDataTest, RenderTextStatic)
{
    std::string const pathToFont{ "../Intermediate/Fonts/Exo/Exo-Medium.otf" };

    auto imageData = TextRenderer::CreateGlyphBitmap(pathToFont, '7' );
    imageData.WritePngFile("testGlyphStatic.png");

    std::cout << "Char A in unicode is:" << std::to_string((std::uint16_t)('A')) << std::endl;
    //std::cout << "String ABC12% in unicode is:" << std::to_string((std::uint16_t)('ABC12%')) << std::endl;

}

TEST(ImageDataTest, CreateGlyphBitmap)
{
    std::string const pathToFont{ "../Intermediate/Fonts/Exo/Exo-Medium.otf" };

    TextRenderer textRenderer(pathToFont, 14);

    auto imageData = textRenderer.CreateGlyphBitmap('4');
    imageData.WritePngFile("testGlyphBitmap.png");

}

TEST(ImageDataTest, CreateTextBitmap1)
{
    std::string const pathToFont{ "../Intermediate/Fonts/Pirulen/pirulen_rg.otf" };

    TextRenderer textRenderer( pathToFont, 48 );

    std::string testText{"This is Pirulen font test text 123 string"};
    auto imageData = textRenderer.CreateStringBitmap(testText);
    imageData.WritePngFile("testPirulenFontTextString.png");

}

TEST(ImageDataTest, CreateTextBitmap2)
{
    std::string const pathToFont{ "../Intermediate/Fonts/DigitalDream/DIGITALDREAM.ttf" };

    TextRenderer textRenderer(pathToFont, 48);

    std::string testText{ "This_is_DIGITAL-DREAM_font_test_text_123_string" };
    auto imageData = textRenderer.CreateStringBitmap(testText);
    imageData.WritePngFile("testDigitalDreamFontTextString.png");
}

TEST(ImageDataTest, TextureAtlasMaker)
{
    ImageData image1(10U, 10U);
    image1.Fill(Color(255, 0, 0, 255));

    ImageData image2(5U, 5U);
    image2.Fill(Color(0, 255, 0, 255));

    ImageData image3(2U, 2U);
    image3.Fill(Color(0, 0, 255, 255));

    std::map<char, ImageData> imageCollection;

    imageCollection.emplace('A', image1);
    imageCollection.emplace('B', image2);
    imageCollection.emplace('C', image3);

    TextureAtlasMaker texAtlasMaker( imageCollection );

    ImageData atlasImage;
    std::map<char, std::pair<unsigned int, unsigned int>> texAtlasData;

    bool const result = texAtlasMaker.CreateTextureAtlas(texAtlasData, atlasImage);
    if( result )
    {
        atlasImage.WritePngFile("TestTextureAtlas.png");
    }

    EXPECT_TRUE(result);
}