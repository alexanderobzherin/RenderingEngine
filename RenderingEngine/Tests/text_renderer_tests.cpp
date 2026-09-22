#include <stdio.h>
#include <cstdint>
#include "gtest/gtest.h"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::TestSuite;
using ::testing::UnitTest;

TEST(TextRendererTestSuite, TextRendererTest)
{
    EXPECT_EQ(0, 0);
}
