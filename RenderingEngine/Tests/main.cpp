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

int main(int argc, char** argv) 
{
    InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();;
}