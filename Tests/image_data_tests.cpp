#include <stdio.h>
#include <cstdint>
#include "gtest/gtest.h"
#include "../RenderingEngine/Include/image_data.hpp"

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

