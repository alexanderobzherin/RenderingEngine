#include <stdio.h>

#include "gtest/gtest.h"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::TestSuite;
using ::testing::UnitTest;

TEST(InitialTest, InitialTest01) 
{
  EXPECT_EQ(0, 0);
}

int main(int argc, char** argv) 
{
    InitGoogleTest(&argc, argv);
    //UnitTest& unit_test = *UnitTest::GetInstance();

    return RUN_ALL_TESTS();;
}