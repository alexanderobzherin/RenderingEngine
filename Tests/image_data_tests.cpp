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

TEST(ImageDataTests, ImageDataTest01) 
{
  rendering_engine::ImageData image2(3, 3);
  rendering_engine::ImageData image1(image2);
  rendering_engine::ImageData image3 = image1;
  EXPECT_EQ(0, 0);
}