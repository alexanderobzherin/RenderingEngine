#include <stdio.h>
#include <cstdint>
#include "gtest/gtest.h"
#include "../Source/image_data.hpp"

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

TEST(ImageDataTests, ArrayDoesntMatchDimension)
{
  /*Load Pixels RGBA matrix 3x3
  115 110 105 100 | 190 185 180 175 | 200 195 190 185
  215 210 205 200 | 250 245 240 235 | 260 255 250 245
  300 295 290 285 | 350 345 340 335 | 400 395 390 385 500
  */
  //Load vector of test pixels
  std::vector<unsigned int> testPixels{ 115, 110, 105, 100, 190, 185, 180, 175, 200, 195, 190, 185,
                                        215, 210, 205, 200, 250, 245, 240, 235, 260, 255, 250, 245,
                                        300, 295, 290, 285, 350, 345, 340, 335, 400, 395, 390, 385, 500
  };

  ImageData image(3, 3);
  EXPECT_THROW(image.LoadImageData(testPixels), std::runtime_error);
}

TEST(ImageDataTests, TransitImageData)
{
  /*Load Pixels RGBA matrix 3x3
  115 110 105 100 | 190 185 180 175 | 200 195 190 185
  215 210 205 200 | 250 245 240 235 | 210 255 250 245
  120 145 140 135 | 111 101 98  76  | 30  123 145 255
  */
  //Load vector of test pixels
  std::vector<unsigned int> testPixels{ 115, 110, 105, 100, 190, 185, 180, 175, 200, 195, 190, 185,
                                        215, 210, 205, 200, 250, 245, 240, 235, 210, 255, 250, 245,
                                        120, 145, 140, 135, 111, 101, 98,  76,  30,  123, 145, 255
  };

  ImageData image(3, 3);
  image.LoadImageData(testPixels);
  image.WriteTextureJpegFile(image, "asd");
  auto correspondingPixels = image.GetImageDataRGBA();

  ASSERT_EQ(testPixels.size(), correspondingPixels.size());

  auto correspondingPixel = correspondingPixels.begin();
  for(auto testPixel : testPixels)
  {
      EXPECT_EQ(testPixel, (int)(*correspondingPixel)); 
      ++correspondingPixel;
  }
}
