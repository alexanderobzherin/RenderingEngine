#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>
#include "gtest/gtest.h"
#include <random>

#include "app_clock.hpp"
#include "app_time.hpp"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::TestSuite;
using ::testing::UnitTest;

TEST(AppClockTestSuite, AppClockTest)
{
    EXPECT_EQ(0, 0);
}
