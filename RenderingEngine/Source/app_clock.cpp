#include "app_clock.hpp"
#include "app_time.hpp"

namespace rendering_engine
{

AppClock::AppClock()
:
mStartTime(),
mCurrentTime(),
mLastTime()
{
	Reset();
}

const std::chrono::high_resolution_clock::time_point& AppClock::StartTime() const
{
	return mStartTime;
}

const std::chrono::high_resolution_clock::time_point& AppClock::CurrentTime() const
{
	return mCurrentTime;
}

const std::chrono::high_resolution_clock::time_point& AppClock::LastTime() const
{
	return mLastTime;
}

void AppClock::Reset()
{
	mStartTime = std::chrono::high_resolution_clock::now();
	mCurrentTime = mStartTime;
	mLastTime = mCurrentTime;
}

void AppClock::UpdateAppTime(AppTime& appTime)
{
	mLastTime = mCurrentTime;
	mCurrentTime = std::chrono::high_resolution_clock::now();
	
	appTime.SetCurrentTime( mCurrentTime );
	appTime.SetTotalAppTime( std::chrono::duration_cast<std::chrono::milliseconds>(mCurrentTime - mStartTime) );
	appTime.SetElapsedAppTime( std::chrono::duration_cast<std::chrono::milliseconds>(mCurrentTime - mLastTime) );
}

} //rendering_engine