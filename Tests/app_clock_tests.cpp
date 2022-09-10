#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>
#include "gtest/gtest.h"
#include <random>

#include "../RenderingEngine/Include/app_clock.hpp"
#include "../RenderingEngine/Include/app_time.hpp"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::TestSuite;
using ::testing::UnitTest;

using namespace rendering_engine;

TEST(AppClockTest, AppClockTest1)
{
    // Usage of AppClock and AppTime;
    AppClock appClock;
    AppTime appTime;

    appClock.Reset();

    unsigned int const numOfIter = 10U;

    std::random_device randomDevice;
    std::default_random_engine randomGenerator(randomDevice());
    std::uniform_int_distribution<int> probDistribution(5000, 15000);
    
    for( unsigned int i = 0U; i < numOfIter; ++i )
    {
        // Here we simulate some work per frame
        auto const sleepFor = probDistribution(randomGenerator);
        std::chrono::milliseconds delay{ std::chrono::milliseconds (sleepFor)};
        std::this_thread::sleep_for(std::chrono::milliseconds{ delay });

        // Now we update our time
        appClock.UpdateAppTime(appTime);

        auto const delayTimeMilliseconds = std::chrono::duration_cast<std::chrono::duration<float>>(delay).count();
        auto const elapsedTimeMilliseconds = appTime.ElapsedAppTimeMilliseconds();
        std::cout << "Delay: " << delayTimeMilliseconds << "Elapsed app time: " << elapsedTimeMilliseconds << std::endl;

        EXPECT_TRUE(delayTimeMilliseconds <= elapsedTimeMilliseconds);
    }

}