#include "app_time.hpp"

namespace rendering_engine
{

AppTime::AppTime()
:
mTotalAppTime(0),
mElapsedAppTime(0)
{
}

const std::chrono::high_resolution_clock::time_point& AppTime::CurrentTime() const
{
	return mCurrentTime;
}

void AppTime::SetCurrentTime(const std::chrono::high_resolution_clock::time_point& currentTime)
{
	mCurrentTime = currentTime;
}

const std::chrono::milliseconds& AppTime::TotalAppTime() const
{
	return mTotalAppTime;
}

const float AppTime::TotalAppTimeMilliseconds() const
{
	return std::chrono::duration_cast<std::chrono::duration<float>>(TotalAppTime()).count();
}

void AppTime::SetTotalAppTime(const std::chrono::milliseconds& totalAppTime)
{
	mTotalAppTime = totalAppTime;
}

const float AppTime::ElapsedAppTimeSeconds() const
{
	return std::chrono::duration_cast<std::chrono::duration<float>>(ElapsedAppTime()).count();
}

const std::chrono::milliseconds& AppTime::ElapsedAppTime() const
{
	return mElapsedAppTime;
}

void AppTime::SetElapsedAppTime(const std::chrono::milliseconds& elapsedAppTime)
{
	mElapsedAppTime = elapsedAppTime;
}

} //rendering_engine