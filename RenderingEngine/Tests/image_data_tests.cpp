#include <stdio.h>
#include <cstdint>
#include "gtest/gtest.h"

#include "image_data.hpp"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::TestSuite;
using ::testing::UnitTest;

using namespace rendering_engine;

TEST(ImageDataTestSuite, ImageDataTest)
{
    auto imageDataPtr = std::make_unique<ImageData>();
    EXPECT_NE(imageDataPtr, nullptr);
}

