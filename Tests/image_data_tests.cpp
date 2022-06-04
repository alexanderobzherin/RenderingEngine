#include <stdio.h>

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
  
  for( uint y = 0; y < image1.GetHeight(); ++y )
  {
    for( uint x = 0; x < image1.GetWidth(); ++x )
    {
      EXPECT_EQ(image1.GetPixel(x, y), image2.GetPixel(x, y));
    }    
  }
}